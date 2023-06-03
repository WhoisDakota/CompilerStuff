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
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "IF", "WHILE", "DO", "THEN", "PRINT",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN",
    "NOTEQUAL", "GREATER", "LESS", "LTEQ", "GTEQ",
    "DOT", "NUM", "ID", "ERROR", "REALNUM" , "BASE08NUM", "BASE16NUM"// TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 5
string keyword[] = { "IF", "WHILE", "DO", "THEN", "PRINT" };
char digit8[] = {'0', '1', '2', '3', '4', '5', '6', '7'};
char pDigit8[] = {'1', '2', '3', '4', '5', '6', '7'};

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
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

bool LexicalAnalyzer::isPdigit8(char s) {
    for (int i = 0; i < 7; i++){
        if(s == pDigit8[i]) {
            return true;
        }
    }
    return false;
}

bool LexicalAnalyzer::isDigit8(char s) {
    for (int i = 0; i < 8; i++){
        if(s == digit8[i]) {
            return true;
        }
    }
    return false;
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

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber()
{
    char c;
    char dot;
    char isThisAnX;
    char nextDigit8;
    char octal0;
    char octal8;
    char hex1;
    char hex6;


                                 // NUM
    input.GetChar(c);
    if (isdigit(c)) {
        if (c == '0') {
            tmp.lexeme = "0";
        } else {
            tmp.lexeme = "";
            while (!input.EndOfInput() && isdigit(c)) {
                tmp.lexeme += c;
                input.GetChar(c);
            }
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
        }
        // TODO: You can check for REALNUM, BASE08NUM and BASE16NUM here!

        /* IDEA : we got a NUM, check if the next char in input is a DOT, if so,
            scan for at 1 or more digits, to tack onto the tmp.lexeme. return as REALNUM
         */
        tmp.token_type = NUM;
        tmp.line_no = line_no;
//####################################################
        // REALNUM
        if(tmp.token_type == NUM){
            input.GetChar(dot);
            if(dot == '.') {
                input.GetChar(c);
                if(isdigit(c)) { //case 1: NUM DOT digit digit*
                    tmp.lexeme += '.';
                    while (!input.EndOfInput() && isdigit(c)) {
                        tmp.lexeme += c;
                        input.GetChar(c);
                    }
                    if (!input.EndOfInput()) {
                        input.UngetChar(c);
                    }
                    tmp.token_type = REALNUM;
                    tmp.line_no = line_no;
                    return tmp;
                }
                else if(!isdigit(c) & !input.EndOfInput()){// case 2: NUM DOT
                    input.UngetChar(c);
                    input.UngetChar(dot);
                    //token stays NUM, the DOT is never added to lexeme, returned to inputbuffer
                }
            }
            else if(dot == 'x'){
                input.UngetChar(dot);
            }
            else if(c != '.' && !input.EndOfInput()){
                input.UngetChar(c);
            }
        }
    }
//BASE16NUM
if(tmp.token_type == NUM){
    input.GetChar(isThisAnX);
    if(isThisAnX == 'x'){
        input.GetChar(hex1);
        if(hex1 == '1'){
            input.GetChar(hex6);
            if(hex6 =='6'){
                tmp.token_type = BASE16NUM;
                tmp.lexeme += "x16";
                tmp.line_no = line_no;
                return tmp;
            }
            else{
                input.UngetChar(hex6);
            }
        }
        else if(hex1 == '0'){
            input.GetChar(octal8);
            if(octal8 == '8'){
                tmp.token_type = BASE08NUM;
                tmp.lexeme += "x08";
                tmp.line_no = line_no;
                return tmp;
            }
            else
            {
                input.UngetChar(octal8);
            }
        }
        else{
            input.UngetChar(hex1);
        }
    }
    else{
        input.UngetChar(isThisAnX);
    }
}

else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
    return tmp;
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);


    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {  //isalnum == (isalpha(c) || isdigit(c) )
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        //KEYWORDS ARE IF, THEN, WHILE, DO AND PRINT
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
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
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
        case '.':
            tmp.token_type = DOT;
            return tmp;
        case '+':
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.token_type = MINUS;
            return tmp;
        case '/':
            tmp.token_type = DIV;
            return tmp;
        case '*':
            tmp.token_type = MULT;
            return tmp;
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
        case '[':
            tmp.token_type = LBRAC;
            return tmp;
        case ']':
            tmp.token_type = RBRAC;
            return tmp;
        case '(':
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.token_type = RPAREN;
            return tmp;
        case '<':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = LTEQ;
            } else if (c == '>') {
                tmp.token_type = NOTEQUAL;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = LESS;
            }
            return tmp;
        case '>':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = GTEQ;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = GREATER;
            }
            return tmp;
        default:
            if (isdigit(c)) {
                input.UngetChar(c);
                return ScanNumber();
            } else if (isalpha(c)) {    //checks for alphabet numbers. duh
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}

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
