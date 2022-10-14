// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------
char *User2System(int virtAddr, int limit)
{
	int i; // index
	int oneChar;
	char *kernelBuf = NULL;
	kernelBuf = new char[limit + 1]; // need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf, 0, limit + 1);
	// printf("\n Filename u2s:");
	for (i = 0; i < limit; i++)
	{
		kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		// printf("%c",kernelBuf[i]);
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}
int System2User(int virtAddr, int len, char *buffer)
{
	if (len < 0)
		return -1;
	if (len == 0)
		return len;
	int i = 0;
	int oneChar = 0;
	do
	{
		oneChar = (int)buffer[i];
		kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);
	return i;
}
void IncreasePC()
{
	{
		/* set previous programm counter (debugging only)*/
		kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

		/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
		kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

		/* set next programm counter for brach execution */
		kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
	}
}
void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;

		case SC_Add:
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			/* Modify return point */
			IncreasePC();

			return;

			ASSERTNOTREACHED();

			break;
		case SC_Create:
		{
			int virtAddr;
			char *filename;
			DEBUG(dbgFile, "\n SC_Create call ...");
			DEBUG(dbgFile, "\n Reading virtual address of filename");
			// check for exception
			virtAddr = kernel->machine->ReadRegister(4);
			DEBUG(dbgFile, "\n Reading filename.");
			filename = User2System(virtAddr, 33); // MaxFileLength là = 32
			if (filename == NULL)
			{
				printf("\n Not enough memory in system");
				DEBUG(dbgFile, "\n Not enough memory in system");
				kernel->machine->WriteRegister(2, -1); // trả về lỗi cho chương trình người dùng
				delete filename;
				return;
			}
			DEBUG(dbgFile, "\n Finish reading filename.");
			// DEBUG(dbgFile,"\n File name : '"<<filename<<"'");
			//  Create file with size = 0
			//  Dùng đối tượng fileSystem của lớp OpenFile để tạo file, việc tạo file này là sử dụng các
			//  thủ tục tạo file của hệ điều hành Linux, chúng ta không quản ly trực tiếp các block trên
			//  đĩa cứng cấp phát cho file, việc quản ly các block của file trên ổ đĩa là một đồ án khác
			if (!kernel->fileSystem->Create(filename, 0))
			{
				printf("\n Error create file '%s'", filename);
				kernel->machine->WriteRegister(2, -1);
				delete filename;
				return;
			}
			kernel->machine->WriteRegister(2, 0); // trả về cho chương trình người dùng thành công
			delete filename;
			return;
		}
			// xử lý syscall ReadNum
		case SC_ReadNum:
		{
			 int num;
            num = SysReadNum();                          // system read integer number
            kernel->machine->WriteRegister(2, (int)num); // write the return value to register 2

            IncreasePC();
            return;

            ASSERTNOTREACHED();
            break;
		}
		// xử lý syscall PrintNum
		case SC_PrintNum:
		{
			int num = (int)kernel->machine->ReadRegister(4); // get the number to print from register 4
            SysPrintNum(num);                                // system print number

            IncreasePC();
            return;

            ASSERTNOTREACHED();
            break;
		}
			// xử lý syscall ReadChar
		case SC_ReadChar:
		{
			char c;
			c = SysReadChar();					  // read a character
			kernel->machine->WriteRegister(2, c); // write the return value to register 2

			IncreasePC();

			return;
			ASSERTNOTREACHED();
			break;
		}
		// xu ly syscall PrintChar
		case SC_PrintChar:
		{
			char c = kernel->machine->ReadRegister(4); // get the character to print from register 4
			SysPrintChar(c);						   // print character

			IncreasePC();

			return;
			ASSERTNOTREACHED();
			break;
		}
		// xử lý syscall RandomNum
		case SC_RandomNum:
		{
			int a;
			a = RandomNumber();
			kernel->machine->WriteRegister(2, a);

			IncreasePC();

			return;
			ASSERTNOTREACHED();
			break;
		}
		// xử lý syscall ReadString
		case SC_ReadString:
		{
			int virtualAddr;
			char *buffer;
			int length;
			virtualAddr = kernel->machine->ReadRegister(4); // get buffer' address
			length = kernel->machine->ReadRegister(5);		// maximum length of input string
			buffer = User2System(virtualAddr, length);		// copy string from User space to Kernel space
			SysReadString(buffer, length);					// system read string
			System2User(virtualAddr, length, buffer);		// return string to User space
			delete buffer;

			IncreasePC();
			return;

			ASSERTNOTREACHED();
			break;
		}
		// xử lý syscall PrintString
		case SC_PrintString:
		{
			int virtualAddr = kernel->machine->ReadRegister(4); // get address of buffer
			char *buffer = User2System(virtualAddr, 255);		// copy string (max 255 byte) from User space to Kernel space
			SysPrintString(buffer);								// print string
			delete[] buffer;

			IncreasePC();
			return;

			ASSERTNOTREACHED();
			break;
		}
		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
