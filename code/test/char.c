#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int main()
{
    PrintChar(ReadChar());
    Halt();
}