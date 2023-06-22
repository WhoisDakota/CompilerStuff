
#include <vector>
#include <string>
#include "lexer.h"
#include "inputbuf.h"

#ifndef PROJECT1_PARSER_H
#define PROJECT1_PARSER_H


class Parser {

public:     //can be accessed by other programs

    LexicalAnalyzer lexer;
    Token token;
    TokenType tempTType;
    int parse_Program();

private:

    int parse_global_vars();
    int parse_var_list();
    int parse_scope();
    int parse_public_vars();
    int parse_private_vars();
    int parse_stmt_list();
    int parse_stmt();
    void syntax_error();



    //InputBuffer input;

};


#endif //PROJECT1_PARSER_H
