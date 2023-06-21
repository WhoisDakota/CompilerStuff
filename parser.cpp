//
// Created by dakota on 6/17/23.
//

#include "parser.h"
#include <iostream>
#include <istream>
#include <string>

#include "lexer.h"
#include "inputbuf.h"

/*
 * need one function for each non-terminal of grammar to handle
 * parsing of such non-terminal
 * ie:
 * parse_X()
 * */

Parser::Parser(){
    //tmp.lexeme = "";
    //tmp.token_type = ERROR;
}

void Parser::parse_Program(LexicalAnalyzer lexer) {
   Token t =  lexer.GetToken();
   //check first set (First(Program): {ID})
   if(t.token_type == ID){
       lexer.UngetToken(t);
       parse_global_vars(lexer);
       parse_scope(lexer);
       printf("Program->global_vars.scope\n");
   }
   //no need to check follow set as while global_vars has epsilon in first set, scope does not.
   else{
       syntax_error();

   }
}

void Parser::parse_global_vars(LexicalAnalyzer lexer) {
    Token t = lexer.GetToken();
    //check first set (First(global_vars): {ID, epsilon} )
    if(t.token_type == ID){
        lexer.UngetToken(t);
        parse_var_list(lexer);
        t = lexer.GetToken();
        if(t.token_type == SEMICOLON){
            printf("global_vars->var_list.SEMICOLON\n");
        }
        else{
            printf("Syntax Error\n");
        }
    }
    // epsilon exists in first set, so check follow set see if t exists in follow(global_vars): {$, RBRACE}
    else if(t.token_type == RBRACE){
        lexer.UngetToken(t);
        printf("global_vars->epsilon\n");
    }
    else if(t.token_type == END_OF_FILE){
        printf("global_vars->EOF\n");
    }
    else
        syntax_error();
        //printf("Syntax Error");
}

void Parser::parse_var_list(LexicalAnalyzer lexer) {
    Token t = lexer.GetToken();
    //check first set (First(var_list): {ID} )

    if(t.token_type == ID){
        Token tmpToken = lexer.GetToken();
        if (tmpToken.token_type == COMMA){
            parse_var_list(lexer);
            printf("var_list->ID COMMA var_list\n");
        }
        else if (lexer.IsKeyword(tmpToken.lexeme) && tmpToken.token_type != ID){
            lexer.UngetToken(tmpToken);
            printf("var_list->ID\n");
        }
        else{
            syntax_error();
        }
    }
    //epsilon does NOT exist in first(var_list), therefore no need to check follow set.
    else{
        syntax_error();
        //printf("Syntax Error");
    }

}

void Parser::parse_scope(LexicalAnalyzer lexer) {
    Token t = lexer.GetToken();
    //First(scope): {ID}
    if(t.token_type == ID){
        Token tmp1 = lexer.GetToken();
        if(tmp1.token_type == LBRACE){
            lexer.UngetToken(tmp1);
            parse_public_vars(lexer);
            parse_private_vars(lexer);
            parse_stmt_list(lexer);
            tmp1 = lexer.GetToken();
            if(tmp1.token_type == RBRACE){
                printf("Scope->ID LBRACE public_vars private_vars stmt_list RBRACE\n");
            }
            else{
                //lexer.UngetToken(tmp1);
                syntax_error();
            }
        }
        syntax_error();
    }
    //epsilon does NOT exist in first(parse_scope), therefore no need to check follow set.
    else{
        syntax_error();
    }

}

void Parser::parse_public_vars(LexicalAnalyzer lexer) {
    Token t = lexer.GetToken();
    //First(public_vars): {PUBLIC, epsilon}
    if(t.token_type == PUBLIC){
        Token tmp = lexer.GetToken();
        if(tmp.token_type == COLON){
            parse_var_list(lexer);
            Token tmp2 = lexer.GetToken();
            if(tmp2.token_type == SEMICOLON){
                printf("public_vars-> PUBLIC COLON var_list SEMICOLON");
            }
            else{
                //lexer.UngetToken(tmp2);
                syntax_error();
            }
        }
        else{
            //lexer.UngetToken(tmp);
            syntax_error();
        }
    }
    //Epsilon exists in first(public_vars), check Follow(public_vars)
    else if(t.token_type == RBRACE){
        lexer.UngetToken(t);
        printf("public_vars->epsilon\n");
    }
    else{
        syntax_error();
    }
}

void Parser::parse_private_vars(LexicalAnalyzer lexer) {
    Token t = lexer.GetToken();
    //First(private_vars): {PRIVATE, epsilon}
    if(t.token_type == PRIVATE){
        t = lexer.GetToken();
        if(t.token_type == COLON){
            lexer.UngetToken(t);
            parse_var_list(lexer);
            t = lexer.GetToken();
            if(t.token_type == SEMICOLON){
                printf("private_vars->PRIVATE COLON var_list SEMICOLON");
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
    else if(t.token_type == RBRACE){
        lexer.UngetToken(t);
        printf("private_vars->epsilon");
    }
    else{
        syntax_error();
    }

}

void Parser::parse_stmt_list(LexicalAnalyzer lexer) {
    Token t = lexer.GetToken();
    //First(stmt_list): {RBRACE}
    if(t.token_type == ID){
        lexer.UngetToken(t);
        parse_stmt(lexer);
        t = lexer.GetToken();
        //check first rule stmt_list -> stmt
        if(t.token_type == ID){
            lexer.UngetToken(t);
            parse_stmt_list(lexer);
            printf("stmt_list->stmt stmt_list");
        }
        else if(t.token_type != ID){
            printf("stmt_list->stmt");
        }
    }
    //epsilon NOT in first set, so no follow set to check

    else
    {
        syntax_error();
    }
}

void Parser::parse_stmt(LexicalAnalyzer lexer) {
    Token t = lexer.GetToken();
    //First(stmt): {ID}
    if(t.token_type == ID){
        Token t2 = lexer.GetToken();
        if(t2.token_type == EQUAL){
            Token t3 = lexer.GetToken();
            if(t3.token_type == ID){
                Token t4 = lexer.GetToken();
                if(t4.token_type == SEMICOLON){
                    printf("stmt->ID EQUAL ID SEMICOLON");
                }
                else{
                    lexer.UngetToken(t4);
                    syntax_error();
                }
            }
            else{
                lexer.UngetToken(t3);
                syntax_error();
            }
        }
        else if (t2.token_type == ID){      //stmt->scope
            lexer.UngetToken(t2);
            parse_scope(lexer);
            printf("stmt->scope");
        }
        else{
            lexer.UngetToken(t2);
            syntax_error();
        }
    }
    else{
        syntax_error();
    }
}



void Parser::syntax_error() {
    std::cout << "Syntax Error\n";
    exit(1);

}


int main()
{
    Parser parser;
    LexicalAnalyzer lexer;
    Token t = lexer.GetToken();



    while(t.token_type != END_OF_FILE){
        lexer.UngetToken(t);
        parser.parse_Program(lexer);
    }//TODO might encounter EOF issues here without checking for token. Maybe
}