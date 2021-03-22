#include <stdio.h>
#include <time.h>
#include <ctime>
#include <conio.h>
#include <Windows.h>
#include <stdlib.h>
#define lenght 96
#define iteration 50000

void CWithoutO(int arr[lenght][lenght]);

int main()
{
    int arr[lenght][lenght], stime, i, j, k;
    int intSize = sizeof(int);
    int doubleIntSize = sizeof(int) * 2;
    long ltime, sum = 0;
    clock_t start, end;
    double currentTime;
    /* получение текущего календарного времени */
    ltime = time(NULL);
    stime = (unsigned int)ltime / 2;
    srand(stime);
    for (i = 0; i < lenght; i++)
        for (j = 0; j < lenght; j++)
        {
            arr[i][j] = rand();
        }

    CWithoutO(arr);

    start = clock();
    for (k = 0; k < iteration; k++)
    {
        sum = 0;
        for (i = 0; i < lenght; i++)
        {
            for (j = 0; j < lenght; j++)
            {
                sum += arr[i][j];
            }
        }
    }
    printf("\nC with optimization: %d\n", sum);
    end = clock();
    currentTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time: %lf\n", currentTime);
    int* myptr = &arr[0][0];
    sum = 0;
    i = 0;
    int l = lenght * lenght;

    start = clock();
    for(k = 0; k < iteration; k++)
    _asm
    {
        xor esi, esi
        xor eax, eax
        xor edi, edi
        xor ebx, ebx
        mov ecx, l
        mov esi, myptr
    cycle:
        add eax, [esi]
        add esi, 4
        loop cycle
        mov sum, eax
    }
    printf("\nAccembler: %d\n", sum);
    end = clock();
    currentTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time: %lf\n", currentTime);

    
    int MMXCounter = lenght * lenght * intSize * doubleIntSize / 64;
    int count = iteration;
    int mas[2];

    start = clock();
    while (count--)
    {
        __asm
        {
            mov ecx, MMXCounter
            xor esi, esi
            pxor mm1, mm1
            startq :
            movq mm0, arr[esi]
                paddd mm1, mm0
                add esi, 8
                loop startq
                movq mas, mm1
                emms
        }
        sum = mas[0] + mas[1];
    }
    end = clock();
    printf("\nMMX: %d\n", sum);
    currentTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time: %lf\n", currentTime);
}



#pragma optimize("",off)
void CWithoutO(int arr[lenght][lenght]) {
    int i, j, k;
    long ltime, sum = 0;
    clock_t start, end;
    double currentTime;
    start = clock();
    for (k = 0; k < iteration; k++)
    {
        sum = 0;
        for (i = 0; i < lenght; i++)
        {
            for (j = 0; j < lenght; j++)
            {
                sum += arr[i][j];
            }
        }
    }
    printf("\nC without optimization: %d\n", sum);
    end = clock();
    currentTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time: %lf\n", currentTime);

}
#pragma optimize("",on)
