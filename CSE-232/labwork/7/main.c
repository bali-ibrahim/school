#include <stdio.h>
#include <time.h>
#include "oddEven.h"

#define MulFive(x) ((5) * (x))
#define TEST_ITERATIONS 1000000

int mulFive(int x)
{
    return 5 * x;
}

int main(void)
{
    int arg;
    scanf("%d", &arg);
    int fiveTimes = MulFive(arg);
    printf("%d\n", arg);
    printMaxOfEvenNumbers(fiveTimes);
    printMaxOfOddNumbers(fiveTimes);


    clock_t start, end;
    float cpu_time_used_m, cpu_time_used_f;
    int f, m;
    start = clock();
    for (int i = 0; i < TEST_ITERATIONS; i++)
    {
        m = MulFive(arg);
    }
    end = clock();
    cpu_time_used_m = ((double)(end - start)) / CLOCKS_PER_SEC;

    start = clock();
    for (int i = 0; i < TEST_ITERATIONS; i++)
    {
        f = mulFive(arg);
    }
    end = clock();
    cpu_time_used_f = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Macro calculation was %d seconds faster than subroutine call.", (cpu_time_used_f - cpu_time_used_m));

    return 0;
}
