#include <stdio.h>
#include <string.h> //строки
#include <stdlib.h> //динамическая память
#include <ctype.h> //обработка символов

struct TREE
{
    char* key; //ключ (слово)
    double frequency; //сколько раз встретилось
    struct TREE *left;
    struct TREE *right;
};

/*прототипы функций*/
struct TREE* input(int* N); //ввод
struct TREE* AddElement(char* key, struct TREE* Br); //добавление элемента в дерево
struct TREE* AddNode(struct TREE* node, struct TREE* Br); //новый узел
struct TREE* NewBranch(char* K); //новая ветка
struct TREE* NewTree(struct TREE* node); //новая ветка из старой
struct TREE* GetNodes(struct TREE* Br, struct TREE* newBr); //создание нового дерева
void output(struct TREE* list, int n);
void FreeMemory(struct TREE* list, int WordFreed);

int main(void)
{
    int n = 0;
    struct TREE* root = input(&n);
    
    if (n != 0)
    {
		struct TREE* new = NULL;
		new = GetNodes(root, new);
		output(new, n);

		FreeMemory(new, 1);
		FreeMemory(root, 0);
	}
	return 0;
}

//ввод
struct TREE* input(int* N)
{
    struct TREE* branch = NULL;
    int symb;
    int len = 1;
    int space = 0;
    char* word = NULL;
    word = (char*)malloc(len*sizeof(char));
    word[len-1] = '\0';

    while (1)
    {
        symb = getchar();
        if (symb == EOF)
        {
            if (len==1)
            {
                free(word);
                return branch;
            }
            (*N)++;
            return AddElement(word, branch);
        }

        //symb - пробельный символ
        else if (isspace(symb))
        {
            if (!space && *N>0)
            {
                if (*word != '\0')
                {
					(*N)++;
					branch = AddElement(word, branch);
					len = 1;
					word = malloc(len);
					word[len-1] = '\0';
					space = 1;
				}
            }
        }

        //symb - знак пунктуации
        else if (ispunct(symb))
        {
            space = 0;
            if (len >= 2 )
            {
                if (*word != '\0')
                {
					(*N)++;
					branch = AddElement(word, branch);
					len = 2;
					word = (char*)malloc(len);
					word[1] = '\0';
					word[0] = symb;
				}
            }
            /*if (len >= 2 && !ispunct(word[len-2]))
            {
                if (*word != '\0')
                {
					(*N)++;
					branch = AddElement(word, branch);
					len = 2;
					word = (char*)malloc(len);
					word[1] = '\0';
					word[0] = symb;
				}
            }
            else 
            {
                word[len-1] = symb;
                len++;
                word = (char*)realloc(word, len);
                word[len-1] = '\0';
            }*/
        }
        else if (isprint(symb))//symb - число или буква
        {
            space = 0;
            if (len >= 2 && ispunct(word[len-2]))
            {
                if (*word != '\0')
                {
					(*N)++;
					branch = AddElement(word, branch);
					len = 2;
					word = (char*)malloc(len);
					word[1] = '\0';
					word[0] = symb;
				}
            }
            else
            {
                word[len-1] = symb;
                len++;
                word = (char*)realloc(word, len);
                word[len-1] = '\0';
            }
        }
    }
}

//добавление элемента в дерево
struct TREE* AddElement(char* key, struct TREE* Br)
{ 
    //если пусто, то создаём новую ветку
    if (Br == NULL)
    {
        return NewBranch(key);
    }
    
    //если уже есть эл-т в дереве
    if (strcmp(key, Br->key) == 0)
    {
        (Br -> frequency)++;
        free(key);
    }
    //если всё ещё не нашли
    else if (strcmp(key, Br->key) > 0) //если key>buff
    {
        Br -> right = AddElement(key, Br -> right);
    }
    else //если key<buff
    {
        Br -> left = AddElement(key, Br -> left);
    }
    return Br;
}

struct TREE* AddNode(struct TREE* node, struct TREE* Br)
{
    //если пусто, то создаём новую ветку
    if (Br == NULL)
    {
        return NewTree(node);
    }
    
    if((Br->frequency) < (node->frequency))
    {
        Br -> right = AddNode(node, Br -> right);
    }
    else
    {
        Br -> left = AddNode(node, Br -> left);
    }
    return Br;
}

//новая ветка
struct TREE* NewBranch(char* K)
{
    struct TREE* next = (struct TREE*)malloc(sizeof(struct TREE));
    next -> key = K;
    next -> left = next -> right = NULL;
    next -> frequency = 1.0;
    return next;
}

struct TREE* NewTree(struct TREE* node)
{
    struct TREE* next = (struct TREE*)malloc(sizeof(struct TREE));
    next -> key = node->key;
    next -> left = next -> right = NULL;
    next -> frequency = node->frequency;
    return next;
}

struct TREE* GetNodes(struct TREE* Br, struct TREE* newBr) 
{
    if (Br->right != NULL) 
    {
        newBr = GetNodes(Br->right, newBr);
    }
    if (Br->left != NULL) 
    {
        newBr = GetNodes(Br->left, newBr);
    }
    newBr = AddNode(Br, newBr);
    return newBr;
}

//вывод
void output(struct TREE* list, int n)
{
    if (list == NULL) {
        return;
    }
    output(list -> right, n);
    printf("%s %d %f %d\n", list -> key, (int)list->frequency,
            list->frequency / n, list->key[0]);
    output(list -> left, n);

    return;
}

void FreeMemory(struct TREE* list, int WordFreed)
{
    if (list->right != NULL) {
        FreeMemory(list->right, WordFreed);
    }
    if (list->left != NULL) {
        FreeMemory(list->left, WordFreed);
    }
    if (WordFreed) {
        free(list->key);
    }
    free(list);
}
