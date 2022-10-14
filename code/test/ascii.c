#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int main()
{
    int i = 32;
    while (i <= 255)
    {
        PrintNum(i);
        PrintChar(' ');
        PrintChar((char)i);
        PrintChar('\n');
        i = i + 1;
    }
    Halt();
}