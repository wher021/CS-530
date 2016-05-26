/*  Jeff McGarrah, Willy Herrera, Ohmeko Ocampo, Camille Afalla
    Team Wisconsin
    masc1208
    prog4
    sicxe_asm.cpp
    CS530, Spring 2014
*/

#include "sicxe_asm.h"
#define FLAGS1 0x00030000 // LDA C
#define FLAGS2 0x00038000 // LDA C,X
#define FLAGS3 0x00020000 // LDA @C
#define FLAGS4 0x00010000 // LDA #C
#define FLAGS5 0x01100000 // +LDA #C
#define FLAGS6 0x00034000 // LDA ALPHA (base)
#define FLAGS7 0x00032000 // LDA ALPHA (pc)
#define FLAGS8 0x03100000 // +LDA ALPHA
#define FLAGS9 0x0003C000 // LDA ALPHA,X (base)
#define FLAGS10 0x0003A000 // LDA ALPHA,X (pc)
#define FLAGS11 0x03900000 // +LDA ALPHA,X
#define FLAGS12 0x00024000 // LDA @ALPHA (base)
#define FLAGS13 0x00022000 // LDA @ALPHA (pc)
#define FLAGS14 0x02100000 // +LDA @ALPHA
#define FLAGS15 0x00014000 // LDA #ALPHA (base)
#define FLAGS16 0x00012000 // LDA #ALPHA (pc)
#define FLAGS17 0x01100000 // +LDA ALPHA

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2) {
        cout << "Error, you must supply the name of the file " <<
        "to process at the command line." << endl;
        exit(1);
    }
    sicxe_asm assembler(argv[1]);

    return 0;
}

//Constructor
sicxe_asm::sicxe_asm(string filename) {
    symtab sym;
    LocCtr = "00000";
    lineCounter = 0;
    this->first_pass(filename, &sym);
    LocCtr = "00000";
    lineCounter = 0;
    this->second_pass(filename, &sym);
}

void sicxe_asm::first_pass(string filename, symtab *sym) {
    try {
        opcodetab optab;
        file_parser parser(filename);
        parser.read_file();
        // Begin with START
        handle_start(&parser, sym);
        // Proceed to next instructions
        lineCounter++;
        while (to_uppercase(parser.get_token(lineCounter,1)) != "END") {
            write_line(&parser);
            add_label_to_sym(&parser, sym);
            // If opcode is in the opcodetab:
            try {
                int instSize = optab.get_instruction_size(to_uppercase(parser.get_token(lineCounter,1)));
                //handle_load_inst(&parser, sym);
                int tmp = hex_to_int(LocCtr) + instSize;
                LocCtr = int_to_hex(tmp, 5);
            }
            // Else if opcode is actually an assembler directive:
            catch(...) {
                check_asm_directive(&parser, sym);
            }
            lineCounter++;
        } // Close while
        handle_end(&parser, sym);
        write_line(&parser);
    }
    catch(file_parse_exception excpt) {
        cout << "Error: " << excpt.getMessage() << endl;
        exit(1);
    }
    catch(opcode_error_exception excpt) {
        cout << "Error: " << excpt.getMessage() << endl;
        exit(1);
    }
    catch(symtab_exception excpt) {
        cout << "Error: " << excpt.getMessage() << endl;
        exit(1);
    }
    catch (string s) {
        cout << s << endl;
        exit(1);
    }
    return;
}

void sicxe_asm::second_pass(string filename, symtab *sym) {
    try {
        opcodetab optab;
        file_parser parser(filename);
        parser.read_file();
        // Find START
        while (to_uppercase(parser.get_token(lineCounter,1)) != "START") {
            lineCounter++;
        }
        lineCounter++;
        while (to_uppercase(parser.get_token(lineCounter,1)) != "END") {
            // If opcode is in the opcodetab:
            try {
                int instSize = optab.get_instruction_size(to_uppercase(parser.get_token(lineCounter,1)));
                if (instSize == 1)
                    setFormat1MachineCode(&parser, &optab);
                else if (instSize == 2)
                    setFormat2MachineCode(&parser, &optab, sym);
                else if (instSize == 3)
                    setFormat3MachineCode(&parser, &optab, sym);
                else // instSize == 4
                    setFormat4MachineCode(&parser, &optab, sym);
            }
            // Else if opcode is actually an assembler directive:
            catch(...) {handle_asm2(&parser, sym);}
            lineCounter++;
        }
        write_lstfile(filename);
    }
    catch(file_parse_exception excpt) {
        cout << "Error: " << excpt.getMessage() << endl;
        exit(1);
    }
    catch(opcode_error_exception excpt) {
        cout << "Error: " << excpt.getMessage() << endl;
        exit(1);
    }
    catch(symtab_exception excpt) {
        cout << "Error: " << excpt.getMessage() << endl;
        exit(1);
    }
    catch (string s) {
        cout << s << endl;
        exit(1);
    }
    return;
}

//Function to convert string to upper case
string sicxe_asm::to_uppercase(string s) {
    transform(s.begin(),s.end(),s.begin(),::toupper);
    return s;
}

//Function to convert string to int
int sicxe_asm::string_to_int(string s) {
    istringstream instr(s);
    int n;
    instr >> n;
    return n;
}

//Function to convert decimal string to an integer
int sicxe_asm::dec_to_int(string s) {
    int value;
    sscanf(s.c_str(),"%d",&value);
    return value;
}

//Function to convert string hexadecimal to a decimal integer
int sicxe_asm::hex_to_int(string s) {
    int value;
    sscanf(s.c_str(),"%x",&value);
    return value;
}

//Function to convert decimal integer to string hexadecimal
string sicxe_asm::int_to_hex(int num, int width) {
    stringstream out;
    out << setw(width) << setfill('0') << hex << num;
    return to_uppercase(out.str());
}

int sicxe_asm::validate_number_operand(file_parser *parser) {
    string s = parser->get_token(lineCounter, 2);
    if (s == "")
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": no operand for " + parser->get_token(lineCounter, 1);
    if (s[0] != '$' && s[0] != '#')
        return handle_numbers(parser, s);
    else if (s[0] == '$')
        return handle_hex(parser, s);
    else if (s[0] == '#')
        return handle_imm(parser, s);
    else
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": invalid operand for " + parser->get_token(lineCounter, 1);
}

string sicxe_asm::validate_ref_operand(file_parser *parser, symtab *sym) {
    string s = to_uppercase(parser->get_token(lineCounter, 2));
    if (s == "")
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": no operand for " + parser->get_token(lineCounter, 1);
    else if (s[0] == '@')
    {
        s.erase(0,1);
        if (sym->check_sym(s))
            return sym->getAddress(s);
        else
            return "";
    }
    else if (s[0] =='#')
    {
        s.erase(0,1);
        if (s[0] == '$')
            s.erase(0,1);
        if (sym->check_sym(s))
            return sym->getAddress(s);
        else
            return "";
    }
    else if (s[s.length()-2] == ',' && (s[s.length()-1] == 'x' || s[s.length()-1] == 'X'))
    {
        s.erase(s.length()-2,2);
        if (sym->check_sym(s))
            return sym->getAddress(s);
        else
            return "";
    }
    else if (sym->check_sym(s))
        return sym->getAddress(s);
    // If not in symtab, could be FW ref. Return empty string.
    return "";
}

int sicxe_asm::handle_numbers(file_parser *parser, string s) {
        //string s = parser->get_token(lineCounter, 2);
        bool negFlag = false;
        if (s[0] == '-')
        {
            negFlag = true;
            s.erase(0,1);
        }
        for (unsigned int i = 0; i < s.length(); i++)
            if (!isdigit(s[i]))
                throw "Error at line " +
                    static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
                    ": invalid operand for " + parser->get_token(lineCounter, 1);
        if (dec_to_int(s) > 1048575)
            throw "Error at line " +
                static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
                ": operand too large for " + parser->get_token(lineCounter, 1);
        else if (negFlag)
            return (-1*dec_to_int(s));
        else
            return dec_to_int(s);
}

int sicxe_asm::handle_hex(file_parser *parser, string s) {
        //string s = parser->get_token(lineCounter, 2);
        for (unsigned int i = 1; i < s.length(); i++)
            if (!isxdigit(s[i]))
                throw "Error at line " +
                    static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
                    ": invalid operand for " + parser->get_token(lineCounter, 1);
        s.erase(0,1);
        if (s == "")
            throw "Error at line " +
                static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
                ": invalid operand for " + parser->get_token(lineCounter, 1);
        else if (s.length() > 5)
            throw "Error at line " +
                static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
                ": operand too large for " + parser->get_token(lineCounter, 1);
        return hex_to_int(s);
}

int sicxe_asm::handle_imm(file_parser *parser, string s) {
        if (s[1] == '$')
        {
            s.erase(0,1);
            return handle_hex(parser, s);
        }
        s.erase(0,1);
        if (s == "")
            throw "Error at line " +
                static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
                ": invalid operand for " + parser->get_token(lineCounter, 1);
        return handle_numbers(parser, s);
}

void sicxe_asm::add_label_to_sym(file_parser *parser, symtab *sym)
{
    if (to_uppercase(parser->get_token(lineCounter,0)) != "")
    {
        if (sym->check_sym(to_uppercase(parser->get_token(lineCounter,0))))
            throw "Error at line " +
                static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
                ": symbol " + parser->get_token(lineCounter,0) + " previously defined";
        else
            sym->insert_sym(to_uppercase(parser->get_token(lineCounter,0)), LocCtr);
    }
}

bool sicxe_asm::isblank_or_comment(file_parser *parser) {
    if ( (parser->get_token(lineCounter,0) == "" &&
            parser->get_token(lineCounter,1) == "" &&
            parser->get_token(lineCounter,2) == "" &&
            parser->get_token(lineCounter,3) == "") ||
          (parser->get_token(lineCounter,3) != ""
           && parser->get_token(lineCounter,2) == ""
           && parser->get_token(lineCounter,1) == "")
          )
            return true;
    return false;
}

void sicxe_asm::check_asm_directive(file_parser *parser, symtab *sym) {
    if (to_uppercase(parser->get_token(lineCounter,1)) == "BYTE")
        handle_byte(parser, sym);
    else if (to_uppercase(parser->get_token(lineCounter,1)) == "WORD")
        handle_word(parser, sym);
    else if (to_uppercase(parser->get_token(lineCounter,1)) == "RESB")
        handle_resb(parser);
    else if (to_uppercase(parser->get_token(lineCounter,1)) == "RESW")
        handle_resw(parser);
    else if (to_uppercase(parser->get_token(lineCounter,1)) == "BASE")
        handle_base(parser, sym);
    else if (to_uppercase(parser->get_token(lineCounter,1)) == "NOBASE")
        handle_base(parser, sym);
    else if (to_uppercase(parser->get_token(lineCounter,1)) == "EQU")
        handle_equ(parser, sym);
    // Else if blank or a comment:
    else if (!isblank_or_comment(parser))
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": invalid opcode, " + parser->get_token(lineCounter,1);
}

void sicxe_asm::handle_start(file_parser *parser, symtab *sym)
{
    // Find START
    while (to_uppercase(parser->get_token(lineCounter,1)) != "START") {
        write_blank_data();
        if (!isblank_or_comment(parser))
            throw "Error at line " +
                static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
                ": cannot have non-comment, non-blank token before START";
        lineCounter++;
    }
    // Handle label
    if (parser->get_token(lineCounter,0) == "")
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": START must have a label";
    // Make sure numerical operand is not preceded by '#'
    else if (parser->get_token(lineCounter,2)[0] == '#')
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": assembler directives cannot have immediate values";
    add_label_to_sym(parser, sym);
    // Write to lst file
    write_line(parser);
    // Set the START address
    LocCtr = int_to_hex(validate_number_operand(parser), 5);
}

void sicxe_asm::handle_byte(file_parser *parser, symtab *sym) {
    // First check label
    if (parser->get_token(lineCounter,0) == "")
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": must have label for BYTE";
    // Make sure numerical operand is not preceded by '#'
    else if (parser->get_token(lineCounter,2)[0] == '#')
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": assembler directives cannot have immediate values";
    // Validate operand
    string s = parser->get_token(lineCounter,2);
    if (s[0] == 'X' || s[0] == 'x')
        handle_byte_x(s, parser, sym);
    else if (s[0] == 'C' || s[0] == 'c')
        handle_byte_c(s, parser, sym);
    else
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": invalid operand for BYTE, " + s;
}

void sicxe_asm::handle_byte_x(string s, file_parser *parser, symtab *sym)
{
    if (s[1] != '\'' || s[s.length()-1] != '\'')
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": operand for BYTE is not properly quoted, " + s;
    else if ( (s.length()-3)%2 == 1)
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": operand for BYTE cannot have odd number of hex chars, " + s;
    else
    {
        for (unsigned int i = 2; i < s.length()-1; i++)
            if (!isxdigit(s[i]))
                throw "Error at line " +
                    static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
                    ": operand for BYTE must be in hex chars, " + s;
        s.erase(s.length()-1); // Delete last char (')
        s.erase(0,2); // Delete first two chars (X')
        lstFile[lineCounter].machineCode = int_to_hex(hex_to_int(s), 6);
        int tmp = s.length()/2 + hex_to_int(LocCtr);
        LocCtr = int_to_hex(tmp, 5);
    }
}

void sicxe_asm::handle_byte_c(string s, file_parser *parser, symtab *sym)
{
    if (s[1] != '\'' || s[s.length()-1] != '\'')
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": operand for BYTE is not properly quoted, " + s;
    else
    {
        s.erase(s.length()-1); // Delete last char (')
        s.erase(0,2); // Delete first two chars (X')
        lstFile[lineCounter].machineCode = string_to_hex(s);
        int tmp = s.length() + hex_to_int(LocCtr);
        LocCtr = int_to_hex(tmp, 5);
    }
}

void sicxe_asm::handle_word(file_parser *parser, symtab *sym) {
    if (parser->get_token(lineCounter, 0) == "")
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": must have label for WORD";
    // Make sure numerical operand is not preceded by '#'
    else if (parser->get_token(lineCounter,2)[0] == '#')
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": assembler directives cannot have immediate values";
    else
    {
        // Ensure that operand is numerical
        string hexVal = int_to_hex(validate_number_operand(parser), 6);
        // Ensure output is exactly 6 chars
        while (hexVal.length() > 6)
            hexVal.erase(0,1);
        lstFile[lineCounter].machineCode = hexVal;
    }
    int tmp = 3 + hex_to_int(LocCtr);
    LocCtr = int_to_hex(tmp, 5);
}

void sicxe_asm::handle_resb(file_parser *parser) {
    // Make sure numerical operand is not preceded by '#'
    if (parser->get_token(lineCounter,2)[0] == '#')
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": assembler directives cannot have immediate values";
    int tmp = validate_number_operand(parser) + hex_to_int(LocCtr);
    LocCtr = int_to_hex(tmp, 5);
}

void sicxe_asm::handle_resw(file_parser *parser) {
    // Make sure numerical operand is not preceded by '#'
    if (parser->get_token(lineCounter,2)[0] == '#')
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": assembler directives cannot have immediate values";
    int tmp = 3*validate_number_operand(parser) + hex_to_int(LocCtr);
    LocCtr = int_to_hex(tmp, 5);
}

void sicxe_asm::handle_base(file_parser *parser, symtab *sym) {
    // Ensure that operand is not numerical:
    string s = parser->get_token(lineCounter, 2);
    if (isdigit(s[0]) || (!isalpha(s[0]) && isdigit(s[0])))
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": BASE must use a symbol reference (no numerical operands)";
    else if (validate_ref_operand(parser, sym) != "")
        sym->insert_sym("BASE", sym->getAddress(validate_ref_operand(parser, sym)));
    // Else: operand not in symtab (yet). Leave for pass2
}

void sicxe_asm::handle_nobase(file_parser *parser, symtab *sym) {
    sym->insert_sym("BASE", "-1");
}

void sicxe_asm::handle_equ(file_parser *parser, symtab *sym) {
    // Make sure operand is not preceded by '#'
    if (parser->get_token(lineCounter,2)[0] == '#')
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": assembler directives cannot have immediate values";
    // If operand is a number:
    try {
        int tmp = validate_number_operand(parser);
        sym->insert_sym(to_uppercase(parser->get_token(lineCounter,0)), int_to_hex(tmp, 5));
    }
    // Else if it is a reference in the symtab:
    catch(...) {
        if (validate_ref_operand(parser, sym) != "")
            sym->insert_sym(to_uppercase(parser->get_token(lineCounter,0)), sym->getAddress(validate_ref_operand(parser, sym)));
        // Else, could be FW ref. Leave for pass2.
    }
}

void sicxe_asm::handle_end(file_parser *parser, symtab *sym)
{
    // Make sure the END operand matches START label
    string startAddr = to_uppercase(parser->get_token(lineCounter,2));
    if (!sym->check_sym(startAddr))
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": END operand must match START label";
    else if (sym->getAddress(startAddr) != "00000")
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": END operand must match START label";
}

void sicxe_asm::write_blank_data()
{
    lstFile[lineCounter].line = static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str();
    lstFile[lineCounter].address = "00000";
    lstFile[lineCounter].label = "";
    lstFile[lineCounter].opcode = "";
    lstFile[lineCounter].operand = "";
}

void sicxe_asm::write_line(file_parser *parser)
{
    lstFile[lineCounter].line = static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str();
    lstFile[lineCounter].address = LocCtr;
    lstFile[lineCounter].label = parser->get_token(lineCounter,0);
    lstFile[lineCounter].opcode = parser->get_token(lineCounter,1);
    lstFile[lineCounter].operand = parser->get_token(lineCounter,2);
}

void sicxe_asm::write_lstfile(string filename)
{
    ofstream outfile;
    string s = filename;
    s.erase(s.length()-4);
    s += ".lis";
    outfile.open(s.c_str(), ios::out);
    if (!outfile)
        throw "Error creating output file.";
    outfile << "Line#     " << "Address   " << "Label     "
            << "Opcode    " << "Operand   " << "MachineCode" << endl;
    outfile << "=====     " << "=======   " << "=====     "
            << "======    " << "=======   " << "===========" << endl;
    for (int i=0; lstFile[i].line != ""; i++)
    {
        write_field(&outfile, lstFile[i].line);
        write_field(&outfile, lstFile[i].address);
        write_field(&outfile, lstFile[i].label);
        write_field(&outfile, lstFile[i].opcode);
        write_field(&outfile, lstFile[i].operand);
        write_field(&outfile, lstFile[i].machineCode);
        outfile << endl;
    }
    outfile.close();
    cout << "File successfully created." << endl;
}

void sicxe_asm::write_field(ofstream *outfile, string s) {
        *outfile << s;
        int spaces = 10 - s.size();
        for (int j = 0; j < spaces; j++)
            *outfile << " ";
}

string sicxe_asm::verifyRegOperand(string s)
{
    if (s == "A" )
        return "0";
    else if (s == "X")
        return "1";
    else if (s == "L")
        return "2";
    else if (s == "B")
        return "3";
    else if (s == "S")
        return "4";
    else if (s == "T")
        return "5";
    else if (s == "PC")
        return "8";
    else if (s == "SW")
        return "9";
    else
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": Format 2 must have valid register(s) for their operands.";
}

void sicxe_asm::setFormat1MachineCode(file_parser *parser, opcodetab *optab)
{
    if (parser->get_token(lineCounter,2) != "")
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": operands cannot exist in Format 1 instructions.";
    else
        lstFile[lineCounter].machineCode = optab->get_machine_code(to_uppercase(parser->get_token(lineCounter,1)));
}

void sicxe_asm::findR1R2(string *r1, string *r2, file_parser *parser)
{
    // Find r1
    unsigned int i;
    for (i = 0; i < parser->get_token(lineCounter,2).length(); i++)
    {
        if (parser->get_token(lineCounter,2)[i] != ',')
            *r1 += parser->get_token(lineCounter,2)[i];
        else
            break;
    }
    // Continue with value i for finding r2
    for (i = i + 1; i < parser->get_token(lineCounter,2).length(); i++)
        *r2 += parser->get_token(lineCounter,2)[i];
    if (*r2 == "")
        *r2 = "0";
}

void sicxe_asm::getRegMachineCode(string *r1, string *r2, string *r1MachineCode, string *r2MachineCode, file_parser *parser, symtab *sym)
{
    // r1
    if (to_uppercase(parser->get_token(lineCounter,1)) == "SVC")
        *r1MachineCode = int_to_hex(validate_number_operand(parser), 1);
    else
    {
        string r1val = verifyRegOperand(to_uppercase(*r1));
        *r1MachineCode = r1val;
    }

    // Check if r2 not blank
    if (*r2 == "0")
        *r2MachineCode = "0";
    else if (to_uppercase(parser->get_token(lineCounter,1)) == "SHIFTR"
             || to_uppercase(parser->get_token(lineCounter,1)) == "SHIFTL")
        *r2MachineCode = int_to_hex(string_to_int(*r2), 1);
    else
    {
        string r2val = verifyRegOperand(to_uppercase(*r2));
        *r2MachineCode = r2val;
    }
}

void sicxe_asm::setFormat2MachineCode(file_parser *parser, opcodetab *optab, symtab *sym)
{
    try {
        string r1 = "", r2 = "";
        findR1R2(&r1, &r2, parser);
        string r1MachineCode, r2MachineCode;
        getRegMachineCode(&r1, &r2, &r1MachineCode, &r2MachineCode, parser, sym);
        lstFile[lineCounter].machineCode = optab->get_machine_code(to_uppercase(parser->get_token(lineCounter,1)))
            + r1MachineCode + r2MachineCode;
    }
    catch (string s) {
        cout << s << endl;
        exit(1);
    }
}

void sicxe_asm::checkNumOperandRange(file_parser *parser)
{
    int C = validate_number_operand(parser);
    try {
        if (C < -2048 || C > 2047)
            throw "Error at line " +
                static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
                ": number operand out of range.";
    }
    catch (string s) {
        cout << s << endl;
        exit(1);
    }
}

void sicxe_asm::setPCorBASEcodes(int baseCase, int pcCase, symtab *sym, int instCode, string operand) {
    try {
        if (!sym->check_sym(operand))
            throw "Error at line " +
                static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
                ": operand reference does not exist in symtab.";
        if (hex_to_int(sym->getAddress(operand)) - hex_to_int(lstFile[lineCounter].address) -3 < -2048 ||
                    hex_to_int(sym->getAddress(operand)) - hex_to_int(lstFile[lineCounter].address) -3 > 2047)
        {
            if (sym->getAddress("BASE") == "-1")
                throw "Error at line " +
                    static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
                    ": PC relative offset out of range and BASE register not set. Cannot set offset.";
            else if (hex_to_int(sym->getAddress(operand)) - hex_to_int(sym->getAddress("BASE")) < 0 ||
                        hex_to_int(sym->getAddress(operand)) - hex_to_int(sym->getAddress("BASE")) > 4095)
                throw "Error at line " +
                    static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
                    ": PC relative and BASE relative addressing out of range.";
            else
            {
                if (baseCase == 6)
                    instCode |= FLAGS6;
                else if (baseCase == 9)
                    instCode |= FLAGS9;
                else if (baseCase == 12)
                    instCode |= FLAGS12;
                else
                    instCode |= FLAGS15;
                instCode += hex_to_int(int_to_hex( hex_to_int(sym->getAddress(operand)) - hex_to_int(sym->getAddress("BASE")), 3));
                lstFile[lineCounter].machineCode = int_to_hex(instCode, 6);
            }
        }
        else
        {
            if (pcCase == 7)
                instCode |= FLAGS7;
            else if (pcCase == 10)
                instCode |= FLAGS10;
            else if (pcCase == 13)
                instCode |= FLAGS13;
            else
                instCode |= FLAGS16;
            instCode += hex_to_int(int_to_hex( hex_to_int(sym->getAddress(operand)) - hex_to_int(lstFile[lineCounter].address) - 3, 3));
            lstFile[lineCounter].machineCode = int_to_hex(instCode, 6);
        }
    }
    catch(string s) {
        cout << s << endl;
        exit(1);
    }
}

void sicxe_asm::handle_asm2(file_parser *parser, symtab *sym)
{
    if (to_uppercase(parser->get_token(lineCounter,1)) == "BASE")
    {
        // Ensure that operand is not numerical:
        string s = parser->get_token(lineCounter, 2);
        if (isdigit(s[0]) || (!isalpha(s[0]) && isdigit(s[0])))
            throw "Error at line " +
                static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
                ": BASE must use a symbol reference (no numerical operands)";
        else if (sym->check_sym(to_uppercase(s)))
            sym->insert_sym("BASE", sym->getAddress(to_uppercase(s)));
        else
            throw "Error at line " +
                static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
                ": operand for BASE not defined in the symtab.";
    }
    else if (to_uppercase(parser->get_token(lineCounter,1)) == "EQU")
    {
        // Make sure operand is not preceded by '#'
        if (parser->get_token(lineCounter,2)[0] == '#')
            throw "Error at line " +
                static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
                ": assembler directives cannot have immediate values";
        // If operand is a number:
        try {
            int tmp = validate_number_operand(parser);
            sym->insert_sym(to_uppercase(parser->get_token(lineCounter,0)), int_to_hex(tmp, 5));
        }
        // Else if it is a reference in the symtab:
        catch(...) {
            if (validate_ref_operand(parser, sym) != "")
                sym->insert_sym(to_uppercase(parser->get_token(lineCounter,0)), validate_ref_operand(parser, sym));
            else
                throw "Error at line " +
                    static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
                    ": reference operand for EQU statement not defined in symtab.";
        }
    }
}

// Borrowed from Stack Overflow
string sicxe_asm::string_to_hex(const std::string& input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();

    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    string outString = output;
    if (outString.length() > 6)
        outString.erase(6,outString.length()-6);
    return outString;
}

void sicxe_asm::handle_indFormat4(symtab *sym, string operand, int instCode)
{
    operand.erase(0,1);
    if (!sym->check_sym(operand))
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": operand reference does not exist in symtab.";
    else
    {
        instCode |= FLAGS14;
        instCode += hex_to_int(int_to_hex(hex_to_int(sym->getAddress(operand)),5));
        lstFile[lineCounter].machineCode = int_to_hex(instCode, 8);
    }
}

void sicxe_asm::handle_immFormat4(symtab *sym, file_parser *parser, string operand, int instCode)
{
    operand.erase(0,1);
    try {
        int C = validate_number_operand(parser);
        instCode |= FLAGS5;
        instCode += hex_to_int(int_to_hex(C, 5));
        lstFile[lineCounter].machineCode = int_to_hex(instCode, 8);
    }
    catch(...) {
        if (!sym->check_sym(operand))
            throw "Error at line " +
                static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
                ": operand reference does not exist in symtab.";
        else
        {
            instCode |= FLAGS17;
            instCode += hex_to_int(int_to_hex(hex_to_int(sym->getAddress(operand)),5));
            lstFile[lineCounter].machineCode = int_to_hex(instCode, 8);
        }
    }
}

void sicxe_asm::handle_xFormat4(symtab *sym, string operand, int instCode)
{
    // Remove the ,X
    operand.erase(operand.length()-2,2);
    if (!sym->check_sym(operand))
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": operand reference does not exist in symtab.";
    else
    {
        instCode |= FLAGS11;
        instCode += hex_to_int(int_to_hex(hex_to_int(sym->getAddress(operand)),5));
        lstFile[lineCounter].machineCode = int_to_hex(instCode, 8);
    }
}

void sicxe_asm::setFormat4MachineCode(file_parser *parser, opcodetab *optab, symtab *sym)
{
    int instCode = 0;
    instCode = hex_to_int(optab->get_machine_code(to_uppercase(parser->get_token(lineCounter,1))));
    instCode <<= 24;
    string operand = to_uppercase(parser->get_token(lineCounter,2));
    // Special Case
    if (to_uppercase(parser->get_token(lineCounter,1)) == "RSUB")
    {
        instCode |= FLAGS8; // n,i = 1; x,b,p = 0; e = 1
        lstFile[lineCounter].machineCode = int_to_hex(instCode, 8);
    }
    else if (operand[0] == '@')
        handle_indFormat4(sym, operand, instCode);
    else if (operand[0] == '#')
        handle_immFormat4(sym, parser, operand, instCode);
    else if (operand[operand.length()-2] == ','&& toupper(operand[operand.length()-1]) == 'X')
        handle_xFormat4(sym, operand, instCode);
    else if (!sym->check_sym(operand))
        throw "Error at line " +
            static_cast<ostringstream*>( &(ostringstream() << lineCounter+1) )->str() +
            ": operand reference does not exist in symtab.";
    else
    {
        instCode |= FLAGS8;
        instCode += hex_to_int(int_to_hex(hex_to_int(sym->getAddress(operand)),5));
        lstFile[lineCounter].machineCode = int_to_hex(instCode, 8);
    }
}

void sicxe_asm::setFormat3MachineCode(file_parser *parser, opcodetab *optab, symtab *sym)
{
    int instCode = 0;
    instCode = hex_to_int(optab->get_machine_code(to_uppercase(parser->get_token(lineCounter,1))));
    instCode <<= 16;
    string operand = to_uppercase(parser->get_token(lineCounter,2));
    // Special Case
    if (to_uppercase(parser->get_token(lineCounter,1)) == "RSUB")
    {
        instCode |= FLAGS1; // n,i = 1; x,b,p,e = 0
        lstFile[lineCounter].machineCode = int_to_hex(instCode, 6);
    }
    // Cases 3,12,13
    else if (operand[0] == '@')
    {
        operand.erase(0,1);
        try {
            int C = validate_number_operand(parser);
            checkNumOperandRange(parser);
            instCode |= FLAGS3;
            instCode += hex_to_int(int_to_hex(C, 3));
            lstFile[lineCounter].machineCode = int_to_hex(instCode, 6);
        }
        catch(...) {setPCorBASEcodes(12, 13, sym, instCode, operand);}
    }
    // Cases 4,15,16
    else if (operand[0] == '#')
    {
        operand.erase(0,1);
        try {
            int C = validate_number_operand(parser);
            checkNumOperandRange(parser);
            instCode |= FLAGS4;
            instCode += hex_to_int(int_to_hex(C, 3));
            lstFile[lineCounter].machineCode = int_to_hex(instCode, 6);
        }
        catch(...) {setPCorBASEcodes(15, 16, sym, instCode, operand);}
    }
    // Cases 2,9,10
    else if (operand[operand.length()-2] == ','&& toupper(operand[operand.length()-1]) == 'X')
    {
        // Remove the ,X
        operand.erase(operand.length()-2,2);
        try {
            int C = validate_number_operand(parser);
            checkNumOperandRange(parser);
            instCode |= FLAGS2;
            instCode += hex_to_int(int_to_hex(C, 3));
            lstFile[lineCounter].machineCode = int_to_hex(instCode, 6);
        }
        catch(...) {setPCorBASEcodes(9, 10, sym, instCode, operand);}
    }
    // Cases 1,6,7
    else
    {
        try {
            int C = validate_number_operand(parser);
            checkNumOperandRange(parser);
            instCode |= FLAGS1;
            instCode += hex_to_int(int_to_hex(C, 3));
            lstFile[lineCounter].machineCode = int_to_hex(instCode, 6);
        }
        catch(...) {setPCorBASEcodes(6, 7, sym, instCode, operand);}
    }
}
