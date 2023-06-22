//
// Dakota Burke
//

#include "parser.h"
#include <iostream>
#include <istream>
#include <string>
#include <cctype>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "inputbuf.h"

// some global stuff

struct scopeEval {
    char* scope;
    scopeEval* next;
};

struct symbolTableEntry{
    char* name;
    char* scope;

};

struct symbolTable {
    symbolTableEntry* node;
    symbolTable *prev;
    symbolTable *next;
};

symbolTable* sTable;
char* actualScope;
char* lhs;
char* rhs;
scopeEval* scopeList;


// Start of functions
int Parser::parse_Program(void) {
   Token t =  lexer.GetToken();
    int returnVal;

   //check first set (First(Program): {ID})
   while(t.token_type != END_OF_FILE) {
       if (t.token_type == ID) {
           Token t = lexer.GetToken();
           if(t.token_type == LBRACE){
               tempTType = lexer.UngetToken(t);
               parse_scope();
               std::cout << "\n Parsed program -> epsilon scope \n";
           }
           else if (t.token_type == COMMA){
               tempTType = lexer.UngetToken(t);
               parse_global_vars();
               parse_scope();
               std::cout << "\n Parsed program -> global_vars scope";
           }
           else if(t.token_type == END_OF_FILE){
           return(0);
            }
           else {
               syntax_error();
           }
       }
       //TODO: i believe i would start the symbol table entries here
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
           parse_var_list();
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
            parse_scope();
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
            parse_var_list();
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
            parse_public_vars();
            parse_private_vars();
            parse_stmt_list();
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
            parse_var_list();
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
        parse_private_vars();
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
            parse_var_list();
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
        parse_stmt_list();
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
        parse_stmt();
        t = lexer.GetToken();
        if(t.token_type == ID) {
            tempTType = lexer.UngetToken(t);
            std::cout << "\n stmt_list-> stmt stmt_list\n";
            parse_stmt_list();
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
            parse_scope();
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

void Parser::syntax_error(void) {
    std::cout << "Syntax Error\n";
    exit(1);
}

void createTable(char* lexeme) {
    //case blank symbol table, create new table altogether.
    if (sTable == NULL) {
        symbolTable* newTable = new symbolTable;
        symbolTableEntry* newNode = new symbolTableEntry;
        newNode->name = lexeme;
        newNode->scope = actualScope;
        newTable->node = newNode;
        newTable->next = NULL;
        newTable->prev = NULL;
        sTable = newTable;          //set global Symbol table to current table
    }
        //case non-empty table. update temp table
    else {
        //get copy of current SymbolTable and iterate through to find new scope
        symbolTable *tempTable = sTable;
        while (tempTable->next != NULL) {
            tempTable = tempTable->next;
        }
            symbolTable* newTable = new symbolTable;
            symbolTableEntry* newNode = new symbolTableEntry;
            newNode->name = lexeme;
            newNode->scope = actualScope;
            newTable->node = newNode;
            newTable->prev = tempTable;
            newTable->next = NULL;
            tempTable->next = newTable;
    }
}

void deleteTable(void){
    symbolTable* tmp = sTable;
    if(tmp != NULL){
        while(tmp->next != NULL && strcmp(tmp->node->scope,actualScope) != 0){
            tmp = tmp->next;
        }
        if(strcmp(tmp->node->scope,actualScope) == 0){  //0 = they are equal,
            if(strcmp(actualScope, "::") != 0){
                tmp = tmp->prev;
                tmp->next = NULL;
            }
            else{       //
                tmp = NULL;
            }
        }
    }
}

void createScope(void) {
    if (scopeList == NULL) {
        scopeEval *newScopeObject = new scopeEval;
        newScopeObject->scope = (char *) malloc(sizeof(actualScope));
        memcpy(newScopeObject->scope, actualScope, sizeof(actualScope));
        newScopeObject->next = NULL;
        scopeList = newScopeObject;
    }
    else
    {
        scopeEval* tmpScopeObject = scopeList;
        while(tmpScopeObject->next != NULL){
            tmpScopeObject = tmpScopeObject->next;
        }
        scopeEval* newScopeObject = new scopeEval;
        newScopeObject->scope = (char *) malloc(sizeof(actualScope));
        memcpy(newScopeObject->scope, actualScope, sizeof(actualScope));
        newScopeObject->next = NULL;
        tmpScopeObject->next = newScopeObject;

    }
}

void removeScope(void){
    scopeEval* tmpScopeTable = scopeList;
    if(tmpScopeTable != NULL){
        if(tmpScopeTable->next == NULL){
            tmpScopeTable = tmpScopeTable->next;
        }
        else{   //check for following-following node
            while(tmpScopeTable->next->next != NULL){
                tmpScopeTable = tmpScopeTable->next;
            }
            actualScope = (char *)malloc(sizeof(tmpScopeTable->scope));
            memcpy(actualScope,tmpScopeTable->scope,sizeof(tmpScopeTable->scope));
            tmpScopeTable->next = NULL;
        }
    }

}

void printTable(void){
    symbolTable* tmpTable = sTable;
    while(tmpTable->next != NULL){
        std::cout << tmpTable->node->name;
    }
}
int main()
{

   int returnValue;
    Parser* parse = new Parser();
    parse->parse_Program();

}