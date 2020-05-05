#include <stdio.h>
void printMaxOfEvenNumbers(int arg)
{
    if (arg & 1) --arg;
    printf("Max Even: %d\n", arg);
}
