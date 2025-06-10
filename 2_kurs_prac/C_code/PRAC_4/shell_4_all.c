#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SYMBOLS "&|><;()"
#define doubleSYMBOLS "&|>"

void free_memory(char** arr, long long int N);
void InputAndPrint(FILE* FP_I, FILE* FP_O);
char* Inp_Str(int* a, FILE* FP_I, long long int* N);
char** Processing_String(char* str, long long int L, long long int* newL);
int Special_Characters(int symb);
void Print_String(char** arr, long long int N, FILE* FP_O);
int execution_line(char** arr, long long int N);
int i_o_redirection(char** arr, long long int counter);
int is_there_conveyor(char*** arr);
int conveyor_n_processes(char** arr, long long int counter, int num_conv);
int is_there_background_process(char*** arr);
void prepare_for_execution(char** commands_line, int number_of_words);
void rid_of_brackets(char*** commands, int* number_of_commands, int offset);

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
		//printf("%s: ", getcwd(NULL, 0));
        char path[100];
        getcwd(path, 100);
        fprintf(stdout, "%s: ", path);

        char* str = Inp_Str(&a, FP_I, &L);
        /*if (a == EOF && str == NULL)
			break;
        if (str == NULL) {
            //perror("Вы не ввели ни одного символа\n");
            continue;
        }*/
        long long int newL = 0;
        //if (L != 1) {
            new_str = Processing_String(str, L, &newL);
            //Print_String(new_str, newL, FP_O);
            prepare_for_execution(new_str, newL);
            //execution_line(new_str, newL);       
			free_memory(new_str, newL);
        //}
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
    free(word);
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
    arr[N] = (char*)0;
    if(N == 0) return 1;
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
        int num_amp = is_there_background_process(&arr);
        if(num_amp > 1) {
            perror("Error: more than two ampersands (&)\n");
            return -1;
        }
        pid = fork();
        if (pid < 0) {
            perror("Error: fork\n");
            return -1;
        }
        else if (pid == 0) {
            signal(SIGINT, SIG_DFL);
            if(num_amp) {
                signal(SIGINT, SIG_IGN);
                int fd = open("/dev/null", O_RDWR);
                dup2(fd, 0);
                close(fd);
            }
            int num_conv = is_there_conveyor(&arr);
            if(num_conv == 1) {
                if(i_o_redirection(arr, N)) {
                    exit(2);
                }
                //скобки
                if (strcmp(arr[0], "(") == 0) {
                    rid_of_brackets(&arr, (int*)&N, 0);
                    while (wait(NULL)!=-1);
                    exit(0);
                }
                else {
                    execvp(arr[0], arr);
                }    
            }
            else if(num_conv > 1) {
                conveyor_n_processes(arr, N, num_conv);
            }
            perror("Error execvp\n");
            exit(0);
        }
        if(!num_amp) {
            waitpid(pid, NULL, 0);
        }
    }
    return 0;
}

/*-----------------ТРЕТЬЯ ЧАСТЬ SHELL-----------------*/
//перенаправление ввода-вывода
int i_o_redirection(char** arr, long long int counter)
{
    char* word = " ";
    char* pred_word = " ";
    for (int i = 0; arr[i] != NULL; i++) {
        word = arr[i];
        if (strcmp(pred_word, "<") == 0) {
            int fd = open(word, O_RDONLY);
            if(fd == -1) {
                perror("Error: open file\n");
                return 1;
            }
            dup2(fd, 0);
            close(fd);
            int j = 0;
            for (j = i; arr[j] != NULL; j++) {
                arr[j-1] = arr[j+1];
            }
            arr[j-1] = arr[j-2] = NULL;
            i--;
        }
        if (strcmp(pred_word, ">") == 0) {
            int fd = open(word, O_WRONLY | O_CREAT | O_TRUNC, 
                S_IRWXU | S_IRWXG | S_IRWXO);
            /*O_TRUNK - если файл уже существует, он является обычным файлом и режим позволяет записывать в этот файл
            (т.е. установлено O_RDWR или O_WRONLY), то его длина будет урезана до нуля.
            Если файл является каналом FIFO или терминальным устройством, то этот флаг игнорируется.
            Иначе действие флага O_TRUNC не определено.
            S_IRWXU - пользователь (владелец файла) имеет права на чтение, запись и выполнение файла
            S_IRWXG - группа имеет права на чтение, выполнение файла и запись в него информации
            S_IRWXO - все остальные имеют права на чтение, выполнение файла и запись в него информации*/
            //int fd = open(word, O_WRONLY | O_CREAT | O_TRUNC, 0777);
            if(fd == -1) {
                perror("Error: open or creat file\n");
                return 1;
            }
            dup2(fd, 1);
            close(fd);
            int j = 0;
            for (j = i; arr[j] != NULL; j++) {
                arr[j-1] = arr[j+1];
            }
            arr[j-1] = arr[j-2] = NULL;
            i--;
        }
        if (strcmp(pred_word, ">>") == 0) {
            int fd = open(word, O_WRONLY | O_CREAT | O_APPEND, 
                S_IRWXU | S_IRWXG | S_IRWXO);
            /*O_APPEND - Файл открывается в режиме добавления.
            Перед каждой операцией write файловый указатель будет устанавливаться в конце файла,
            как если бы использовался lseek*/
            //int fd = open(word, O_WRONLY | O_CREAT | O_APPEND, 0777);
            if(fd == -1) {
                perror("Error: open or creat file\n");
                return 1;
            }
            dup2(fd, 1);
            close(fd);
            int j = 0;
            for (j = i; arr[j] != NULL; j++) {
                arr[j-1] = arr[j+1];
            }
            arr[j-1] = arr[j-2] = NULL;
            i--;
        }
        pred_word = word;
    }
    return 0;
}

int is_there_conveyor(char*** arr)
{
    int N = 1;
    for (int i = 0; (*arr)[i] != NULL; i++) {
        if(strcmp((*arr)[i], "|") == 0) {
            N++;
            (*arr)[i] = NULL;
        }
    }
    return N;
}

int conveyor_n_processes(char** arr, long long int counter, int num_conv)
{
    int fd[2], j = 0;
    for (int i = 0; i < num_conv; i++) {
        pipe(fd);
        switch(fork()) {
            case -1:
                perror("Error: fork in conveyor\n");
                exit(0);
            case 0:
                if(i != num_conv-1)
                    dup2(fd[1], 1);
                close(fd[0]);
                close(fd[1]);
                if(i_o_redirection(arr, counter)) 
                    exit(2);
                //скобки
                if (strcmp(arr[j], "(") == 0) {
                    int temp = counter-j;
                    rid_of_brackets(&arr, &temp, j);
                    while (wait(NULL)!=-1);
                    exit(0);
                }
                else {
                    execvp(arr[j], arr+j);
                }
                perror("Error: the conveyor is failed\n");
                exit(0);
        }
        dup2(fd[0], 0);
        close(fd[0]);
        close(fd[1]);
        while (arr[j++] != NULL); //ищем след. команду
    }
    while (wait(NULL) != -1);
    exit(0);
}

int is_there_background_process(char*** arr)
{
    int num_amp = 0, i = 0;
    while ((*arr)[i]) {
        if (strcmp((*arr)[i], "&") == 0) {
            (*arr)[i] = NULL;
            num_amp++;
            //сдвигаем указатель если амперсанд не последний
            int j = i+1;
            while ((*arr)[j]) {
                (*arr)[j-1] = (*arr)[j];
                j++;
            }
            (*arr)[j-1] = NULL;
            i--;
        }
        i++;
    }
    return num_amp;
}

/*----------------ПОСЛЕДНЯЯ ЧАСТЬ SHELL----------------*/

void prepare_for_execution(char** commands_line, int num_words)
{
    //отлавливаем завершение фонового процесса
    int status;
    pid_t pid_bg_pr = waitpid(0, &status, WNOHANG);
    if(pid_bg_pr > 0) {
        if(WIFEXITED(status)) {
            printf("-----Process %d exited with code %d-----\n",
                pid_bg_pr, WEXITSTATUS(status));
        }
        else {
            printf("-----Process %d aborted by signal %d-----\n",
                pid_bg_pr, WEXITSTATUS(status));
        }
    }
    commands_line[num_words-1] = (char*)0;
    if(num_words == 0) return;

    //если пустая строка
    if (!(commands_line)[0]) {
		return;
	}

    //делим введенную строку на отдельные команды, если есть разделители ; && ||
    char*** commands = calloc(num_words,sizeof(char**));
    char** separators = calloc(num_words,sizeof(char*));
    int* n_commands_cur = calloc(num_words, sizeof(int));
    int n_commands = 0, n_separators = 0;
    int n_opened_brackets = 0, status_finished_process;
    for (int i = 0; i < num_words && commands_line[i]; i++) {
        commands[i] = calloc(num_words, sizeof(char*));
        if (strcmp(commands_line[i], "(")==0) {
            n_opened_brackets++;
        }
        if (strcmp(commands_line[i], ")")==0) {
            n_opened_brackets--;
        }
        if ((strcmp(commands_line[i], "||")==0 || strcmp(commands_line[i], "&&")==0
           || strcmp(commands_line[i], ";")==0) && i && !n_opened_brackets) {
            separators[n_separators++] = commands_line[i];
            n_commands++;
        }
        else {
            commands[n_commands][n_commands_cur[n_commands]++] = commands_line[i];
        }
    }
    n_commands++;
    //запускаем все команды в нужном порядке, учитывая разделители
    for (int i = 0; i < n_commands; i++) {
        status_finished_process = execution_line(commands[i], n_commands_cur[i]);
        if (i == n_commands-1) {
            continue;
        }
        if (WIFEXITED(status_finished_process) && !WEXITSTATUS(status_finished_process)
        && !strcmp(separators[i], "||")) { //success
            while (i < n_separators && strcmp(separators[i++], ";"));
        }
        else if ((!WIFEXITED(status_finished_process) || WEXITSTATUS(status_finished_process))
             && !strcmp(separators[i], "&&")) {
            while (i < n_separators && strcmp(separators[i++], ";"));
        }
    }
    free(n_commands_cur);
    free(separators);
    for (int i = 0; i < num_words; i++) {
        free(commands[i]);
    }
    free(commands);
}

void rid_of_brackets(char*** commands, int* n_commands, int offset) {
    free((*commands)[offset]);
    int j = 1+offset;
    while ((*commands)[j]) {
        (*commands)[j-1] = (*commands)[j];
        j ++;
    }
    (*commands)[j-1] = NULL;
    if (strcmp((*commands)[j-2], ")")==0) {
        free((*commands)[j-2]);
        (*commands)[j-2] = NULL;
    }
    prepare_for_execution(*commands+offset, *n_commands-2);
}
