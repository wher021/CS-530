/*  Jeff McGarrah, Willy Herrera, Ohmeko Ocampo, Camille Afalla
    Team Wisconsin
    masc1208
    prog4
    file_parse_exception.h
    CS530, Spring 2014
*/

#ifndef FILE_PARSE_EXCEPTION_H
#define FILE_PARSE_EXCEPTION_H
#include <string>

using namespace std;

class file_parse_exception {

public:
    file_parse_exception(string s) {
        message = s;
        }

    file_parse_exception() {
        message = "An error has occurred";
        }

    string getMessage() {
        return message;
    }

private:
    string message;
};

#endif
