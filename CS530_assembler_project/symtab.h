/*  Jeff McGarrah, Willy Herrera, Ohmeko Ocampo, Camille Afalla
    Team Wisconsin
    masc1208
    prog4
    symtab.h
    CS530, Spring 2014
*/

#ifndef SYMTAB_H
#define SYMTAB_H

#include "symtab_exception.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;


class symtab {
    public:
        symtab();
        ~symtab();
        void insert_sym(string label, string address);
        bool check_sym(string label);
        string getAddress(string label);

    private:
        map<string,string> myMap;
        void remove_sym(string label);

};

#endif

