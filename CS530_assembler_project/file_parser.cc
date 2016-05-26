/*  Jeff McGarrah, Willy Herrera, Ohmeko Ocampo, Camille Afalla
    Team Wisconsin
    masc1208
    prog4
    file_parser.cc
    CS530, Spring 2014
*/

#include "file_parser.h"
#include "file_parse_exception.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <ctype.h>
#include <sstream>
#define MAX_CHARS 255

using namespace std;

file_parser::file_parser(string f_name)
{
    file_name = f_name;
}

file_parser::~file_parser() {}

void file_parser::read_file()
{
    ifstream infile;
    string line;
    infile.open(file_name.c_str(),ios::in); //.c_str() converts to c string
    if(!infile)
    {
        throw file_parse_exception("File not found");
    }
    while(!infile.eof())
    {
        getline(infile,line);
        contents.push_back(line); //vector structure contents is defined in the private section of this class
    }
    infile.close();

    int c = 0;
    char myChar[MAX_CHARS];
    string label,opcode,operand,comment;

    for(unsigned int line_counter = 0;line_counter<contents.size();line_counter++)
    {
        if(contents[line_counter].empty()) //Blank line
            empty_tokens(&label, &opcode, &operand, &comment);
        else
        {
            strcpy(myChar, contents[line_counter].c_str());//copy C++ string to C string
            c=0;
            while(true)
            {
                // Check for LABEL
                if (!isspace(myChar[c]) && myChar[c] != '.') // LABEL exists
                    create_label(myChar, &c, &label, &line_counter);
                else if (myChar[c] == '.') // COMMENT (col 0)
                {
                    label = "";
                    opcode = "";
                    operand = "";
                    create_comment(myChar, &c, &comment);
                    break;
                }
                else // No LABEL
                {
                    label = "";
                    advance_col(&c, myChar);
                    if (myChar[c] == '.') // COMMENT (col 1)
                    {
                        opcode = "";
                        operand = "";
                        create_comment(myChar, &c, &comment);
                        break;
                    }
                    else // Has OPCODE
                        create_opcode(myChar, &c, &opcode);
                }
                if (label != "") // LABEL cases
                {
                    advance_col(&c, myChar);
                    if (myChar[c] == '\0') // LABEL only
                        break;
                    else if (myChar[c] == '.') // LABEL,COMMENT
                    {
                        opcode = "";
                        operand = "";
                        create_comment(myChar, &c, &comment);
                        break;
                    }
                    else if (!isspace(myChar[c])) // Next token is OPCODE
                    {
                        create_opcode(myChar, &c, &opcode);
                        advance_col(&c, myChar);
                        if (myChar[c] == '\0') // LABEL,OPCODE
                            break;
                        else if (myChar[c] == '.') //LABEL,OPCODE,COMMENT
                        {
                            operand = "";
                            create_comment(myChar, &c, &comment);
                            break;
                        }
                        else  // has OPERAND
                        {
                            create_operand(myChar, &c, &operand);
                            advance_col(&c, myChar);
                            if (myChar[c] == '\0') // LABEL,OPCODE,OPERAND
                            {
                                comment = "";
                                break;
                            }
                            else if (myChar[c] == '.') // LABEL,OPCODE,OPERAND,COMMENT
                            {
                                create_comment(myChar, &c, &comment);
                                break;
                            }
                            else
                                throw file_parse_exception("Too many tokens: Line " +
                                                           static_cast<ostringstream*>( &(ostringstream() << (line_counter+1)) )->str());
                        }
                    }
                }
                else // no label cases
                {
                    advance_col(&c, myChar);
                    if (myChar[c] == '\0') // OPCODE only
                    {
                        operand = "";
                        comment = "";
                        break;
                    }
                    else if (myChar[c] == '.') // OPCODE,COMMENT
                    {
                        operand = "";
                        create_comment(myChar, &c, &comment);
                        break;
                    }
                    else if (!isspace(myChar[c])) // Next token is OPERAND
                    {
                        create_operand(myChar, &c, &operand);
                        advance_col(&c, myChar);
                        if (myChar[c] == '\0') // OPCODE,OPERAND
                        {
                            comment = "";
                            break;
                        }
                        else if (myChar[c] == '.') // OPCODE,OPERAND,COMMENT
                        {
                            create_comment(myChar, &c, &comment);
                            break;
                        }
                        else
                            throw file_parse_exception("Too many tokens: Line " +
                                                       static_cast<ostringstream*>( &(ostringstream() << (line_counter+1)) )->str());
                    }
                }
            }
        }
        this->myStructure[line_counter].label = label;
        this->myStructure[line_counter].opcode = opcode;
        this->myStructure[line_counter].operand = operand;
        this->myStructure[line_counter].comment = comment;
        clear_values(&label, &opcode, &operand, &comment, &c, myChar);
    }
    cout<<endl;
}

void file_parser::print_file()
{
    for (int i = 0; i < this->size(); i++)
    {
        cout << get_token(i,0);
        int spaces = 10 - (get_token(i,0)).size();
        for (int j = 0; j < spaces; j++)
            cout << " ";

        cout << get_token(i,1);
        spaces = 10 - (get_token(i,1)).size();
        for (int j = 0; j < spaces; j++)
            cout << " ";

        cout << get_token(i,2);
        spaces = 10 - (get_token(i,2)).size();
        for (int j = 0; j < spaces; j++)
            cout << " ";

        cout << get_token(i,3) << endl;
    }
}
string file_parser::get_token(unsigned int r, unsigned int c)
{
    if (c==0)
    {
        string s = this->myStructure[r].label;
        //for (unsigned int i = 0; i < s.length(); i++)
        //    s[i] = toupper(s[i]);
        return s;
    }
    else if (c==1)
    {
        string s = this->myStructure[r].opcode;
        //for (unsigned int i = 0; i < s.length(); i++)
        //    s[i] = toupper(s[i]);
        return s;
    }
    else if (c==2)
    {
        string s = this->myStructure[r].operand;
        //for (unsigned int i = 0; i < s.length(); i++)
        //    s[i] = toupper(s[i]);
        return s;
    }
    else
    {
        string s = this->myStructure[r].comment;
        //for (unsigned int i = 0; i < s.length(); i++)
        //    s[i] = toupper(s[i]);
        return s;
    }
}
int file_parser::size()
{
    return (this->contents).size();
}

void file_parser::clear_values(string *lab, string *opcd, string *oprd, string *com, int *c, char* myChar)
{
    lab->clear();
    opcd->clear();
    oprd->clear();
    com->clear();
    (*c) = 0;
    myChar[0] = '\0';
}

void file_parser::empty_tokens(string *lab, string *opcd, string *oprd, string *com)
{
  *lab = "";
  *opcd = "";
  *oprd = "";
  *com = "";
}

void file_parser::advance_col(int*c, char* myChar)
{
    while (isspace(myChar[*c]))
        (*c)++;
}

void file_parser::create_label(char* myChar, int* c, string* label, unsigned int* line_counter)
{
    if (isalpha(myChar[*c]))
    {
        *label += myChar[*c];
        (*c)++;
        int charCount = 1;
        while (!isspace(myChar[*c]) && myChar[*c] != '\0')
        {
            if (isalnum(myChar[*c]))
            {
                if (charCount < 8) // Keep label to 8 char max
                {
                    *label += myChar[*c];
                    charCount++;
                }
                (*c)++;
            }
            else
                throw file_parse_exception("Label is not alphanumeric: Line " +
                                            static_cast<ostringstream*>( &(ostringstream() << (*line_counter+1)) )->str());
        }
    }
    else
        throw file_parse_exception("Label does not begin with letter: Line " +
                                    static_cast<ostringstream*>( &(ostringstream() << (line_counter+1)) )->str());
}

void file_parser::create_comment(char* myChar, int* c, string* comment)
{
    while(myChar[*c]!= '\0')
    {
        *comment+=myChar[*c];
        (*c)++;
    }
}

void file_parser::create_opcode(char* myChar, int* c, string* opcode)
{
    while(!isspace(myChar[*c]) && myChar[*c] != '\0')
    {
        *opcode += myChar[*c];
        (*c)++;
    }
}

void file_parser::create_operand(char* myChar, int* c, string* operand)
{
    while (!isspace(myChar[*c]) && myChar[*c] != '\0')
    {
        if (myChar[*c] != '\'')
        {
            *operand += myChar[*c];
            (*c)++;
        }
        else
        {
            *operand += myChar[*c]; //Push in first quote
            (*c)++;
            while (myChar[*c] != '\'' && myChar[*c] != '\0') //Push all in between
            {
                *operand += myChar[*c];
                (*c)++;
            }
            if (myChar[*c] == '\'') //If end quote, push in
            {
                *operand += myChar[*c];
                (*c)++;
            }
        }
    }
}
