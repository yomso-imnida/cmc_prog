#include <iostream>
#include <tinyxml2.h> // Для генерации xml-файла
#include <ctime>

#define COUNT_PROCESSORS 67
#define COUNT_PROGRAMS 87
#define LIM_NET 12900

// Промежуток [a1, a2]
int Randomizer(int a1, int a2) {
    return (std::rand() % (a2 - a1 + 1) + a1);
};

int main() {
    srand(time(nullptr)); // "Проинициализировали" генератор

    tinyxml2::XMLDocument docum;
    tinyxml2::XMLElement* root = docum.NewElement("information");

    docum.InsertFirstChild(root);
    tinyxml2::XMLElement* processor = docum.NewElement("processors_count");
    processor->SetText(COUNT_PROCESSORS);
    root->InsertEndChild(processor);

    tinyxml2::XMLElement* program = docum.NewElement("programs_count");
    program->SetText(COUNT_PROGRAMS);
    root->InsertEndChild(program);

    tinyxml2::XMLElement* net_lim = docum.NewElement("network_load_limit");
    net_lim->SetText(LIM_NET);
    root->InsertEndChild(net_lim);

    // Информация о каждой программе
    tinyxml2::XMLElement* prog_info = docum.NewElement("programs_information");

    for (int i = 0; i < COUNT_PROGRAMS; i++) {
        tinyxml2::XMLElement* prog = docum.NewElement("program");
        int PR_LOAD = Randomizer(1, 4) * 5;
        prog->SetAttribute("num_prog", i+1);
        prog->SetAttribute("proc_load", PR_LOAD);
        prog_info->InsertEndChild(prog);
    }

    tinyxml2::XMLElement* exch_data = docum.NewElement("exchange_data");
    for (int i = 1; i <= COUNT_PROGRAMS; i++) {
        for (int j = i + 1; j <= COUNT_PROGRAMS; j++) {
            tinyxml2::XMLElement* exch_pair = docum.NewElement("exchange_pair");
            tinyxml2::XMLElement* pair = docum.NewElement("pair_programs");
            pair->SetAttribute("Prog1", i);
            pair->SetAttribute("Prog2", j);
            exch_pair->InsertEndChild(pair);

            tinyxml2::XMLElement* int_exch = docum.NewElement("intensity_exchange");
            int load_pair[5] = {0, 20, 50, 70, 100};
            int index = std::rand() % 5;
            int_exch->SetAttribute("load", load_pair[index]);
            exch_pair->InsertEndChild(int_exch);

            exch_data->InsertEndChild(exch_pair);
        }
    }
    root->InsertEndChild(prog_info);
    root->InsertEndChild(exch_data);

    tinyxml2::XMLError res_save = docum.SaveFile("input_data_3.xml");
    if(res_save != tinyxml2::XML_SUCCESS) {
        std::cout << "Ошибка: не удалось сохранить файл" << std::endl;
        return 1;
    }
    return 0;
}