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
    LEX_NULL,
    LEX_IF, LEX_AND, LEX_REAL, LEX_STRING, LEX_DO, LEX_GOTO, LEX_FALSE, LEX_INT,                          
    LEX_NOT, LEX_OR, LEX_PROGRAM, LEX_READ, LEX_TRUE, LEX_BREAK, LEX_WRITE, LEX_WHILE,

    LEX_FIN,

    LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ, LEX_LSS,
    LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH,
    LEX_MOD,
    LEX_LEQ, LEX_NEQ, LEX_GEQ,
    LEX_LFIG, LEX_RFIG,
    
    LEX_NUM,
    LEX_STR,
    LEX_ID,

    POLIZ_LABEL,
    POLIZ_ADDRESS,
    POLIZ_GO,
    POLIZ_FGO
};

class Lex {
    type_of_lex t_lex;
    int v_lex;
    int line_lex;
    std::string s_lex;
public:
    explicit Lex ( type_of_lex t = LEX_NULL, int v = 0, int line = 0, std::string str = ""): 
                                t_lex (t), v_lex (v), line_lex(line), s_lex(std::move(str)) { }
    
    type_of_lex  get_type () const { 
    	  return t_lex; 
    }

    int get_value () const { 
    	  return v_lex; 
    }

    friend std::ostream &operator<<( std::ostream &s, Lex l );
};

class Ident {
    std::string name;
    bool declare;
    type_of_lex type;
    bool assign;
    int value;
public:
    Ident() { 
        declare = false; 
        assign = false; 
    }

    bool operator==(const std::string &s) const {
        return name == s; 
    }

    explicit Ident(const std::string n) {
        name = std::move(n);
        declare = false; 
        assign = false; 
    }

    std::string get_name() const { 
        return name; 
    }

    bool get_declare() const { 
        return declare; 
    }

    void put_declare() { 
        declare = true; 
    }

    type_of_lex get_type() const { 
        return type;
    }

    void put_type(type_of_lex t) { 
        type = t; 
    }

    bool get_assign() const { 
        return assign; 
    }

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

int put ( const std::string &buf ) {
    std::vector<Ident>::iterator k;
    if ((k = find(TID.begin(), TID.end(), buf)) != TID.end())
        return static_cast<int>(std::distance(TID.begin(), k));
    TID.push_back ( Ident(buf) );
    return static_cast<char>(TID.size() - 1);
}

class Scanner {
    FILE * fp;
    char   c;

    static int look (const std::string buf, const char **list) {
        int i = 0;
        while (list[i]) {
            if (buf == list[i])
                return i;
            ++i;
        }
        return 0;
    }

    void gc () {
        c = fgetc (fp);
    }

public:
    static const char *TW[], *TD[];
    static int line_counter;
    explicit Scanner (const char *program) {
        if (!(fp = fopen(program, "r"))) 
            throw  "can`t open file"; //
    }

    Lex get_lex ();
};


// Спец слова
const char *Scanner::TW[] = {
    "", "if", "and", "real", "string", "do",
    "goto","false","int","not","or","program","read",
    "true", "break", "write", "while", NULL
};

// Спец символы
const char *Scanner::TD[] = {
    "", ";", ",", ":", "=", "(", ")", "==",
    "<", ">", "+", "-", "*", "/", "%", "<=", "!=", ">=", 
    "{", "}", NULL
};

int count = 0;

Lex Scanner::get_lex () {
    enum state { 
        H, IDENT, NUMB, COM, ALE, STR
    };
    int d, j;
    std::string buf;
    state CS = H;
    
    while (true) {
        gc ();
        switch (CS) {
            case H:
                if (c==' ' or c == '\n' or c== '\r' or c == '\t') {
                    if (c == '\n') line_counter++;
                } else if (isalpha(c)) {
                    buf.push_back(c);
                    CS  = IDENT;
                } else if (isdigit (c)) {
                    d   = c - '0';
                    CS  = NUMB;
                } else if (c == EOF) { 
                    return Lex(LEX_FIN, 0, line_counter);
                } else if (c == '=' or c == '<' or c == '>' or c == '!') { 
                    buf.push_back (c);
                    CS  = ALE; 
                } else if (c == '"') {
                    CS = STR;
                } else if (c == '/') {
                    gc();
                    if (c == '*') CS = COM;
                    else {
                        ungetc(c, fp);
                        buf.push_back('/');
                        if ((j = look(buf, TD))) {
                            return Lex(static_cast<type_of_lex> (j + static_cast<int>(LEX_FIN)), j, line_counter);
                        } else {
                            throw c;
                        }
                    }
                } else {
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
                    buf.push_back(c); 
                }
                else {
                    ungetc(c, fp);
                    if((j = look(buf, TW))) {
                        return Lex(static_cast<type_of_lex> (j), j, line_counter);
                    } else {
                        j = put(buf);
                        return Lex(LEX_ID, j, line_counter);
                    }
                }
                break;

            case NUMB:
                if (isdigit (c)) {
                    d = d * 10 + (c - '0');
                } else {
                    ungetc(c, fp);
                    return Lex(LEX_NUM, d, line_counter);
                }
                break;

            case COM:
                if (c == EOF)
                    throw c;
                else if (c == '\n'){
                    line_counter++;
                }
                else if (c == '*') {
                    gc();
                    if (c =='\n'){
                        line_counter++;
                    }
                    else if (c == '/')
                        CS = H;
                    else if (c == EOF)
                        throw c;
                }
                break;

            case ALE:
                if (c == '=') {
                    buf.push_back(c);
                    j = look (buf, TD);
                    return Lex(static_cast<type_of_lex>  (j + static_cast<int>( LEX_FIN)), j, line_counter);
                } else {
                    ungetc(c, fp);
                    j = look(buf, TD);
                    return Lex(static_cast<type_of_lex> (j + static_cast<int>( LEX_FIN)), j, line_counter);
                }
                break;

            case STR:
                if (c != '"') {
                    buf.push_back(c); 
                }
            else {
                ++count;
                return Lex(LEX_STR, count, line_counter, buf);
            }
            break;
    } //end switch
  }
}

std::ostream &operator<<(std::ostream &s, Lex print_lex) {
    std::string t;
    if (print_lex.t_lex < LEX_FIN)
        t = Scanner::TW[print_lex.t_lex];
    else if (print_lex.t_lex <= LEX_RFIG)
        t = Scanner::TD[print_lex.t_lex - LEX_FIN];
    else if (print_lex.t_lex == LEX_NUM)
        t = "NUMB";
    else if (print_lex.t_lex == LEX_STR)
        t = "STR";
    else if (print_lex.t_lex == LEX_ID)
        t = TID[print_lex.v_lex].get_name();
    else
        throw print_lex;
    s << "Line: " << print_lex.line_lex << ": (type: " << t << " , index: " << print_lex.t_lex
      << " , counter: " << print_lex.v_lex << ");" << std::endl;
    std::cout << std::endl;
    return s;
}

int Scanner::line_counter=1;

int main () {
    try {
        Scanner Inp("file.txt");
        while (1) {
            Lex current_lex = Inp.get_lex();
            if (current_lex.get_type() == LEX_FIN)
                break;
            std::cout << current_lex;
        }
        return 0;
    }

    catch (char c) {
        std::cout << "Непредвиденный символ: " << c << std::endl;
        return 1;
    }
    catch (Lex l) {
        std::cout << "Непредвиденная лексема: " << l << std::endl;
        return 1;
    }
    catch (const char *source) {
        std::cout << source << std::endl;
        return 1;
    }
}