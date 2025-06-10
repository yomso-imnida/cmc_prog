#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <signal.h>

#define SYMBOLS "&|><;()"
#define doubleSYMBOLS "&|>"

void free_memory(char** arr, long long int N);
void InputAndPrint(FILE* FP_I, FILE* FP_O);
char* Inp_Str(int* a, FILE* FP_I, long long int* N);
char** Processing_String(char* str, long long int L, long long int* newL);
int Special_Characters(int symb);
void Print_String(char** arr, long long int N, FILE* FP_O);
int execution_line(char** arr, long long int N);

int main(int argc, char **argv)
{
    signal(SIGINT, SIG_IGN);
    FILE* FP_O = stdout;
    FILE* FP_I;
    if (argc > 1) {
        char* f_i = argv[1];
        FP_I = fopen(f_i, "r");
    }
    else
        FP_I = stdin;
    InputAndPrint(FP_I, FP_O);
    fclose(FP_I);
    return 0;
}

void free_memory(char** arr, long long int N)
{   
    for (long long int i = 0; i < N; i++) {
        free(arr[i]);
    }
    free(arr);
    return;
}

/*------------------------Input------------------------*/
void InputAndPrint(FILE* FP_I, FILE* FP_O)
{
    long long int L = 0;
    int a;
    char** new_str = NULL;
    while(1)
    {
		//printf(">> ");
		printf("%s: ", getcwd(NULL, 0));
        char* str = Inp_Str(&a, FP_I, &L);
        if (a == EOF && str == NULL)
			break;
        if (str == NULL) {
            //perror("Вы не ввели ни одного символа\n");
            continue;
        }
        long long int newL = 0;
        if (L != 1) {
            new_str = Processing_String(str, L, &newL);
            //Print_String(new_str, newL, FP_O);
            execution_line(new_str, newL);
            free_memory(new_str, newL);
        }
        free(str);
        if (a == EOF)
			break;
        L = 0;
    }
    return;
}

char* Inp_Str(int* a, FILE* FP_I, long long int* N)
{
    long long int L = 1;
    int symb;
    char* C = NULL;
    *a = 1;
    C = (char*)malloc(L*sizeof(char));
    if (C == NULL)
        return C;
    C[L-1] = '\0';

    while (1) {
        symb = fgetc(FP_I);
        if (symb == EOF) {
            *a = EOF;
            break;
        }
        if (symb == '\n')
            break;
        C[L-1] = symb;
        L++;
        C = (char*)realloc(C, L*sizeof(char));
        if (C == NULL) {
            printf("Ошибка выделения памяти\n");
            exit(1);
        }
        C[L-1] = '\0';
    }
    *(N) = L;
    return C;
}

/*------------------Processing String------------------*/
char** Processing_String(char* str, long long int L, long long int* newL)
{
    int counter = 1, word_cout = 1;
    char* word = NULL;
    word = (char*)malloc(1 * sizeof(char));
    word[0] = '\0';
    char** new_arr = NULL;
    new_arr = (char**)malloc(1 * sizeof(char*));
    new_arr[0] = NULL;
    int symb, pred_symb = 'f';
    int FLAG = 0; //флаг двойных ковычек; 1 - ковычки открылись
    int FL_SC = 0; //флаг спец. симв.; 1 - нашли первый спец. симв.

    for (long long int i = 0; i < L; i++) {
        symb = str[i];
        if (symb == '"') {
            FLAG = 1 - FLAG;
            continue;
        }
        if(FLAG == 1) {
            word[word_cout-1] = symb;
            word_cout++;
            word = (char*)realloc(word, word_cout*sizeof(char));
            if (word == NULL) {
                perror("Ошибка выделения памяти\n");
                exit(1);
            }
            word[word_cout-1] = '\0';
        }
        else if (FL_SC == 1) {
            FL_SC = Special_Characters(symb);
            if (FL_SC) {
                //проверка, что это двойной спец симв, а не просто два спец симв
                //если просто два спец симв, то сохраняем, как отдельные слова
                if ((symb == pred_symb) && (strchr(doubleSYMBOLS, symb))) {
                    word[word_cout-1] = symb;
                    word_cout++;
                    word = (char*)realloc(word, word_cout*sizeof(char));
                    word[word_cout-1] = '\0';
                    new_arr[counter-1] = word;
                    counter++;
                    new_arr = (char**)realloc(new_arr, counter*sizeof(char*));
                    word = (char*)malloc(1 * sizeof(char));
                    word_cout = 1;
                    word[word_cout-1] = '\0';
                    FL_SC = 0;
                    continue;
                }
                else {
                    new_arr[counter-1] = word;
                    counter++;
                    new_arr = (char**)realloc(new_arr, counter*sizeof(char*));
                    word = (char*)malloc(2 * sizeof(char));
                    word_cout = 2;
                    word[0] = symb;
                    word[1] = '\0';
                    pred_symb = symb;
                    continue;
                }
            }
            else {
                new_arr[counter-1] = word;
                counter++;
                new_arr = (char**)realloc(new_arr, counter*sizeof(char*));
                if (!isspace(symb)) {
					word = (char*)malloc(2 * sizeof(char));
					word_cout = 2;
					word[0] = symb;
					word[1] = '\0';
				}
				else {
					word = (char*)malloc(1* sizeof(char));
					word_cout = 1;
					word[0] = '\0';
				}
                pred_symb = symb;
            }
        }
        else if ((FL_SC = Special_Characters(symb))) {
            if (word_cout > 1) {
                new_arr[counter-1] = word;
                counter++;
                new_arr = (char**)realloc(new_arr, counter*sizeof(char*));
            }
            else
                free(word);
            word = (char*)malloc(2 * sizeof(char));
            word_cout = 2;
            word[0] = symb;
            word[1] = '\0';
            pred_symb = symb;
        }
        else if ((isspace(symb)) && (!FLAG)) {
            if (word_cout > 1) {
                new_arr[counter-1] = word;
                counter++;
                new_arr = (char**)realloc(new_arr, counter*sizeof(char*));
                word = (char*)malloc(1 * sizeof(char));
                word_cout = 1;
                word[0] = '\0';
                pred_symb = symb;
                continue;
            } 
        }
        else {
            word[word_cout-1] = symb;
            word_cout++;
            word = (char*)realloc(word, word_cout*sizeof(char));
            if (word == NULL) {
                perror("Ошибка выделения памяти\n");
                exit(1);
            }
            word[word_cout-1] = '\0';
        }
    }
    new_arr[counter-1] = word; 
    *(newL) = counter;
    return new_arr;
}

/*------------Check for control characters------------*/
//возвращает 1, если спец симв, возвр. 0, если не спец симв
int Special_Characters(int symb)
{
    char* F = strchr(SYMBOLS, symb);
    if (F != NULL)
        return 1;
    else
        return 0;
}

/*--------------------Print String--------------------*/
void Print_String(char** arr, long long int N, FILE* FP_O)
{
    char* C = NULL;
    fprintf(FP_O, "\n");
    for (long long int i = 0; i < N; i++) {
        C = arr[i];
        fprintf(FP_O, "%s\n", C);
    }
    return;
}


/*-----------------ВТОРАЯ ЧАСТЬ SHELL-----------------*/
int execution_line(char** arr, long long int N)
{
    pid_t pid;
    if (strcmp(arr[0], "cd") == 0) {
        if (N == 2) {
            if (chdir(getenv("HOME")) != 0) {
                perror("Error chdir\n");
            }
        }
        else {
            if (chdir(arr[1]) != 0) {
                perror("Error chdir\n");
            }
        }
    }
    else {
        pid = fork();
        if (pid < 0) {
            perror("Error fork\n");
            return -1;
        }
        else if (pid == 0) {
            arr[N-1] = (char*)0;
            signal(SIGINT, SIG_DFL);
            execvp(arr[0], arr);
            perror("Error execvp\n");
            exit(0);
        }
        wait(NULL);
    }
    return 0;
}
