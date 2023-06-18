
#include <vector>
#include <string>
#include <lexer.h>
#include <inputbuf.h>

#ifndef PROJECT1_PARSER_H
#define PROJECT1_PARSER_H


class Parser {

public:     //can be accessed by other programs

    void Print();
    Token parse_Program();
    Token parse_global_vars();
    Token parse_var_list();
    Token parse_scope();
    Token parse_public_vars();
    Token parse_private_vars();
    Token parse_stmt_list();
    Token parse_stmt();
    Token syntax_error();
};


#endif //PROJECT1_PARSER_H
