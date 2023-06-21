
#include <vector>
#include <string>
#include <lexer.h>
#include <inputbuf.h>

#ifndef PROJECT1_PARSER_H
#define PROJECT1_PARSER_H


class Parser {

public:     //can be accessed by other programs

    Parser();

    void Print();
    void parse_Program(LexicalAnalyzer);
    void parse_global_vars(LexicalAnalyzer);
    void parse_var_list(LexicalAnalyzer);
    void parse_scope(LexicalAnalyzer);
    void parse_public_vars(LexicalAnalyzer);
    void parse_private_vars(LexicalAnalyzer);
    void parse_stmt_list(LexicalAnalyzer);
    void parse_stmt(LexicalAnalyzer);
    void syntax_error();

private:
    Token tmp;
    //InputBuffer input;

};


#endif //PROJECT1_PARSER_H
