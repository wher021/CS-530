/*  Jeff McGarrah, Willy Herrera, Ohmeko Ocampo, Camille Afalla
    Team Wisconsin
    masc1208
    prog4
    sicxe_asm.h
    CS530, Spring 2014
*/

#include "opcodetab.h"
#include "opcode_error_exception.h"
#include "file_parser.h"
#include "file_parse_exception.h"
#include "symtab.h"
#include "symtab_exception.h"
#include <iostream>

#include <sstream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <iomanip> //std::setw
#include <algorithm> //std::transform

#define MAX_LINES 255

typedef struct {
    string line, address, label, opcode, operand, machineCode;
} lstStruct;

class sicxe_asm {
    public:
        // Constructor
        sicxe_asm(string filename);
    private:
        // Private Data Members
        string LocCtr;
        int lineCounter;
        lstStruct lstFile[MAX_LINES];
        // First and Second Pass Function
        void first_pass(string filename, symtab *sym);
        void second_pass(string filename, symtab *sym);
        // Misc Helper Functions
        string to_uppercase(string s);
        int string_to_int(string s);
        int dec_to_int(string s);
        int hex_to_int(string s);
        string int_to_hex(int num, int width);
        void add_label_to_sym(file_parser *parser, symtab *sym);
        // Validation Functions
        string validate_ref_operand(file_parser *parser, symtab *sym);
        int validate_number_operand(file_parser *parser);
        int handle_numbers(file_parser *parser, string s);
        int handle_hex(file_parser *parser, string s);
        int handle_imm(file_parser *parser, string s);
        bool isblank_or_comment(file_parser *parser);
        // Assembler directive handlers
        void check_asm_directive(file_parser *parser, symtab *sym);
        void handle_start(file_parser *parser, symtab *sym);
        void handle_byte(file_parser *parser, symtab *sym);
        void handle_byte_x(string s, file_parser *parser, symtab *sym);
        void handle_byte_c(string s, file_parser *parser, symtab *sym);
        void handle_word(file_parser *parser, symtab *sym);
        void handle_resb(file_parser *parser);
        void handle_resw(file_parser *parser);
        void handle_base(file_parser *parser, symtab *sym);
        void handle_nobase(file_parser *parser, symtab *sym);
        void handle_equ(file_parser *parser, symtab *sym);
        void handle_end(file_parser *parser, symtab *sym);
        // Output file functions
        void write_blank_data();
        void write_line(file_parser *parser);
        void write_lstfile(string filename);
        void write_field(ofstream *outfile, string s);
        // Second Pass Functions
        void setFormat1MachineCode(file_parser *parser, opcodetab *optab);
        void findR1R2(string *r1, string *r2, file_parser *parser);
        string verifyRegOperand(string s);
        void getRegMachineCode(string *r1, string *r2, string *r1MachineCode,
                                          string *r2MachineCode, file_parser *parser, symtab *sym);
        void setFormat2MachineCode(file_parser *parser, opcodetab *optab, symtab *sym);
        void checkNumOperandRange(file_parser *parser);
        void setPCorBASEcodes(int baseCase, int pcCase, symtab *sym, int instCode, string operand);
        void handle_asm2(file_parser *parser, symtab *sym);
        void handle_indFormat4(symtab *sym, string operand, int instCode);
        void handle_immFormat4(symtab *sym, file_parser *parser, string operand, int instCode);
        void handle_xFormat4(symtab *sym, string operand, int instCode);
        void setFormat4MachineCode(file_parser *parser, opcodetab *optab, symtab *sym);
        void setFormat3MachineCode(file_parser *parser, opcodetab *optab, symtab *sym);
        // Helper
        string string_to_hex(const std::string& input);
};
