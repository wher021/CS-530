/*  Jeff McGarrah, Willy Herrera, Ohmeko Ocampo, Camille Afalla
    Team Wisconsin
    masc1208
    prog4
    opcodetab.h
    CS530, Spring 2014
*/

#ifndef OPCODETAB_H
#define OPCODETAB_H
#include <map>
#include <string>

using namespace std;

class opcodetab {
    public:
        // ctor
        // creates a new dictionary structure and loads all of the opcodes for
        // the SIC/XE architecture into the table.  Use the STL map for this.
        opcodetab();

        // takes a SIC/XE opcode and returns the machine code
        // equivalent as a two byte string in hexadecimal.
        // Example:  get_machine_code("ADD") returns the value 18
        // Note that opcodes may be prepended with a '+'.
        // throws an opcode_error_exception if the opcode is not
        // found in the table.
        string get_machine_code(string);

        // takes a SIC/XE opcode and returns the number of bytes
        // needed to encode the instruction, which is an int in
        // the range 1..4.
        // NOTE: the opcode must be prepended with a '+' for format 4.
        // throws an opcode_error_exception if the opcode is not
        // found in the table.
        int get_instruction_size(string);

    private:
        // your variables and private methods go here
        map<string, pair<string, int> > codeMap;
};

const string Ops[59] = {
    "ADD","ADDF","ADDR","AND","CLEAR","COMP","COMPF","COMPR","DIV","DIVF","DIVR","FIX",
    "FLOAT","HIO","J","JEQ","JGT","JLT","JSUB","LDA","LDB","LDCH","LDF","LDL",
    "LDS","LDT","LDX","LPS","MUL","MULF","MULR","NORM","OR","RD","RMO","RSUB",
    "SHIFTL","SHIFTR","SIO","SSK","STA","STB","STCH","STF","STI","STL","STS","STSW",
    "STT","STX","SUB","SUBF","SUBR","SVC","TD","TIO","TIX","TIXR","WD"
};

const pair<string, int> Pairs[59] = {
    pair<string,int>("18",3),pair<string,int>("58",3),pair<string,int>("90",2),
    pair<string,int>("40",3),pair<string,int>("B4",2),pair<string,int>("28",3),
    pair<string,int>("88",3),pair<string,int>("A0",2),pair<string,int>("24",3),
    pair<string,int>("64",3),pair<string,int>("9C",2),pair<string,int>("C4",1),
    pair<string,int>("C0",1),pair<string,int>("F4",1),pair<string,int>("3C",3),
    pair<string,int>("30",3),pair<string,int>("34",3),pair<string,int>("38",3),
    pair<string,int>("48",3),pair<string,int>("00",3),pair<string,int>("68",3),
    pair<string,int>("50",3),pair<string,int>("70",3),pair<string,int>("08",3),
    pair<string,int>("6C",3),pair<string,int>("74",3),pair<string,int>("04",3),
    pair<string,int>("D0",3),pair<string,int>("20",3),pair<string,int>("60",3),
    pair<string,int>("98",2),pair<string,int>("C8",1),pair<string,int>("44",3),
    pair<string,int>("D8",3),pair<string,int>("AC",2),pair<string,int>("4C",3),
    pair<string,int>("A4",2),pair<string,int>("A8",2),pair<string,int>("F0",1),
    pair<string,int>("EC",3),pair<string,int>("0C",3),pair<string,int>("78",3),
    pair<string,int>("54",3),pair<string,int>("80",3),pair<string,int>("D4",3),
    pair<string,int>("14",3),pair<string,int>("7C",3),pair<string,int>("E8",3),
    pair<string,int>("84",3),pair<string,int>("10",3),pair<string,int>("1C",3),
    pair<string,int>("5C",3),pair<string,int>("94",2),pair<string,int>("B0",2),
    pair<string,int>("E0",3),pair<string,int>("F8",1),pair<string,int>("2C",3),
    pair<string,int>("B8",2),pair<string,int>("DC",3)
};

#endif
