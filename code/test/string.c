#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int main()
{
    char str[256];
    PrintString("String lenth < 256: ");
    ReadString(str, 3);
    PrintString(str);
    Halt();
}