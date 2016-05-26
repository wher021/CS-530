/*  Jeff McGarrah, Willy Herrera, Ohmeko Ocampo, Camille Afalla
    Team Wisconsin
    masc1208
    prog4
    opcode_error_exception.h
    CS530, Spring 2014
*/

#ifndef OPCODE_ERROR_EXCEPTION_H
#define OPCODE_ERROR_EXCEPTION_H
#include <string>

using namespace std;

class opcode_error_exception {

public:
    opcode_error_exception(string s) {
        message = s;
        }

    opcode_error_exception() {
        message = "An error has occurred";
        }

    string getMessage() {
        return message;
    }

private:
    string message;
};

#endif
