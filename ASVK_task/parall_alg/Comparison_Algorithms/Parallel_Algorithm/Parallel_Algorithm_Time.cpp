// Реализация параллельного алгоритма
#include "read_info.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>

using namespace std;

// Количество созданных потоков (в моём компьютере их 4)
#define COUNT_THREADS 4

mutex mtx; // Мьютекс для синхронизации работы программ

// Целевая функция по заданному вектору (такая же, как в базовом алгоритме)
unsigned short F_best(vector<int> &answer, const Information &system_info) {
    vector<int> solution(system_info.Get_processors_count());

    for (int i = 0; i < system_info.Get_programs_count(); i++) {
        solution[answer[i] - 1] += system_info.Get_proc_load(i+1); // answer[i] - номер процессора, i - номер программы
    }

    int MAX = *max_element(solution.begin(), solution.end());
    return MAX;
}

void Start_Thread(vector<int> &best_solution, int &best_val, int &iter,
                  const Information &info, int &sum_num_iter) {

    //Инициализация генератора случайных чисел (для каждой нити свой)
    random_device seed;
    mt19937 gen(seed());
    uniform_int_distribution<int> distrib(1, (info.Get_processors_count()));
    // Для равного шанса получить каждое число
    // И чтобы вектор решений генерировался более равномерно

    vector<int> cur_solution = best_solution;

    // Бесконечный цикл нужен для создания критической секции,
    // в которой изменяется текущее кол-во итераций
    while (1) {
        mtx.lock();
        
        if (iter < NUM_ITERATIONS) {
            iter++;
            sum_num_iter++;

            mtx.unlock();
            // Конец критической секции, начало выполнения итерации алгоритма
            
            for (int j = 0; j < cur_solution.size(); j++) {
                cur_solution[j] = distrib(gen);
            }

            unsigned short cur_val = F_best(cur_solution, info);

            // В критической секции проверяем, можно ли обновить best_val (как и в базовом алгоритме)
            mtx.lock();

            if (best_val > cur_val) {
                unsigned cur_load = 0;

                for (auto j: info.exchange_pair) {
                    if (cur_solution[j.first.first - 1] != cur_solution[j.first.second - 1]) {
                        cur_load += j.second;
                    }
                }

                if (cur_load <= info.Get_network_load_limit()) {
                    best_val = cur_val;
                    best_solution = cur_solution;
                    iter = -1;
                }
            }
            mtx.unlock();
        } else {
            mtx.unlock();
            break;
        }
    }
}

// Ф-ция, которая запускает несколько параллельных потоков
vector<int> Algorithm(const Information &info, int &sum_num_iter) {
    int best_value = 100;

    vector<int> best_solution(info.Get_programs_count());
    vector<int> cur_solution;

    // Подсчёт кол-ва итераций алгоритма
    int cur_iteration = 0;

    for (int i = 0; i < best_solution.size(); i++) {
        best_solution[i] = -1;
    }
    
    // Вектор, в котором лежат идентификаторы всех нитей
    vector<thread> thr;

    for (int i = 0; i < COUNT_THREADS; i++) {
        // Создаётся новый поток и добавляется в thr
        thr.emplace_back(Start_Thread, ref(best_solution), ref(best_value),
                         ref(cur_iteration), info, ref(sum_num_iter));
    }

    // Ожидание завершения всех нитей
    for (int i = 0; i < thr.size(); i++) {
        thr[i].join();
    }

    return best_solution;
}

// main такой же, как в базовом алгоритме
int main() {
    // Инициализация rand()
    srand(time(nullptr));

    try {
        cout << "Введите путь к файлу с входными данными: ";
        string file_name;
        getline(cin, file_name);
        cout << endl;

        // Парсим входные данные
        Information system_info(file_name);
        // Кол-во итераций в данный момент
        int num_iterations_now = 0;
        // Корректное решение
        auto start = chrono::system_clock::now();
        vector<int> final_answer = Algorithm(system_info, num_iterations_now);
        auto end = chrono::system_clock::now();
        auto time_worked = chrono::duration_cast<chrono::milliseconds>(end - start);

        // Если вектор == -1 -1 ... -1 => ошибка
        cout << "Результат работы программы: ";
        if (all_of(final_answer.begin(), final_answer.end(), [](int x) { return x == -1; })) {
            cout << "failure" << endl;
            cout << "Кол-во итераций базового алгоритма: " << num_iterations_now << endl;
        } else {
            cout << "success" << endl;
            cout << "Кол-во итераций базового алгоритма: " << num_iterations_now << endl;

            cout << "Решение: ";
            for (int i = 0; i < final_answer.size(); i++) {
                cout << final_answer[i] << " ";
            }
            
            cout << endl;
            cout << "F_best(x_best) = " << F_best(final_answer, system_info) << endl;
        }
        cout << "Сколько времени работала программа: " << time_worked.count()
             << " миллисекунд" << endl;
    }

    // Если произошёл некорректный ввод
    catch (const invalid_argument &Error) {
        cout << Error.what() << endl;
    }
    catch (const out_of_range &Error) {
        cout << Error.what() << endl;
    }

    return 0;
}