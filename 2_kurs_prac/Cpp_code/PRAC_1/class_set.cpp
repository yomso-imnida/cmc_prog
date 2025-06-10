// 18 вариант
#include <iostream>
using namespace std;

class Set {
    int* M;
    int count;

    void rewrite_and_add(int E) {
        count++;
        int* newM = new int[count];
        for (int i = 0; i < count-1; i++) {
            newM[i] = M[i];
        }
        newM[count-1] = E;
        delete[] M;
        M = newM;
    }

public:
    Set() { count = 0; M = nullptr; };
    Set(int elem) { count = 1; M = new int[1]; M[count-1] = elem; };
    ~Set() { delete[] M; };
    int Get_M(int i) { return M[i]; }
    int Get_count() { return count; }

    void sort() {
        for (int i = 0; i < count-1; i++)
            for (int j = 0; j < count-1-j; j++)
                if(M[j] > M[j+1]) swap(M[j], M[j+1]);
    }

    // Добавление эл-та
    void add(int elem) {
        int j = 0;
        if(count) { 
            for(int i = 0; i < count; i++) { 
                if (M[i] == elem)  j = 1;
            }
        }
        if (!j) {
            rewrite_and_add(elem);
            sort();
        }
        else {
            cout << "Такой эл-т уже есть в мн-ве" << endl;
        }
    }
    // Удаление эл-та
    void del(int elem) {
        int flag = 0, N = 0;
        for (int i = 0; i < count; i++) {
            if(elem == M[i]) {
                flag = 1;
                N = i;
                break;
            }
        }
        if(flag) { // Нашли эл-т
            int* newM = new int[--count];
            for (int j = 0; j < N; j++) {
                newM[j] = M[j];
            }
            for (int j = N; j < count; j++) {
                newM[j] = M[j+1];
            }
            delete[] M;
            M = newM;
        }
        else { cout << "Такого эл-та нет в мн-ве\n"; }
    }

    // Есть ли элемент в множестве
    int in_arr(int elem) {
        int F = 0;
        for (int i = 0; i < count; i++) {
            if (elem == M[i]) {
                F = 1;
                break;
            }
        }
        return F;
    }

    void print_set() {
        if(count == 0) {
            cout << "{ пусто }" << endl;
        }
        else {
            cout << "{ ";
            for (int i = 0; i < count-1; i++) {
                cout << M[i] << ", ";
            }
            cout << M[count-1] << " }" << endl;
        }
    }

    // Конструктор копирования
    Set(const Set& other) {
        count = other.count;
        M = new int[other.count];
        for (int i = 0; i < count; i++) {
            M[i] = other.M[i];
        }
    }
    // Перегрузка операции присваивания
    Set& operator=(const Set& other) {
        if (this != &other) {
            delete[] M;
            count = other.count;
            M = new int[other.count];
            for (int i = 0; i < count; i++) {
                M[i] = other.M[i];
            }
        }
        return *this;
    }
};

// Пересечение мн-в
Set intersection(Set M1, Set M2) {
    int C1 = M1.Get_count(), C2 = M2.Get_count();
    if (C1 == 0 || C2 == 0) return Set();
    int C3 = C1+C2;
    int* arr = new int[C3];
    for (int i = 0; i < C1; i++) {
        arr[i] = M1.Get_M(i);
    }
    if (C2 > 0){
        for (int i = C1; i < C3; i++) {
            arr[i] = M2.Get_M(i-C1);
        }
    }
    for (int i = 0; i < C3-1; i++) {
        for (int j = 0; j < C3-1-i; j++) {
            if(arr[j] > arr[j+1]) swap(arr[j], arr[j+1]);
        }
    }

    Set newSet;
    for (int k = 0; k < C3-1; k++) {
        if (arr[k] == arr[k+1]) {
            newSet.add(arr[k]);
        }
    }
    delete[] arr;
    return newSet;
}

// Объединение мн-в
Set union_sets(Set M1, Set M2) {
    int C1 = M1.Get_count(), C2 = M2.Get_count();
    if (C1 == 0 && C2 == 0) return Set();
    Set newSet;
    for (int i = 0; i < C1; i++) {
        newSet.add(M1.Get_M(i));
    }
    for(int j = 0; j < C2; j++) {
        newSet.add(M2.Get_M(j));
    }
    return newSet;
}

// Разность мн-в (newM = M1 - M2)
Set diff(Set M1, Set M2) {
    int C1 = M1.Get_count();
    if(C1 == 0) return Set();
    Set newSet;
    for (int i = 0; i < C1; i++) {
        if(!M2.in_arr(M1.Get_M(i))) {
            newSet.add(M1.Get_M(i));
        }
    }
    return newSet;
}

int main() {
    cout << "-------- New Session --------" << endl;
    cout << "       ~Список команд~" << endl;
    cout << "Выберите команду (введите цифру):  " << endl;
    cout << "0 - создать мн-во" << endl;
    cout << "1 - создать мн-во и добавить эл-т" << endl;
    cout << "2 - добавить эл-т в мн-во" << endl;
    cout << "3 - удалить эл-т из мн-ва" << endl;
    cout << "4 - показать, из чего сейчас состоит мн-во" << endl;
    cout << "5 - уничтожить мн-во" << endl;
    cout << "6 - найти объединение двух мн-в" << endl;
    cout << "7 - найти пересечение двух мн-в" << endl;
    cout << "8 - найти разность мн-в" << endl;
    cout << "9 - завершить сеанс" << endl << endl;

    int count_set = 0; // Кол-во мн-в
    Set A, B;
    while (1) {
        int X;
        cin >> X;
        switch (X) {
        case 0: { // Создать мн-во
            if (count_set == 0) {
                count_set++;
                cout << "Мн-во A создано" << endl;
            }
            else if (count_set == 1) {
                count_set++;
                cout << "Мн-во B создано" << endl;
            }
            else {
                cout << "У Вас создано max кол-во мн-в (2)" << endl;
                cout << "Существующие мн-ва:" << endl;
                cout << "   A = "; A.print_set();
                cout << "   B = "; B.print_set();
            }
        }
            break;
        case 1: { // Создать мн-во и добавить эл-т
            if (count_set == 0) {
                int elem;
                cout << "Введите эл-т, который хотите добавить:  ";
                cin >> elem;
                A.add(elem);
                count_set++;
                cout << "Мн-во A создано, в него добавлен эл-т " << elem << endl;
            }
            else if (count_set == 1) {
                int elem;
                cout << "Введите эл-т, который хотите добавить:  ";
                cin >> elem;
                B.add(elem);
                count_set++;
                cout << "Мн-во B создано, в него добавлен эл-т " << elem << endl;
            }
            else {
                cout << "У Вас создано max кол-во мн-в (2)" << endl;
                cout << "Существующие мн-ва:" << endl;
                cout << "   A = "; A.print_set();
                cout << "   B = "; B.print_set();
            }
        }
            break;
        case 2: { // Добавить эл-т в мн-во
            if (count_set == 0) {
                cout << "Нет ни одного мн-ва" << endl;
            }
            else if (count_set == 1) {
                int elem;
                cout << "Введите эл-т:  ";  cin >> elem;
                A.add(elem);
                cout << "A = "; A.print_set();
            }
            else {
                int elem, N;
                cout << "Введите эл-т:  ";  cin >> elem;
                cout << "В какое мн-во Вы хотите его добавить? ";
                cout << "Выберите цифру: 1 - A,  2 - B" << endl;
                cin >> N;
                switch (N) {
                case 1:
                    A.add(elem);
                    cout << "A = "; A.print_set();
                    break;
                case 2:
                    B.add(elem);
                    cout << "B = "; B.print_set();
                    break;
                }
            }
        }
            break;
        case 3: { // Удалить эл-т из мн-ва
            if (count_set == 0) {
                cout << "Нет ни одного мн-ва" << endl;
            }
            else if (count_set == 1) {
                int elem;
                cout << "Введите эл-т:  ";  cin >> elem;
                A.del(elem);
                cout << "A = "; A.print_set();
            }
            else {
                int elem, N;
                cout << "Введите эл-т:  ";  cin >> elem;
                cout << "Из какого мн-ва Вы хотите его удалить? ";
                cout << "Выберите цифру: 1 - A,  2 - B" << endl;
                cin >> N;
                switch (N) {
                case 1:
                    A.del(elem);
                    cout << "A = "; A.print_set();
                    break;
                case 2:
                    B.del(elem);
                    cout << "B = "; B.print_set();
                    break;
                }
            }
        }
            break;
        case 4: { // Показать, из чего сейчас состоит мн-во
            if (count_set == 0) {
                cout << "Нет ни одного мн-ва" << endl;
            }
            else if (count_set == 1) {
                cout << "A = "; A.print_set();
            }
            else {
                int N;
                cout << "Какое мн-во Вы хотите посмотреть? ";
                cout << "Выберите цифру: 1 - A,  2 - B" << endl;
                cin >> N;
                switch (N) {
                case 1:
                    cout << "A = "; 
                    A.print_set();
                    break;
                case 2:
                    cout << "B = "; 
                    B.print_set();
                    break;
                }
            }
        }
            break;
        case 5: { // Уничтожить мн-во
            if (count_set == 0) {
                cout << "Нет ни одного мн-ва" << endl;
            }
            else if (count_set == 1) {
                for(int i = 0; i < A.Get_count(); i++) {
                    A.del(A.Get_M(i));
                }
                count_set--;
                cout << "Мн-во A уничтожено" << endl;
            }
            else {
                int N;
                cout << "Какое мн-во Вы хотите уничтожить? ";
                cout << "Выберите цифру: 1 - A,  2 - B" << endl;
                cin >> N;
                switch (N) {
                case 1:
                    for(int i = 0; i < A.Get_count(); i++) {
                        A.del(A.Get_M(i));
                    }
                    cout << "Мн-во A уничтожено" << endl;
                    A = B;
                    cout << "Мн-во B переименовано в мн-во A" << endl;
                case 2:
                    for(int i = 0; i < B.Get_count(); i++) {
                        B.del(B.Get_M(i));
                    }
                    count_set--;
                    if(N == 2) cout << "Мн-во B уничтожено" << endl;
                    break;
                }
            }
        }
            break;
        case 6: { // Найти объединение двух мн-в
            if (count_set == 0) {
                cout << "Нет ни одного мн-ва" << endl;
            }
            else if (count_set == 1) {
                cout << "Создайте второе мн-во" << endl;
            }
            else {
                Set C = union_sets(A, B);
                cout << "Объединение A и B = ";
                C.print_set();
            }
        }
            break;
        case 7: { // Найти пересечение двух мн-в
            if (count_set == 0) {
                cout << "Нет ни одного мн-ва" << endl;
            }
            else if (count_set == 1) {
                cout << "Создайте второе мн-во" << endl;
            }
            else {
                Set C = intersection(A, B);
                cout << "Пересечение A и B = ";
                C.print_set();
            }
        }
            break;
        case 8: { // Найти разность мн-в
            if (count_set == 0) {
                cout << "Нет ни одного мн-ва" << endl;
            }
            else if (count_set == 1) {
                cout << "Создайте второе мн-во" << endl;
            }
            else {
                int N; Set C;
                cout << "Какую именно разность Вы хотите узнать? ";
                cout << "Выберите цифру: 1 - A/B (A-B),  2 - B/A (B-A)" << endl;
                cin >> N;
                switch (N) {
                case 1:
                    C = diff(A, B);
                    cout << "A/B (A-B) = ";
                    C.print_set();
                    break;
                case 2:
                    C = diff(B, A);
                    cout << "B/A (B-A) = ";
                    C.print_set();
                    break;
                }
            }
        }
            break;
        case 9: // Завершить сеанс
            cout << "-------- End Session --------" << endl;
            return 0;
        default: // Пользователь ввёл что-то не то / ничего не ввёл
            break;
        }
        cout << endl;
    }
}

/*0 - создать множество (больше 2-ух нельзя будет создать)*/