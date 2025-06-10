#include <stdio.h>
#include <stdlib.h> //динамическая память
#include <string.h> //строки

/*прототипы функций*/
char** input(long long int* N); //ввод
char* inp_str(); //ввод одной строки
void sort(char** mass, long long int N); //сортировка
int srt_comparison(char* C1, char* C2); //сравнение двух строк
void output(char** mass, long long int N); //вывод

int main(void)
{
    long long int n = 0; //кол-во строк
    char** massive = NULL; //выделили память для одного символа

    //printf("Введите строки через Enter:\n");
    massive = input(&n);
    sort(massive, n);
    output(massive, n);

    for (long long int i = 0; i < n; i++)
        free(massive[i]);

    free(massive);
    return 0;
}

char** input(long long int* N)
{
    long long int L = 1; //C - символ, L - кол-во строк
    char** arr = NULL;
    int a;
    arr = (char**)malloc(L * sizeof(char*));
    arr[0] = NULL;
    while (1)
    {
        arr[L-1] = inp_str(&a);
        if (a == EOF)
        {
			*N = L;
			break;
		}
        if (arr[L-1] == NULL)
        {
            L--;
            *N = L;
            break;
        }
        L++;
        arr = (char**)realloc(arr, L * sizeof(char*));
        if (arr == NULL)
        {
            printf("Ошибка выделения памяти \n");
            exit(1);
        }
        arr[L-1] = NULL;
    }
    return arr;
}

char* inp_str(int* a)
{
    long long int L = 1, symb;
    char* C = NULL;
    *a = 1;
    C = malloc(L*sizeof(char));
    if (C == NULL)
    {
        return '\0';
    }
    C[L-1] = '\0';
    while (1)
    {
        symb = getchar();
        if (symb == EOF)
        {
            *a = EOF;
            return C;
        }
        if (symb == '\n')
            {break;}
        
        C[L-1] = symb;
        L++;
        C = realloc(C, L*sizeof(char));
        if (C == NULL)
        {
            printf("Ошибка выделения памяти \n");
            exit(1);
        }
        C[L-1] = '\0';
    }

    return C;
}

void sort(char** mass, long long int N) //по возрастанию
{
    char* P;
    for (long long int i = 0; i < N-1; i++)
    {
        for (long long int j = 0; j < N-1-i; j++)
        {
            //if (srt_comparison(mass[j], mass[j+1])) //m[j] > m[j+1] => выполняется
            if (strcmp(mass[j], mass[j+1]) > 0)
            {
                P = mass[j];
                mass[j] = mass[j+1];
                mass[j+1] = P;
            }
        }
    }
}

/*int srt_comparison(char* C1, char* C2)
{
    char* pointer1 = C1;
    char* pointer2 = C2;
    while ((*pointer1 != '\0') && (*pointer2 != '\0'))
    {
        if (*pointer1 != *pointer2)
        {
            int x = *pointer1 > *pointer2;
            return x;
        }
        pointer1++;
        pointer2++;
    }
    return *pointer1 != 0; //если пустая строка
}*/

void output(char** mass, long long int N)
{
    char* P = NULL;
    if (N == 0)
    {
        printf("Вы не ввели ни одной строки\n");
        return;
    }
    //printf("Отсортированные (по возрастанию) строки:\n");
    for (long long int i = 0; i < N; i++)
    {
        //printf("<");
        P = *(mass + i);
        while (*P != '\0')
        {
            printf("%c", *P);
            P++;
        }
        //printf(">\n");
        printf("\n");
    }
    return;
}
