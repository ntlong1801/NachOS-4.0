#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int main()
{
    char a[256];
    PrintString("String length: (<= 255):\n");
    ReadString(a, ReadNum());
    // PrintNum(ReadNum());
    PrintString(a);
    Halt();
}