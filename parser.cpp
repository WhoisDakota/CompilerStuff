//
// Dakota Burke
//

#include "parser.h"
#include <iostream>
#include <istream>
#include <string>
#include <cctype>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "inputbuf.h"

int Parser::parse_Program(void) {
   Token t =  lexer.GetToken();
    int returnVal;

   //check first set (First(Program): {ID})
   while(t.token_type != END_OF_FILE) {
       if (t.token_type == ID) {
           Token t = lexer.GetToken();
           if(t.token_type == LBRACE){
               tempTType = lexer.UngetToken(t);
               returnVal = parse_scope();
               std::cout << "\n Parsed program -> epsilon scope \n";
           }
           else if (t.token_type == COMMA){
               tempTType = lexer.UngetToken(t);
               returnVal = parse_global_vars();
               returnVal = parse_scope();
               std::cout << "\n Parsed program -> global_vars scope";
           }
           else if(t.token_type == END_OF_FILE){
           return(0);
            }
           else {
               syntax_error();
           }
       }
       t = lexer.GetToken();
   }
   return(0);
}

int Parser::parse_global_vars(void) {
    Token t = lexer.GetToken();
    int returnVal;
    //check first set (First(global_vars): {ID, epsilon} )
    if (t.token_type == ID) {
        t = lexer.GetToken();
        if (t.token_type == COMMA) {
            tempTType = lexer.UngetToken(t);
            returnVal = parse_var_list();
            t = lexer.GetToken();
            if (t.token_type == SEMICOLON) {
                std::cout << "global_vars->var_list.SEMICOLON\n";
            }
            else {
                syntax_error();
            }
        }
        else if (t.token_type == LBRACE) {
            tempTType = lexer.UngetToken(t);
            returnVal = parse_scope();
            std::cout << "global_vars -> epsilon -> scope";
        }
        else {
            syntax_error();
        }
    }
    else{
        syntax_error();
    }
        // epsilon exists in first set, so check follow set see if t exists in follow(global_vars): {$, RBRACE}
    /*else if (t.token_type == RBRACE) {
        lexer.UngetToken(t);
        printf("global_vars->epsilon\n");
        return (0);
    }
    */
    return(0);
}


int Parser::parse_var_list(void) {
    Token t = lexer.GetToken();
    int returnVal;
    //check first set (First(var_list): {ID} )

    if(t.token_type == ID){
        Token t = lexer.GetToken();
        if (t.token_type == COMMA){
            returnVal = parse_var_list();
            std::cout << "var_list->ID COMMA var_list\n";
        }
        else if (lexer.IsKeyword(t.lexeme) && t.token_type != ID){
            tempTType = lexer.UngetToken(t);
            std::cout << "var_list->ID\n";
        }
        else{
            syntax_error();
        }
    }
    else{
        syntax_error();
    }
    return(0);
}

int Parser::parse_scope(void) {
    Token t = lexer.GetToken();
    int returnVal;
    //First(scope): {ID}
    if(t.token_type == ID){
        Token t = lexer.GetToken();
        if(t.token_type == LBRACE){
            tempTType = lexer.UngetToken(t);
            returnVal = parse_public_vars();
            returnVal = parse_private_vars();
            returnVal = parse_stmt_list();
            t = lexer.GetToken();
            if(t.token_type == RBRACE){
                printf("Scope->ID LBRACE public_vars private_vars stmt_list RBRACE\n");
            }
            else{
                syntax_error();
            }
        }
        syntax_error();
    }
    //epsilon does NOT exist in first(parse_scope), therefore no need to check follow set.
    else{
        syntax_error();
    }
return(0);
}

int Parser::parse_public_vars(void) {
    Token t = lexer.GetToken();
    int returnVal;
    //First(public_vars): {PUBLIC, epsilon}
    if(t.token_type == PUBLIC){
        Token t = lexer.GetToken();
        if(t.token_type == COLON){
            returnVal = parse_var_list();
            t = lexer.GetToken();
            if(t.token_type == SEMICOLON){
                printf("public_vars-> PUBLIC COLON var_list SEMICOLON");
            }
            else{
                syntax_error();
            }
        }
        else{
            syntax_error();
        }
    }
    //Epsilon exists in first(public_vars), check Follow(public_vars)
    else if(t.token_type == PRIVATE){
        tempTType = lexer.UngetToken(t);
        returnVal = parse_private_vars();
        std::cout << "\npublic_vars->epsilon -> private_vars\n";
    }
    else{
        syntax_error();
    }
    return(0);
}

int Parser::parse_private_vars(void) {
    Token t = lexer.GetToken();
    int returnVal;
    //First(private_vars): {PRIVATE, epsilon}
    if(t.token_type == PRIVATE){
        t = lexer.GetToken();
        if(t.token_type == COLON){
            tempTType = lexer.UngetToken(t);
            returnVal = parse_var_list();
            t = lexer.GetToken();
            if(t.token_type == SEMICOLON){
                std::cout << "\nprivate_vars->PRIVATE COLON var_list SEMICOLON\n";
            }
            else{
                syntax_error();
            }
        }
        else{
            syntax_error();
        }
    }
    //Epsilon exists in first(private_vars), check if t exists in follow(private_vars): {RBRACE}
    else if(t.token_type == ID){
        tempTType = lexer.UngetToken(t);
        returnVal = parse_stmt_list();
        std::cout <<"\nprivate_vars->epsilon->stmt_list\n";
    }
    else{
        syntax_error();
    }
    return(0);
}

int Parser::parse_stmt_list(void) {
    Token t = lexer.GetToken();
    int returnVal;
    //First(stmt_list): {RBRACE}
    if(t.token_type == ID) {
        tempTType = lexer.UngetToken(t);
        returnVal = parse_stmt();
        t = lexer.GetToken();
        if(t.token_type == ID) {
            tempTType = lexer.UngetToken(t);
            std::cout << "\n stmt_list-> stmt stmt_list\n";
            returnVal = parse_stmt_list();
        }
        else if(t.token_type == RBRACE){
            tempTType = lexer.UngetToken(t);
            std::cout << "\n stmt_list-> stmt\n";
        }
    }
    else
    {
        syntax_error();
    }
    return(0);
}

int Parser::parse_stmt(void) {
    Token t = lexer.GetToken();
    int returnVal;
    //First(stmt): {ID}
    if(t.token_type == ID){
        t = lexer.GetToken();
        if(t.token_type == EQUAL){
             t = lexer.GetToken();
            if(t.token_type == ID){
                 t = lexer.GetToken();
                if(t.token_type == SEMICOLON){
                    std::cout << "\nstmt->ID EQUAL ID SEMICOLON\n";
                }
                else{
                    syntax_error();
                }
            }
            else{
                syntax_error();
            }
        }
        else if (t.token_type == LBRACE){      //stmt->scope
            tempTType = lexer.UngetToken(t);
            returnVal = parse_scope();
            std::cout << "\nstmt->scope\n";
        }
        else{
            syntax_error();
        }
    }
    else{
        syntax_error();
    }
    return(0);
}



int Parser::syntax_error(void) {
    std::cout << "Syntax Error\n";
    exit(1);

}


int main()
{
   int returnValue;
    Parser* parse = new Parser();
    returnValue = parse->parse_Program();

}