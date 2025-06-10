#include "read_info.h"

#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

int Information::Get_processors_count() const {
    return processors_count;
}
int Information::Get_programs_count() const {
    return programs_count;
}
int Information::Get_network_load_limit() const {
    return network_load_limit;
}

int Information::Get_proc_load(int num) const {
    return (prog_info.at(num));
}

Information::Information(const string &file_name) {

    /*~~~~~ Открытие файла и считывание в буффер ~~~~~*/
    ifstream file(file_name);
    if(!file.is_open()) {
        throw invalid_argument("Некорректное имя файла / путь к файлу");
    }

    stringstream buffer;
    buffer << file.rdbuf();
    std::string text = buffer.str(); // Считывается весь файл в строку

    /*~~~~~ Находим значения и достаём их ~~~~~*/
    size_t substr_start, substr_end; // Начало и конец эл-та в файле

    // --- Кол-во процессоров ---
    substr_start = text.find("<processors_count>");
    if(substr_start == string::npos) {
        throw invalid_argument("Некорректный ввод (нет кол-ва процессоров)");
    }
    substr_end = text.find("</processors_count>", substr_start);

    string proc_count_str = text.substr(substr_start + string("<processors_count>").size(),   // С какого символа считывать
                            substr_end - substr_start + string("<processors_count>").size()); // Какую длину считывать
    int val = stoi(proc_count_str);

    if(val <= 0) {
        throw invalid_argument("Некорректный ввод кол-ва процессоров");
    }

    processors_count = val;

    // --- Кол-во программ ---
    substr_start = text.find("<programs_count>");
    if(substr_start == string::npos) {
        throw invalid_argument("Некорректный ввод (нет кол-ва программ)");
    }
    substr_end = text.find("</programs_count>", substr_start);

    string prog_count_str = text.substr(substr_start + string("<programs_count>").size(),   // С какого символа считывать
                            substr_end - substr_start + string("<programs_count>").size()); // Какую длину считывать
    val = stoi(prog_count_str);

    if(val <= 0) {
        throw invalid_argument("Некорректный ввод кол-ва программ");
    }

    programs_count = val;

    // --- Верхняя граница нагрузки на сеть ---
    substr_start = text.find("<network_load_limit>");
    if(substr_start == string::npos) {
        throw invalid_argument("Некорректный ввод (нет значения верхней нагрузки на сеть)");
    }
    substr_end = text.find("</network_load_limit>", substr_start);

    string network_load_limit_str = text.substr(substr_start + string("<network_load_limit>").size(),   // С какого символа считывать
                            substr_end - substr_start + string("<network_load_limit>").size()); // Какую длину считывать
    val = stoi(network_load_limit_str);

    if(val <= 0 || (val % 100 != 0)) {
        throw invalid_argument("Некорректный ввод верхней нагрузки на сеть");
    }

    network_load_limit = val;

    // --- Информация о программах ---
    substr_start = text.find("<programs_information>");
    if(substr_start == string::npos) {
        throw invalid_argument("Некорректный ввод (нет информации о программах)");
    }
    substr_end = text.find("</programs_information>", substr_start);

    string prog_info_str = text.substr(substr_start + string("<programs_information>").size(),   // С какого символа считывать
                            substr_end - substr_start + string("<programs_information>").size()); // Какую длину считывать
    size_t pos_between = 0;
    int count_prog_read = 0; // Кол-во считанных в данный момент программ
    
    // === Номер программы и нагрузка на процессор ===
    while((pos_between = prog_info_str.find("<program", pos_between)) != string::npos) {
        if(count_prog_read >= programs_count) {
            throw out_of_range("Попытка ввода большего числа программ, чем заявлено");
        }

        size_t num_prog_end = prog_info_str.find("/>", pos_between);
        string prog_str = prog_info_str.substr(pos_between, num_prog_end - pos_between);

        // Номер программы
        size_t pos_num_start = prog_str.find("num_prog=\"");
        size_t pos_num_end = prog_str.find("\"", pos_num_start + size("num_prog=\"") - 1);

        string prog_num_str = prog_str.substr(pos_num_start + string("num_prog=\"").size(),
                              pos_num_end - pos_num_start - string("num_prog=\"").size());
        int program_num = stoi(prog_num_str);

        // Нагрузка на процессор
        size_t pos_load_start = prog_str.find("proc_load=\"");
        size_t pos_load_end = prog_str.find("\"", pos_load_start + size("proc_load=\"") - 1);
        string prog_load_str = prog_str.substr(pos_load_start + string("proc_load=\"").size(),
                              pos_load_end - pos_load_start - string("proc_load=\"").size());
        int program_load = stoi(prog_load_str);

        if((program_load != 5) & (program_load != 10) &
           (program_load != 15) & (program_load != 20)) {
            throw invalid_argument("Некорректный ввод значения нагрузки на процессор");
        } else if(prog_info.count(program_num)) {
            throw invalid_argument("Попытка задать значение для одной программы дважды");
        }

        prog_info[program_num] = program_load;

        count_prog_read++;
        pos_between = num_prog_end + size("/>") - 1;
    }

    if(count_prog_read != programs_count) {
        throw invalid_argument("Некорректный ввод (кол-во программ не соотносится с заданных числом программ)");
    }
    pos_between = 0;

    // --- Информация об обмене данными ---
    substr_start = text.find("<exchange_data>");
    if(substr_start == string::npos) {
        throw invalid_argument("Некорректный ввод (нет информации об обменах)");
    }
    substr_end = text.find("</exchange_data>", substr_start);

    string exchange_data_str = text.substr(substr_start + string("<exchange_data>").size(), 
                               substr_end - substr_start - string("<exchange_data>").size());

    while((pos_between = exchange_data_str.find("<exchange_pair>", pos_between)) != string::npos) {
        size_t exch_pair_end = exchange_data_str.find("</exchange_pair>", pos_between);
        string exch_pair_info = exchange_data_str.substr(pos_between, exch_pair_end - pos_between);

        pair<int, int> pair_programs;

        // ==== Номер Prog1 ====
        size_t Prog1_pos_start = exch_pair_info.find("Prog1=\"");
        if(Prog1_pos_start == string::npos) {
            throw invalid_argument("Некорректный ввод (Prog1)");
        }
        size_t Prog1_pos_end = exch_pair_info.find("\"", Prog1_pos_start + string("Prog1=\"").size());
        pair_programs.first = stoi(exch_pair_info.substr(Prog1_pos_start + string("Prog1=\"").size(),
                     Prog1_pos_end - Prog1_pos_start - size("Prog1=\"") + 1));
        
        // ==== Номер Prog2 ====
        size_t Prog2_pos_start = exch_pair_info.find("Prog2=\"");
        size_t Prog2_pos_end = exch_pair_info.find("\"", Prog2_pos_start + size("Prog2=\"") - 1);
        if(Prog2_pos_start == string::npos){
            throw invalid_argument("Некорректный ввод (Prog2)");
        }
        pair_programs.second = stoi(exch_pair_info.substr(Prog2_pos_start + string("Prog2=\"").size(),
                      Prog2_pos_end - Prog2_pos_start - string("Prog2=\"").size()));
        
        if (pair_programs.first > pair_programs.second) {
            swap(pair_programs.first, pair_programs.second);
        } else if (pair_programs.first == pair_programs.second) {
            throw invalid_argument("Ошибка: номера программ совпадают");
        }
        if (pair_programs.second > programs_count) {
            throw out_of_range("Некорректный ввод (попытка инициализировать программу с числом большим, чем общее число программ)");
        }

        // === Интенсивность обмена данными между Prog1 и Prog2 ===
        size_t intensity_exchange = exch_pair_info.find("load=\"");
        if(intensity_exchange == string::npos) {
            throw invalid_argument("Некорректный ввод (нет информации об интенсивности)");
        }

        size_t int_exch_end = exch_pair_info.find("\"", intensity_exchange + string("load=\"").size());
        val = stoi(exch_pair_info.substr(intensity_exchange + string("load=\"").size(),
              int_exch_end - intensity_exchange - string("load=\"").size()));
        
        if (val < 0) {
            throw invalid_argument("Некорректный ввод (интенсивность обмена)");
        }

        exchange_pair[pair_programs] = val;
        pos_between = exch_pair_end + size("</exchange_pair>") - 1;
    }
    file.close();
}