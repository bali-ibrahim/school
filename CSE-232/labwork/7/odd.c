#include <stdio.h>
void printMaxOfOddNumbers(int arg)
{
    if (!(arg & 1)) --arg;
    printf("MaxOdd: %d\n", arg);
}
