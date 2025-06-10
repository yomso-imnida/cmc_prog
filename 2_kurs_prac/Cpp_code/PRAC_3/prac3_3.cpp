// Вариант: 13. I. 1 II. 0 III. 3 IV. 1 V. 3 VI. 2

#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <cctype>
#include <utility>
#include <vector>
#include <algorithm>
#include <stdexcept>

enum type_of_lex {
    LEX_NULL,        //  0 - пустой лексем

    // Спец слова
    LEX_IF,          //  1 - if
    LEX_AND,         //  2 - логическое и (and)
    LEX_FALSE,       //  3 - false
    LEX_TRUE,        //  4 - true
    LEX_INT,         //  5 - тип данных (int)
    LEX_BOOL,        //  6 - тип данных (bool)
    LEX_NOT,         //  7 - логическое отрицание (not)
    LEX_OR,          //  8 - логическое или (or)
    LEX_PROGRAM,     //  9 - program
    LEX_READ,        // 10 - read
    LEX_WRITE,       // 11 - write
    LEX_WHILE,       // 12 - while
    LEX_STRING,      // 13 - тип данных (string)
    LEX_BREAK,       // 14 - break

    LEX_FIN,         // 15 - конец входного потока

    // Спец символы
    LEX_SEMICOLON,   // 16 - ;
    LEX_COMMA,       // 17 - ,
    LEX_COLON,       // 18 - :
    LEX_ASSIGN,      // 19 - =
    LEX_LPAREN,      // 20 - (
    LEX_RPAREN,      // 21 - )
    LEX_EQ,          // 22 - ==
    LEX_LSS,         // 23 - <
    LEX_GTR,         // 24 - >
    LEX_PLUS,        // 25 - +
    LEX_MINUS,       // 26 - минус (-)
    LEX_TIMES,       // 27 - *
    LEX_SLASH,       // 28 - /
    LEX_MOD,         // 29 - %
    LEX_LEQ,         // 30 - <=
    LEX_NEQ,         // 31 - !=
    LEX_GEQ,         // 32 - >=
    LEX_LFIG,        // 33 - {
    LEX_RFIG,        // 34 - }

    LEX_NUM,         // 35 - числовая const (int)
    LEX_STR,         // 36 - строковая const (string)
    LEX_ID,          // 37 - идентификатор (имя переменной)

    POLIZ_LABEL,     // 38 - адрес, куда прыгать
    POLIZ_ADDRESS,   // 39 - адрес переменной в таблице идентификаторов (TID); говорит, куда писать/читать значение переменной (при присваивании)
    POLIZ_GO,        // 40 - безусловный переход (прыжок на метку)
    POLIZ_FGO,       // 41 - условный переход по false (прыжок на метку)
    LEX_DUP          // 42 – дублировать верх стека (используется внутри ПОЛИЗа)
};

class Lex {
    type_of_lex t_lex; // Тип лексемы (из enum): число, строка и т.д.
    int v_lex;         // Идентификатор лексемы (число типа лексемы)
    int line_lex;      // Номер строки, в которой была найдена лексема
    std::string s_lex; // Сама строка (только для LEX_STR)

public:
    explicit Lex (type_of_lex t = LEX_NULL, int v = 0, int line = 0, std::string str = ""): 
             t_lex (t), v_lex (v), line_lex(line), s_lex(std::move(str)) { } // move - перемещаем содержимое из str в s_lex
    
    // Возвращат тип лексемы (LEX_)
    type_of_lex  get_type () const { 
    	return t_lex; 
    }

    // Возвращат идентификатор лексемы (число)
    int get_value () const { 
    	return v_lex; 
    }

    std::string get_str() const {
        return s_lex;
    }

    friend std::ostream &operator<<(std::ostream &s, Lex l);
};

class Ident {
    std::string name; // Имя идентификатора (переменной)
    bool declare;     // Был ли идентификатор объявлен
    type_of_lex type; // Тип переменной: LEX_INT, LEX_BOOL, LEX_STRING
    bool assign;      // Инициализирована ли переменная
    int value;        // Значение переменной (для int/bool)

public:
    Ident() : declare(false), assign(false), value(0) {}

    bool operator==(const std::string &s) const {
        return name == s;
    }

    explicit Ident(const std::string& n) : name(n), declare(false), assign(false), value(0) {}

    std::string get_name() const { 
        return name; 
    }

    // Был ли идентификатор объявлен
    bool get_declare() const { 
        return declare; 
    }

    // Говорим, что переменная объявлена
    void put_declare() { 
        declare = true; 
    }

    type_of_lex get_type() const { 
        return type;
    }

    // Говорим, какого типа переменная
    void put_type(type_of_lex t) { 
        type = t; 
    }

    // Инициализирована ли переменная
    bool get_assign() const { 
        return assign; 
    }

    // Говорим, что переменную инициализировали
    void put_assign() { 
        assign = true; 
    }

    int get_value() const { 
        return value; 
    }

    void put_value(int v) { 
        value = v; 
    }
};

std::vector<Ident> TID;

// Добавление/поиск идентификатора в табл. инд-ов TID
int put (const std::string &buf) {
    std::vector<Ident>::iterator k;
    if ((k = find(TID.begin(), TID.end(), buf)) != TID.end()) // find - первое вхождение buf в промежутке
        // Если нашли идент-тор, то возвращаем его индекс
        return static_cast<int>(std::distance(TID.begin(), k)); // distance - кол-во эл-тов между двумя эл-тами в контейнере
    // Добавляем в таблицу
    TID.push_back (Ident(buf));

    return static_cast<int>(TID.size() - 1); // Возвращает индекс
}

class Scanner {
    FILE * fp;
    char c; // Текущий символ, считанный из файла

    // Возвращает индекс слова в списке
    static int look (const std::string buf, const char **list) {
        int i = 0;
        while (list[i]) {
            if (buf == list[i])
                return i;
            ++i;
        }
        return 0; // Если не нашли (LEX_NULL)
    }

    // Считывание следующего символа
    void gc () {
        c = fgetc (fp);
    }

public:
    static const char *TW[], *TD[];
    static int line_counter; // Номер текущей строки (для сообщений об ошибках и вывода)

    explicit Scanner (const char* program) {
        if (!(fp = fopen(program, "r"))) 
            throw "Файл не удалось открыть";
    }

    // Возвращает следующую лексему
    Lex get_lex ();
};

// Спец слова
const char *Scanner::TW[] = {
    "",          //  0 - LEX_NULL
    "if",        //  1 - LEX_IF
    "and",       //  2 - LEX_AND
    "false",     //  3 - LEX_FALSE
    "true",      //  4 - LEX_TRUE
    "int",       //  5 - LEX_INT
    "boolean",   //  6 - LEX_BOOL
    "not",       //  7 - LEX_NOT
    "or",        //  8 - LEX_OR
    "program",   //  9 - LEX_PROGRAM
    "read",      // 10 - LEX_READ
    "write",     // 11 - LEX_WRITE
    "while",     // 12 - LEX_WHILE
    "string",    // 13 - LEX_STRING
    "break",     // 14 - LEX_BREAK
    NULL
};

// Спец символы
const char *Scanner::TD[] = {
    "",   //  0 - LEX_NULL
    ";",  //  1 - LEX_SEMICOLON
    ",",  //  2 - LEX_COMMA
    ":",  //  3 - LEX_COLON
    "=",  //  4 - LEX_ASSIGN
    "(",  //  5 - LEX_LPAREN
    ")",  //  6 - LEX_RPAREN
    "==", //  7 - LEX_EQ
    "<",  //  8 - LEX_LSS (меньше)
    ">",  //  9 - LEX_GTR (больше)
    "+",  // 10 - LEX_PLUS
    "-",  // 11 - LEX_MINUS
    "*",  // 12 - LEX_TIMES
    "/",  // 13 - LEX_SLASH
    "%",  // 14 - LEX_MOD
    "<=", // 15 - LEX_LEQ
    "!=", // 16 - LEX_NEQ
    ">=", // 17 - LEX_GEQ
    "{",  // 18 - LEX_LFIG
    "}",  // 19 - LEX_RFIG
    NULL
};

// Нумерации строковых констант (LEX_STR)
int count = 0;

// Возвращает следующую лексему
Lex Scanner::get_lex () {
    enum state { 
        H,      // Начальное состояние (Home)
        IDENT,  // Идентификаторы и спец слова
        NUMB,   // Числовая const
        COM,    // Комментарий /* ... */
        ALE,    // Операции сравнения: =, <, >, <=, >=, !=
        STR     // Строковая константа "..."
    };

    int d, j; // d — для чисел (например, для 23), j — индекс в TW или TD
    std::string buf; // Буфер для строки
    state CS = H; // Текущее состояние автомата
    
    while (true) {
        gc(); // Считали следующий символ

        switch (CS) {
            case H:
                // Пропуск пробельных символов
                if (c==' ' or c == '\n' or c== '\r' or c == '\t') {
                    if (c == '\n') line_counter++; // Переход на следующую строку
                } else if (isalpha(c)) { // Если символ - буква
                    buf.push_back(c); // Добавление символа в конец строки
                    CS  = IDENT;
                } else if (isdigit (c)) {
                    d   = c - '0'; // Начинаем собирать число
                    CS  = NUMB;
                } else if (c == EOF) {
                    return Lex(LEX_FIN, 0, line_counter); // Окончание входа
                } else if (c == '=' or c == '<' or c == '>' or c == '!') {
                    buf.push_back (c);
                    CS  = ALE; // Переход в состояние для проверки сравнения
                } else if (c == '"') {
                    CS = STR; // Начинаем собирать строку
                } else if (c == '/') {
                    gc();
                    if (c == '*') // Проверка начала комментария
                        CS = COM;
                    else {
                        ungetc(c, fp); // Если не комментарий - возвращаем символ обратно
                        buf.push_back('/');
                        if ((j = look(buf, TD))) {
                            return Lex(static_cast<type_of_lex> (j + static_cast<int>(LEX_FIN)), j, line_counter);
                        } else {
                            throw c;
                        }
                    }
                } else { // Другие спец символы
                    buf.push_back (c);
                    if ((j = look (buf, TD))) {
                        return Lex(static_cast<type_of_lex>  (j + static_cast<int> (LEX_FIN)), j, line_counter);
                    } else {
                        throw c;
                    }
                }
                break;

            case IDENT:
                if (isalpha(c) or isdigit(c)) {
                    buf.push_back(c); // Продолжаем собирать имя
                }
                else {
                    ungetc(c, fp); // Символ не часть идентификатора -> возвращаем назад
                    if((j = look(buf, TW))) {
                        return Lex(static_cast<type_of_lex> (j), j, line_counter); // Ключевое слово
                    } else {
                        j = put(buf); // Идентирфикатор
                        return Lex(LEX_ID, j, line_counter);
                    }
                }
                break;

            case NUMB:
                if (isdigit (c)) {
                    d = d * 10 + (c - '0'); // Накапливаем число
                } else {
                    ungetc(c, fp); // Возвращаем, если не цифра
                    return Lex(LEX_NUM, d, line_counter);
                }
                break;

            case COM:
                if (c == EOF)
                    throw "Незакрытый комментарий";
                else if (c == '\n') {
                    line_counter++; // Переход на следующую строку
                }
                else if (c == '*') {
                    gc();
                    if (c == '\n')
                        line_counter++;
                    else if (c == '/')
                        CS = H; // Нашли конец комментария
                    else if (c == EOF) //  /* ... <EOF> 
                        throw "Незакрытый комментарий";
                }
                break;

            case ALE:
                if (c == '=') { // Составная операция сравнения
                    buf.push_back(c);
                    j = look (buf, TD);
                    return Lex(static_cast<type_of_lex>  (j + static_cast<int>( LEX_FIN)), j, line_counter);
                } else { // Одиночная операция сравнения
                    ungetc(c, fp);
                    j = look(buf, TD);
                    return Lex(static_cast<type_of_lex> (j + static_cast<int>( LEX_FIN)), j, line_counter);
                }
                break;

            case STR:
            if (c == EOF)
                throw "Незавершённая строковая константа";
            if (c != '"') {
                buf.push_back(c); // Продолжаем собирать строку
            } else {
                ++count; // Уникальный индекс строки
                return Lex(LEX_STR, count, line_counter, buf);
            }
            break;
    } // end switch
  }
}

std::ostream &operator<<(std::ostream &s, Lex print_lex) {
    std::string t;
    if (print_lex.t_lex < LEX_FIN)                 // Если спец слово
        t = Scanner::TW[print_lex.t_lex];
    else if (print_lex.t_lex <= LEX_RFIG)          // Если спец символ
        t = Scanner::TD[print_lex.t_lex - LEX_FIN];
    else if (print_lex.t_lex == LEX_NUM)           // Если число
        t = "NUMB";
    else if (print_lex.t_lex == LEX_STR)           // Если строка
        t = "STR";
    else if (print_lex.t_lex == LEX_ID)            // Если идентификатор
        t = TID[print_lex.v_lex].get_name();
    else if (print_lex.t_lex == POLIZ_ADDRESS)     // Если адрес идентификатора
        t = "ADR(" + TID[ print_lex.v_lex ].get_name() + ")";
    else if (print_lex.t_lex == POLIZ_LABEL)       // Если адрес для прыжка
        t = "LBL:" + std::to_string(print_lex.v_lex);
    else if (print_lex.t_lex == POLIZ_GO)          // Если безусловный переход
        t = "GO";
    else if (print_lex.t_lex == POLIZ_FGO)         // Если условный переход (по false)
        t = "FGO";
    else if (print_lex.t_lex == LEX_DUP)           // Если дублирование верха стека
        t = "DUP";
    else
        throw print_lex;                           // Если лексема не распознанна
    
    // Вывод лексемы и информации о ней
    s << "В строке " << print_lex.line_lex << ":  лексема  " << t << "   тип лексемы: " << print_lex.t_lex
      << "   идентификатор: " << print_lex.v_lex << std::endl;
    return s;
}

std::string print_poliz(const Lex& l) {
    if (l.get_type() <  LEX_FIN)        return Scanner::TW[l.get_type()];
    if (l.get_type() <= LEX_RFIG)       return Scanner::TD[l.get_type()-LEX_FIN];
    if (l.get_type() == LEX_NUM)        return "NUMB "+std::to_string(l.get_value());
    if (l.get_type() == LEX_STR)        return "STR \""+l.get_str()+'"';
    if (l.get_type() == LEX_ID)         return "ID "+TID[l.get_value()].get_name();
    if (l.get_type() == POLIZ_ADDRESS)  return "ADDRESS ("+TID[l.get_value()].get_name()+")";
    if (l.get_type() == POLIZ_LABEL)    return "LABEL "+std::to_string(l.get_value());
    if (l.get_type() == POLIZ_GO)       return "GO";
    if (l.get_type() == POLIZ_FGO)      return "FGO";
    if (l.get_type() == LEX_DUP)        return "DUP";
    return "";
}

class Parser {
    Lex curr_lex;       // Текущая лексема
    type_of_lex c_type; // Тип текущей лексемы
    int c_val;          // Значение лексемы
    Scanner scan;       // Для получения лексем

    std::vector<int> st_int;         // Вспомогательный стек для хранения индексов переменных
    std::vector<type_of_lex> st_lex; // Стек типов, для проверки операций и присваивания

    int cycle_counter = 0; // Счётчик вхождения в циклы

    void P();  // Запускает анализ всей программы; если что-то не совпадает с синтаксисом - генерируется ошибка
    void D();  // Разбирает раздел описаний переменных
    void B();  // Разбирает блок операторов между { и }
    void S();  // Разбирает все виды операторов: =, read, write, ...
    void E();  // Разбор выражений с операциями отношения (==, <, и т.д.)
    void E1(); // Обрабатывает выражения с +, -, or
    void T();  // Обрабатывает умножение, деление, and
    void F();  // Распознаёт элементарные выражения и логические const (число, идентификатор, скобки, not)

    void dec(type_of_lex type); // Назначает тип переменным из стека st_int
    void check_id();            // Проверка, что переменная инициализирована
    void check_op();            // Проверка типов в бинарной операции
    void check_not();           // Проверка типа аргумента в not
    void eq_type();             // Проверка типов при присваивании (проверка совпадения типов переменной и выражения)
    void eq_bool();             // Проверка, что условие логическое (в if, while)
    void check_id_in_read();    // Проверка идентификатора в read()

    // Обработка цепочек присваиваний (a = b = c;)
    void check_id_chains();           // Проверка инициализации переменных
    void assignment_chain();          // Обработка цепочек

    // Считывание следующей лексемы
    void gl() {
        curr_lex = scan.get_lex();
        c_type = curr_lex.get_type();
        c_val = curr_lex.get_value();
    }

public:
    Parser (const char *program) : scan(program) { }
    void analyze(); // Запуск полного разбора файла

    std::vector<Lex> poliz; // ПОЛИЗ
    std::vector<int> break_stack; // Для переходов по break (после завершения цикла места, отведённые в этом стеке, заполняются адресами)
};

// Проверка, что идентификатор объявлен (без записи в стек)
void Parser::check_id_chains() {
    if (c_val < 0 || c_val >= static_cast<int>(TID.size()))
        throw "Идентификатор вне диапазона";
    if (!TID[c_val].get_declare())
        throw "Переменная не объявлена";
}

// Обработка цепочек присваиваний (a = b = 3;)
void Parser::assignment_chain() {
    std::vector<int> elems;  // Для переменных

    // Первая переменная уже считана
    check_id_chains();
    elems.push_back(c_val);
    gl(); // Пропускаем id

    if (c_type != LEX_ASSIGN)
        throw curr_lex;  // Не операция присваивания

    gl();  // Пропускаем =

    // id = id = .....
    while (true) {
        if (c_type == LEX_ID) {
            check_id_chains();
            elems.push_back(c_val);
            gl(); // Пропустили id

            if (c_type == LEX_ASSIGN) {
                gl(); // Пропустили = и продолжаем цепочку
            } else {
                // id, после которого нет = -> началось выражение
                break;
            }
        }
        else
            break;
    }

    E();  // Парсим выражение
    // type_of_lex expr_type = st_lex.back();

    // Проверка совместимости типов (справа‑налево)
    // for (int i = static_cast<int>(elems.size()) - 1; i >= 0; --i) {
    //     st_lex.push_back(TID[elems[i]].get_type()); // Тип переменной
    //     st_lex.push_back(expr_type);                // Тип выражения
    //     eq_type();                                  // Проверка равенства двух типов (снимает со стека тип пер. и тип выр.)
    // }

    // Генерируем ПОЛИЗ справа-налево
    for (int i = static_cast<int>(elems.size()) - 1; i >= 0; --i) {
        poliz.push_back(Lex(POLIZ_ADDRESS, elems[i]));
        poliz.push_back(Lex(LEX_ASSIGN));
        TID[ elems[i] ].put_assign();
    }
}

// Запуск полного разбора файла
void Parser::analyze() {
    gl();
    P(); // Разбор program {}
    if (c_type != LEX_FIN)
        throw curr_lex;
    std::cout << "Разбор прошёл успешно" << std::endl;

    std::cout << "--- ПОЛИЗ ---" << std::endl;
    for (size_t i = 0; i < poliz.size(); ++i)
       std::cout << i + 1 << ": " << print_poliz(poliz[i]) << std::endl;
}

void Parser::P() {
    if (c_type == LEX_PROGRAM)
        gl(); // Пропускаем program
    else
        throw curr_lex;
    B(); // Разбор {}
    if (c_type != LEX_FIN)
        throw curr_lex;
}

// Описание переменных
void Parser::D() {
    if (c_type != LEX_ID)
        throw curr_lex;
    else {
        st_int.push_back(c_val); // Сохраняем индекс переменной в стек
        gl();
        while (c_type == LEX_COMMA) { // Обработка списка переменных через запятую
            gl(); // Пропустили запятую
            if (c_type != LEX_ID)
                throw curr_lex; // После запятой должен быть идентификатор
            else {
                st_int.push_back(c_val);
                gl(); // Считываем следующую лексему
            }
        }
        if (c_type != LEX_COLON)
            throw curr_lex; // После списка переменных должно быть двоеточие :
        else {
            gl(); // Пропускаем :
            if (c_type == LEX_INT) {
                dec(LEX_INT); // Назначаем тип переменным из стека
                gl();
            } else if (c_type == LEX_BOOL) {
                dec(LEX_BOOL);
                gl();
            } else
                throw curr_lex; // Если тип не int и не bool — ошибка
        }
    }
}

// Блок кода
void Parser::B () {
    if (c_type != LEX_LFIG)
        throw curr_lex;
    gl();  // Пропускаем {

    while (c_type == LEX_INT || c_type == LEX_STRING || c_type == LEX_BOOL) {
        type_of_lex var_type = c_type;
        gl(); // Пропускаем int/string/bool

        bool first = true;
        while ( first || c_type == LEX_COMMA ) {
            if (!first)
                gl();    // Пропустили ,
            first = false;

            if (c_type != LEX_ID)
                throw curr_lex;
            int idx = c_val;  // Пропустили имя переменной

            if (TID[idx].get_declare())
                throw "Повторное объявление переменной";

            TID[idx].put_declare();
            TID[idx].put_type(var_type);
            gl();  // Пропустили идентификатор

            if (c_type == LEX_ASSIGN) {
                gl();  // Пропустили =

                bool good = (var_type == LEX_INT    && c_type == LEX_NUM)
                         || (var_type == LEX_STRING && c_type == LEX_STR)
                         || (var_type == LEX_BOOL   && (c_type == LEX_TRUE || c_type == LEX_FALSE));

                if (!good)
                    throw "Неверный тип константы при инициализации";

                if (var_type == LEX_INT || var_type == LEX_BOOL)
                    TID[idx].put_value(c_val);  // Для int/bool можно сразу запомнить

                TID[idx].put_assign();
                gl();
            }
        }

        if (c_type != LEX_SEMICOLON)
            throw curr_lex;
        gl();  // Пропустили ;
    }

    while (c_type != LEX_RFIG) {
        type_of_lex start_type = c_type;
        S();

        if (start_type == LEX_ID || start_type == LEX_READ || start_type == LEX_WRITE) {
            if (c_type != LEX_SEMICOLON)
                throw curr_lex;
            gl();
        } else {
            if (c_type == LEX_SEMICOLON)
                gl();  // Пропускаем ;, если он есть
        }
    }

    if (c_type != LEX_RFIG)
        throw curr_lex;
    gl();  // Пропускаем }
}

// Разбирает все виды операторов: =, read, write, ...
void Parser::S() {
    if (c_type == LEX_IF) {
        gl();  // Пропускаем if
    
        if (c_type != LEX_LPAREN)
            throw curr_lex;
        gl();  // Пропускаем (
    
        E(); eq_bool();

        int M = poliz.size(); // Место для метки (FGO)
        poliz.push_back(Lex()); // Временно пустая лексема под метку
        poliz.push_back(Lex(POLIZ_FGO)); // Прыгать, если условие ложно (положтлт в ПОЛИЗ)
    
        if (c_type != LEX_RPAREN)
            throw curr_lex;
        gl();  // Пропускаем )
    
        S();

        poliz[M] = Lex(POLIZ_LABEL, poliz.size()); // Заполнили пропущенную метку
    }
    
    else if (c_type == LEX_WHILE) {
        gl();  // Пропускаем while
        int M1 = poliz.size(); // Начало условия (чтобы сделать прыжок в while)
    
        if (c_type != LEX_LPAREN)
            throw curr_lex;
        gl();  // Пропускаем (
    
        E(); eq_bool();

        int M2 = poliz.size(); // Место выхода (выход из цикла)
        poliz.push_back(Lex());
        poliz.push_back(Lex(POLIZ_FGO));
    
        if (c_type != LEX_RPAREN)
            throw curr_lex;
        gl();  // Пропускаем )

        break_stack.push_back(M2); // Зашли в цикл (чтобы break работал)

        cycle_counter++;  // Зашли в цикл
        S();              // Тело цикла
        cycle_counter--;  // Вышли из цикла

        poliz.push_back(Lex(POLIZ_LABEL, M1)); // Обратно на начало условия
        poliz.push_back(Lex(POLIZ_GO));

        poliz[M2] = Lex(POLIZ_LABEL, poliz.size()); // Заполнили место выхода
        
        for (size_t i = 0; i < break_stack.size(); ++i) {
            int j = break_stack[i];
            poliz[j] = Lex(POLIZ_LABEL, poliz.size()); // Для всех break-ов проставляем выход
        }
        
        break_stack.clear(); // Очистка стека break-ов
    }

    else if (c_type == LEX_READ) {
        gl();
        if (c_type != LEX_LPAREN)
            throw curr_lex;
        gl();
    
        bool first = true;
        while (first || c_type == LEX_COMMA) {
            if (!first)
                gl();  // Пропускаем запятую
            first = false;
    
            if (c_type != LEX_ID)
                throw curr_lex;
            check_id_in_read();
    
            poliz.push_back( Lex(POLIZ_ADDRESS, c_val) );  // Куда писать
            TID[c_val].put_assign();  // Инициализация
            gl();
        }
    
        if (c_type != LEX_RPAREN)
            throw curr_lex;
        gl();
    
        poliz.push_back( Lex(LEX_READ) );
    }

    else if (c_type == LEX_WRITE) {
        gl();
        if (c_type != LEX_LPAREN)
            throw curr_lex;
        gl();
    
        bool first = true;
        while (true) {
            E();
            poliz.push_back( Lex(LEX_WRITE) );
            if (c_type != LEX_COMMA) break;
            gl();  // Пропускаем ,
        }
    
        if (c_type != LEX_RPAREN)
            throw curr_lex;
        gl();
    }
    
    else if (c_type == LEX_ID) {
        assignment_chain();  // Обработка присваиваний
    }

    // else if (c_type == LEX_ID) {
    //     check_id();
    //     gl();
    //     if (c_type == LEX_ASSIGN) {
    //         gl(); E();
    //         eq_type();
    //     } else
    //         throw curr_lex;
    // }

    else if (c_type == LEX_BREAK) {
        if (cycle_counter == 0)
            throw "break вне цикла";
        gl();  // Пропускаем break
        if (c_type != LEX_SEMICOLON)
            throw curr_lex;
        gl();  // Пропускаем ;

        poliz.push_back(Lex()); // Место для метки (временно пустое)
        poliz.push_back(Lex(POLIZ_GO)); // Безусловный переход
    
        break_stack.push_back(poliz.size() - 2);  // Адрес пустой лексемы перед GO
    }

    else if (c_type == LEX_LFIG)
            B();
    
    else
        throw curr_lex;
}

// Разбор выражений с операциями отношения (==, <, и т.д.)
void Parser::E() {
    E1();
    while (c_type == LEX_EQ  || c_type == LEX_NEQ ||
           c_type == LEX_LSS || c_type == LEX_GTR ||
           c_type == LEX_LEQ || c_type == LEX_GEQ) {
        type_of_lex op = c_type;
        gl(); // Пропускаем (==, !=, <, >= и т.д.)
        E1(); // Сначала парсим правое выражение
        st_lex.push_back(op);
        check_op();
        poliz.push_back(Lex(op));
    }
    while (c_type == LEX_OR) {
        type_of_lex op = c_type;
        gl();
        E();
        st_lex.push_back(op);
        check_op();
        poliz.push_back(Lex(op));
    }
}

// Обрабатывает выражения с +, -, or
void Parser::E1() {
    T();
    while (c_type == LEX_PLUS || c_type == LEX_MINUS) {
        type_of_lex op = c_type; // Сохраняем текущий оператор
        gl();
        T();
        st_lex.push_back(op);
        check_op();
        poliz.push_back(Lex(op));
    }
}

// Обрабатывает умножение, деление, and
void Parser::T() {
    F();
    while (c_type == LEX_TIMES || c_type == LEX_SLASH || c_type == LEX_AND || c_type == LEX_MOD) {
        type_of_lex op = c_type;
        gl();
        F();
        st_lex.push_back(op);
        check_op();
        poliz.push_back(Lex(op));
    }
}

// Распознаёт элементарные выражения и логические const (число, идентификатор, скобки, not)
void Parser::F() {
    if (c_type == LEX_MINUS) {
        gl();
        F();
        st_lex.push_back(LEX_INT);
    } else if (c_type == LEX_PLUS) {
        gl();
        F();
        st_lex.push_back(LEX_INT);    
    } else if (c_type == LEX_ID) {
        check_id();
        poliz.push_back(Lex(LEX_ID, c_val)); // Положили в ПОЛИЗ
        gl();
    } else if (c_type == LEX_NUM) {
        st_lex.push_back(LEX_INT);
        poliz.push_back(Lex(LEX_NUM, c_val)); // Положили в ПОЛИЗ
        gl();
    } else if (c_type == LEX_TRUE || c_type == LEX_FALSE) {
        st_lex.push_back(LEX_BOOL);
        poliz.push_back(Lex(c_type)); // Положили в ПОЛИЗ
        gl();
    } else if (c_type == LEX_STR) {
        st_lex.push_back(LEX_STRING);
        poliz.push_back(Lex(LEX_STR, c_val, 0, curr_lex.get_str())); // Положили в ПОЛИЗ
        gl();
    } else if (c_type == LEX_NOT) {
        gl(); F();
        check_not();
    } else if (c_type == LEX_LPAREN) {
        gl(); E();
        if (c_type != LEX_RPAREN)
            throw curr_lex;
        gl();
    } else
        throw curr_lex;
}

// Назначает тип переменным из стека st_int
void Parser::dec(type_of_lex type) {
    int i;
    while (!st_int.empty()) {
        i = st_int.back(); st_int.pop_back();
        if (TID[i].get_declare())
            throw "Переменная уже объявлена";
        TID[i].put_declare();
        TID[i].put_type(type);
    }
}

// Проверка, что переменная инициализирована
void Parser::check_id() {
    if (c_val < 0 || c_val >= static_cast<int>(TID.size()))
        throw "Идентификатор вне диапазона";
    
    if (!TID[c_val].get_declare())
        throw "Переменная не объявлена";

    if(!TID[c_val].get_assign())
        throw "Переменная не инициализирована";

    st_lex.push_back( TID[c_val].get_type() );
}

// Проверка типов в бинарной операции
void Parser::check_op() {
    // Операция
    type_of_lex op = st_lex.back(); 
    st_lex.pop_back();
    // Правый операнд
    type_of_lex t2 = st_lex.back(); 
    st_lex.pop_back();
    // Левый операнд
    type_of_lex t1 = st_lex.back(); 
    st_lex.pop_back();

    type_of_lex result;

    // string + string -> string
    if (op == LEX_PLUS && t1 == LEX_STRING && t2 == LEX_STRING) {
        result = LEX_STRING;
    }
    // int + - * / % int -> int
    else if ((op == LEX_PLUS || op == LEX_MINUS ||
              op == LEX_TIMES || op == LEX_SLASH ||
              op == LEX_MOD)
             && t1 == LEX_INT && t2 == LEX_INT) {
        result = LEX_INT;
    }
    // bool and/or bool -> bool
    else if ((op == LEX_AND || op == LEX_OR)
             && t1 == LEX_BOOL && t2 == LEX_BOOL) {
        result = LEX_BOOL;
    }
    // ==, !=, <, >, <=, >=   X op X -> bool
    else if ((op == LEX_EQ  || op == LEX_NEQ ||
              op == LEX_LSS || op == LEX_GTR ||
              op == LEX_LEQ || op == LEX_GEQ)
             && t1 == t2
             && (t1 == LEX_INT || t1 == LEX_STRING || t1 == LEX_BOOL)) {
        result = LEX_BOOL;
    }
    else {
        throw "Неправильные типы в бинарной операции";
    }

    st_lex.push_back(result);
}

// Проверка типа аргумента в not
void Parser::check_not() {
    if (st_lex.back() != LEX_BOOL)
        throw "Неправильный тип в not (применяется не к bool)";
    else st_lex.pop_back();
}

// Проверка типов при присваивании (проверка совпадения типов переменной и выражения)
void Parser::eq_type() {
    type_of_lex t;
    t = st_lex.back(); st_lex.pop_back();
    if (t != st_lex.back())
        throw "Неправильные типы в присваивании";
    st_lex.pop_back();
}

// Проверка, что условие логическое (в if, while)
void Parser::eq_bool() {
    if (st_lex.back() != LEX_BOOL)
        throw "Выражение не является логическим";
    else
        st_lex.pop_back();
}

// Проверка идентификатора в read()
void Parser::check_id_in_read() {
    if (!TID[c_val].get_declare())
        throw "Переменная, в которую считывать, не объявлена";
}

int Scanner::line_counter = 1; // Номер строки, на которой сейчас находимся

int main () {
    // try {
    //     Scanner Inp("file.txt");
    //     while (1) {
    //         Lex current_lex = Inp.get_lex(); // Получаем следующую лексему
    //         if (current_lex.get_type() == LEX_FIN)
    //             break; // Если дошли до конца — выходим
    //         std::cout << current_lex; // Печать информации о лексеме
    //     }
    //     return 0;
    // }

    try {
        Parser parser("file.txt");
        parser.analyze(); // Запуск полного разбора файла
        return 0;
    }

    catch (char c) {
        std::cout << "Непредвиденный символ: " << c << std::endl;
        return 1;
    }
    catch (Lex l) {
        std::cout << "В строке "<< Scanner::line_counter << " непредвиденная лексема: "
        << std::endl << l << std::endl;
        return 1;
    }
    catch (const char *massage) {
        std::cout << massage << std::endl;
        return 1;
    }
}
