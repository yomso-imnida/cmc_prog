/*
 + конструктор копирования,
 + деструктор,
 + перегруженную операцию присваивания,
 + статические члены класса (по существу),
 + константные методы (по существу),
 + перегруженную операцию вывода объекта в стандартный поток вывода,
 + перегруженную операцию [] или () (если их использование не противоречит
    структуре выбранного класса)
*/
// Вариант: Умножение двух матриц, умножение матрицы на число

#include <iostream>
#include <math.h> // Для красивого вывода
using namespace std;

class Matr { // Класс должен быть неплоским
protected:
    int count_string; // Кол-во строк
    int count_column; // Кол-во столбцов
    double** matrix;
    // Статический член класса
    static int count_Matr;  // Кол-во существующих матриц

public:
    // Конструктор по умолчанию
    Matr() : count_string(0), count_column(0), matrix(nullptr) { count_Matr++; }
    // Конструктор с параметрами
    Matr(int s, int c) : count_string(s), count_column(c) {
        matrix = new double*[count_string];
        for (int i = 0; i < count_string; i++) {
            matrix[i] = new double[count_column](); // Массив будет инициализ. знач. 0.0
        }
        count_Matr++;
    }
    // Конструктор копирования
    Matr(const Matr& other) : count_string(other.count_string), count_column(other.count_column) {
        matrix = new double*[count_string];
        for (int i = 0; i < count_string; i++) {
            matrix[i] = new double[count_column];
            for (int j = 0; j < count_column; j++) {
                matrix[i][j] = other.matrix[i][j];
            }
        }
        count_Matr++;
    }
    // Деструктор
    virtual ~Matr() {
        for (int i = 0; i < count_string; i++) {
            delete[] matrix[i];
        }
        delete[] matrix;
        count_Matr--;
    }
    
    // Перегруженная операция присваивания
    Matr& operator=(const Matr& other) {
        if(this != &other) {
            if(matrix) {
                for (int i = 0; i < count_string; i++) {
                    delete[] matrix[i];
                }
                delete[] matrix;
                matrix = nullptr;
            }

            count_string = other.count_string;
            count_column = other.count_column;

            if (count_string > 0 && count_column > 0) {
                matrix = new double*[count_string];
                for (int i = 0; i < count_string; i++) {
                    matrix[i] = new double[count_column];
                    for (int j = 0; j < count_column; j++) {
                        matrix[i][j] = other.matrix[i][j];
                    }
                }
            } else {
                matrix = nullptr;
            }
        }
        return *this;
    }
    // Перегруженная операция вывода объекта в стандартный поток вывода
    friend ostream& operator<< (ostream& outMatr, const Matr& M);
    // Перегруженная операция ()
    virtual void operator()(double elem) = 0;

    // Статический метод
    static int getCountMart() { return count_Matr; }
    // Константные методы
    int getCountString() const { return count_string; }
    int getCountColumn() const { return count_column; }
    double getMatrixElem(int i, int j) const {
        if (i >= count_string || j >= count_column) {
            cout << "Error: выход за границы матрицы" << endl;
            return 0.0;
        }
        return matrix[i][j];
    }

    virtual Matr* Matr_mul_Matr(const Matr& other) const = 0; // Умножение двух матриц
    virtual void Matr_mul_Num(double num) = 0; // Умножение матрицы на число
};

int Matr::count_Matr = 0; // Инициализация статического члена класса

ostream& operator<< (ostream& outMatr, const Matr& M) {
    if (!M.matrix) {
        outMatr << "Матрица пустая" << endl;
        return outMatr;
    }
    for (int i = 0; i < M.count_string; i++) {
        for (int j = 0; j < M.count_column; j++) {
            cout.left;
            cout.width(4);
            cout.precision(3);
            outMatr << M.matrix[i][j] << " ";
            if((j+1) != M.count_column) outMatr << "| ";
        }
        outMatr << endl;
    }
    return outMatr;
}

class ThisMatr : public Matr {
    int NextIndex; // Индекс следующей свободной позиции в матрице
public:
    ThisMatr() : Matr(), NextIndex(0) {
        cout << "Количество матриц сейчас (среди них есть пустая/ые): "
             << getCountMart() << endl;
    }
    ThisMatr(int s, int c) : Matr(s, c), NextIndex(0) {}

    // Перегруженная операция ()
    virtual void operator()(double elem) override {
        if(NextIndex >= count_string * count_column) {
            cout << "Error: Матрица заполнена, больше нельзя добавлять эл-ты" << endl;
            return;
        }

        int i = NextIndex / count_column; // Номер строки
        int j = NextIndex % count_column; // Номер столбца
        matrix[i][j] = elem;
        NextIndex++;
    }

    // Умножение двух матриц
    virtual Matr* Matr_mul_Matr(const Matr& other) const override {
        if(count_column != other.getCountString()) {
            cout << "Error: Невозможно перемножить (матрица результата не создана)" << endl;
            return nullptr;
        }
        ThisMatr* result = new ThisMatr(count_string, other.getCountColumn());
        for (int i = 0; i < count_string; i++) {
            for (int j = 0; j < other.getCountColumn(); j++) {
                result->matrix[i][j] = 0.0;
                for (int k = 0; k < count_column; k++) {
                    result->matrix[i][j] += matrix[i][k] * other.getMatrixElem(k, j);
                }
            }
        }
        return result;
    }
    // Умножение матрицы на число
    virtual void Matr_mul_Num(double num) override {
        for (int i = 0; i < count_string; i++) {
            for (int j = 0; j < count_column; j++) {
                matrix[i][j] *= num;
            }
        }
    }
};

int main() {
    cout << endl;
    ThisMatr A;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

    ThisMatr B(2, 3); // Матрица 2x3
    cout << "Количество матриц сейчас: " << ThisMatr::getCountMart() << endl;
    cout << "Размеры матрицы B: " << B.getCountString() << "x" << B.getCountColumn() << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

    cout << "Добавим шесть элементов в матрицу B" << endl;
    for (int i = 1; i <= 2*3; i++) {
        B(i * 10.1); // 10.1, 20.2 и т.д.
    }
    cout << "Попробуем добавить 7-й элемент в матрицу B" << endl;
    // Матрица в этот момент переполнена (будет ошибка)
    B(983.42);
    cout << "Матрица B:" << endl << B;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

    ThisMatr C(4, 2);
    cout << "Количество матриц сейчас: " << ThisMatr::getCountMart() << endl;
    for (int i = 1; i <= 4*2; i++) {
        C(i * 0.5); // 0.5, 1 и т.д.
    }
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

    cout << "Матрица C:" << endl << C;
    A = C;
    cout << "Матрица A (после A=C):" << endl << A;
    cout << "Матрица B:" << endl << B;
    C = B;
    cout << "Матрица C (после C=B):" << endl << C;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

    Matr * res_B_A;
    Matr * res_A_B;
    cout << "Размер матрицы A: " << A.getCountString() << "x" << A.getCountColumn() << endl;
    cout << "Размер матрицы B: " << B.getCountString() << "x" << B.getCountColumn() << endl;
    cout << "Размер матрицы C: " << C.getCountString() << "x" << C.getCountColumn() << endl;
    
    cout << "Перемножаем B и A" << endl;
    res_B_A = B.Matr_mul_Matr(A); // Должна быть ошибка B(2x3), A(4x2)
    cout << "Перемножаем A и B" << endl;
    res_A_B = A.Matr_mul_Matr(B); // Матрица 4x3

    cout << "Умножение A * B:" << endl << *res_A_B;
    cout << "Элемент матрицы A * B на позиции (2, 1): " << res_A_B->getMatrixElem(0, 1) << endl;
    cout << "Элемент матрицы A * B на позиции (1, 3): " << res_A_B->getMatrixElem(2, 0) << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

    double n = 6.8;
    A.Matr_mul_Num(n);
    cout << "Матрица A:" << endl << A;
    cout << "Умножение A * n:" << endl << A;

    // Должно быть: 4
    cout << "Количество матриц сейчас: " << A.getCountMart() << endl << endl;
    delete res_A_B;
    delete res_B_A;
    return 0;
}