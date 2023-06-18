//
// Created by dakota on 6/17/23.
//

#include "parser.h"
#include <iostream>
#include <istream>
#include <string>
#include <cctype>
#include <lexer.h>
#include <inputbuf.h>

/*
 * need one function for each non-terminal of grammar to handle
 * parsing of such non-terminal
 * ie:
 * parse_X()
 * */

Token tmp;

void Parser::Print() {

}


Token Parser::parse_Program() {
    tmp.token_type =

}

Token Parser::parse_global_vars(Token) {

}

Token Parser::parse_var_list(Token) {

}

Token Parser::parse_scope(Token) {

}

Token Parser::parse_public_vars(Token) {

}

Token Parser::parse_private_vars(Token) {

}

Token Parser::parse_stmt_list(Token) {

}

Token Parser::parse_stmt(Token) {


}



Token Parser::syntax_error() {
    std::cout << "Syntax Error\n";
    exit(1);

}
