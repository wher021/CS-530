/*  Jeff McGarrah, Willy Herrera, Ohmeko Ocampo, Camille Afalla
    Team Wisconsin
    masc1208
    prog4
    opcodetab.cc
    CS530, Spring 2014
*/

#include "opcodetab.h"
#include "opcode_error_exception.h"
#include <iostream>

using namespace std;

opcodetab::opcodetab()
{
    for (unsigned int i = 0; i < sizeof(Ops)/sizeof(Ops[0]); i++)
        codeMap[Ops[i]] = Pairs[i];
}

string opcodetab::get_machine_code(string s)
{
    // Flag
    bool plusFlag = false;

    // Remove extended formatting if it exists (index 0, 1 char)
    if (s[0] == '+')
    {
        s.erase(0,1);
        plusFlag = true;
    }

    // Convert to all uppercase
    for (unsigned int i = 0; i < s.length(); i++)
        s[i] = toupper(s[i]);

    // Check if given string exists in codeMap
    if (this->codeMap.count(s) > 0)
    {
        // Check if '+' appropriate
        if (plusFlag && this->codeMap[s].second != 3)
            throw opcode_error_exception("Use of '+' not acceptable with given opcode " + s + '.');
        else
            return this->codeMap[s].first;
    }
    else
        throw opcode_error_exception("Opcode " + s + " is not valid.");
}

int opcodetab::get_instruction_size(string s)
{
    // Flag
    bool plusFlag = false;

    // First check for validity of input
    if (s[0] == '+')
    {
        s.erase(0,1);
        plusFlag = true; // Set flag if there was a '+'
    }

    // Convert to all uppercase
    for (unsigned int i = 0; i < s.length(); i++)
        s[i] = toupper(s[i]);

    // Check if given string exists in codeMap
    if (this->codeMap.count(s) > 0)
    {
        // Categorize size
        if (plusFlag)
        {
            if (this->codeMap[s].second == 3)
                return 4;
            else
                throw opcode_error_exception("Use of '+' not acceptable with given opcode " + s + '.');
        }
        else
            return this->codeMap[s].second;
    }
    else
        throw opcode_error_exception("Opcode " + s + " is not valid.");
}
