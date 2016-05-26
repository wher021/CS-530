/*  Willy Herrera, Jeff McGarrah, Ohmeko Ocampo, Camille Afalla
    Team Wisconsin
    masc1208
    prog4
    symtab.h
    CS530, Spring 2014
*/
#include "symtab.h"


symtab::symtab()
{
    this->insert_sym("BASE", "-1");
}
symtab::~symtab()
{

}

void symtab::insert_sym(string label,string address)
{
    if (this->check_sym((label)))
        this->remove_sym(label);
    pair<string, string> tmp;
    tmp.first = label;
    tmp.second = address;
    myMap.insert(tmp);
}

bool symtab::check_sym(string label) {
    if (myMap.count(label) > 0)
        return true;
    return false;
}

string symtab::getAddress(string label)
{
    if (this->check_sym(label) == false)
        throw symtab_exception("Cannot get value for symbol " + label + ". Symbol is not in the symtab.");
    return this->myMap[label];
}

void symtab::remove_sym(string label)
{
    if (this->check_sym(label) == false)
        throw symtab_exception("Cannot remove symbol " + label + " from symtab. Symbol does not exist.");
    myMap.erase(label);
}
