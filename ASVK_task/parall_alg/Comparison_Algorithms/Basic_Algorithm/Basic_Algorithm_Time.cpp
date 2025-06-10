// Реализация базового алгоритма
#include "read_info.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <chrono>

using namespace std;

// Целевая функция по заданному вектору
unsigned short F_best(vector<int> &answer, const Information &system_info) {
    vector<int> solution(system_info.Get_processors_count());

    for (int i = 0; i < system_info.Get_programs_count(); i++) {
        solution[answer[i] - 1] += system_info.Get_proc_load(i+1); // answer[i] - номер процессора, i - номер программы
    }

    int MAX = *max_element(solution.begin(), solution.end());
    return MAX;
}

vector<int> Algorithm(const Information &system_info, int &iter) {
    unsigned short best_value = 100;

    vector<int> best_solution(system_info.Get_programs_count());
    vector<int> cur_solution;

    // Инициализация лучшего решения
    for (int i = 0; i < best_solution.size(); i++) {
        best_solution[i] = -1;
    }
    cur_solution = best_solution;

    // const NUM_ITERATIONS объявлена в read_info.h
    for (int i = 0; i < NUM_ITERATIONS; i++, iter++) {
        for (int j = 0; j < cur_solution.size(); j++) {
            cur_solution[j] = rand() % system_info.Get_processors_count() + 1;
        }

        unsigned short cur_value = F_best(cur_solution, system_info);

        if (best_value > cur_value) {
            int cur_load = 0;

            // Проверяем, не превышен ли лимит нагрузки на сеть
            for (auto j: system_info.exchange_pair) {
                // Программы работают на одном процессоре => нагрузку учитывать не надо
                if (cur_solution[j.first.first - 1] != cur_solution[j.first.second - 1]) {
                    cur_load += j.second;
                }
            }

            // true => нашли новое лучшее решение
            if (cur_load <= system_info.Get_network_load_limit()) {
                best_value = cur_value;
                best_solution = cur_solution;
                i = -1;
            }
        }
    }
    return best_solution;
}

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
        int num_iterations = 0;
        // Корректное решение
        auto start = chrono::system_clock::now();
        vector<int> final_answer = Algorithm(system_info, num_iterations);
        auto end = chrono::system_clock::now();
        auto time_worked = chrono::duration_cast<chrono::milliseconds>(end - start);

        // Если вектор == -1 -1 ... -1 => ошибка
        cout << "Результат работы программы: ";
        if (all_of(final_answer.begin(), final_answer.end(), [](int x) { return x == -1; })) {
            cout << "failure" << endl;
            cout << "Кол-во итераций базового алгоритма: " << num_iterations << endl;
        } else {
            cout << "success" << endl;
            cout << "Кол-во итераций базового алгоритма: " << num_iterations << endl;

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