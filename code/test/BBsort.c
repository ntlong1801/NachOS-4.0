#include "syscall.h"
#include "copyright.h"
#define maxlen 32
int main()
{
    int arr[100];
    int n, i, j, tmp, check;
    // PrintString("Nhap so luong phan tu(<= 100): ");
    // n = ReadNum();
    do
    {
        PrintString("Nhap so luong phan tu(<= 100): ");
        n = ReadNum();
        if (n <= 100 && n > 0)
        {
            break;
        }

        PrintString("Nhap khong dung! \n");
    } while (1);
    // if(n > 100) {
    //     PrintString("So luong mang lon hon 100!");
    //     return 0;
    // }

    for (i = 0; i < n; i++)
    {
        PrintString("arr[");
        PrintNum(i);
        PrintString("]= ");
        arr[i] = ReadNum();
    }

    /* then sort! */
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < (n - 1); j++)
        {
            if (arr[j] > arr[j + 1])
            { /* out of order -> need to swap ! */
                tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }

    PrintString("sort tang dan (0) hoac giam dan (1): ");
    check = ReadNum();
    if (check != 0 && check != 1)
    {
        PrintString("Nhap sai lua chon!");
        return 0;
    }
    else if (check == 1)
    {
        for (i = 0; i < n; i++)
        {
            arr[i] = arr[n - 1] - i;
        }
    }

    for (i = 0; i < n; i++)
    {
        PrintNum(arr[i]);
        PrintChar(' ');
    }
    Halt();
}
