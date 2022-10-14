#include "syscall.h"

int main()
{
    int i = 0;
    for(i = 32;i<127;i++)
        {
            PrintNum(i);
            PrintChar(' ');
            PrintChar((char)i);
            PrintChar('\n');
        }
}