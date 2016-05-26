/*  Jeff McGarrah, Willy Herrera, Ohmeko Ocampo, Camille Afalla
    Team Wisconsin
    masc1208
    prog4
    file_parser.h
    CS530, Spring 2014
*/

#ifndef FILE_PARSER_H
#define FILE_PARSER_H
#define MAX_LINES 255

#include <string>
#include <vector>

using namespace std;

typedef struct {
    string label, opcode, operand, comment;
} Struct;

class file_parser {
    public:
        file_parser(string);
        ~file_parser();
        void read_file();
        void print_file();
        string get_token(unsigned int, unsigned int);
        int size();

    private:
        Struct myStructure[MAX_LINES];
        string file_name;
        vector <string> contents;
        void clear_values(string* lab, string* opcd, string* oprd, string* com, int* c, char* myChar);
        void empty_tokens(string* lab, string* opcd, string* oprd, string* com);
        void advance_col(int*c, char* myChar);
        void create_label(char* myChar, int* c, string* label, unsigned int* line_counter);
        void create_comment(char* myChar, int* c, string* comment);
        void create_opcode(char* myChar, int* c, string* opcode);
        void create_operand(char* myChar, int* c, string* operand);
};

#endif
