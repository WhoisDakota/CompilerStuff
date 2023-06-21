/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "parser.h"

#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE", "PUBLIC", "PRIVATE",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRACE", "RBRACE","ID", "ERROR" , // TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 2
string keyword[] = { "public", "private"};

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type]
          << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    //tmp.line_no = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    //closes the whitespace
    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

void LexicalAnalyzer::RemoveComments() {
    char c;
    input.GetChar(c);
    if(c == '/'){
        char c2;
        input.GetChar(c2);
        if(c2 == '/'){
            char comments;
            input.GetChar(comments);
            while(comments != '\n' && !input.EndOfInput()){
                input.GetChar(comments);
            }
            if(!input.EndOfInput()){        //out of comments
                input.UngetChar(comments);
            }
        }
    }
}

bool LexicalAnalyzer::IsKeyword(string s)
{   // IF, WHILE, DO, THEN OR PRINT
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s) {
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return (tmp.token_type);
}


Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;

    input.GetChar(c);

    if (isalpha(c) && !input.EndOfInput()) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {  //isalnum == (isalpha(c) || isdigit(c) )
            tmp.lexeme += c;
            input.GetChar(c);
        }
        tmp.token_type = ID;
    }
//check for PUBLIC and PRIVATE
if (IsKeyword(tmp.lexeme)) {
    tmp.token_type= FindKeywordIndex(tmp.lexeme);
}
    return tmp;
}

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
   // tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '{':
            tmp.token_type = LBRACE;
            return tmp;
        case '}':
            tmp.token_type = RBRACE;
            return tmp;
        default:
            if (isalpha(c)) {    //checks for ID as well as PUBLIC, PRIVATE and comments
                input.UngetChar(c);
                return ScanIdOrKeyword();
            }
            else if(c == '/'){
                input.UngetChar(c);
                RemoveComments();
            }
            else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;
            return tmp;
    }
}
/*
int main()
{
    LexicalAnalyzer lexer;
    Token token;

    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }
}
*/