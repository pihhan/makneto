
#include <cctype>
#include <iostream>
#include <cstdlib>

#include "parser.h"

using namespace std;

/**
@brief Parse input text to words.
*/
Parser::Parser(const std::string &text)
    : m_state(S_BEGIN)
{
    std::string subword;
    char quote;
    size_t  start = 0;
    m_it = m_tokens.begin();

    m_text = text;

    for (int i=0; i<text.size(); ++i) {
        char c = text.at(i);
        switch (m_state) {
            case S_BEGIN:
                if (isalpha(c)) {
                    subword.erase();
                    subword.append(1, c);
                    m_state = S_WORD;
                    start = i;
                } else if (isdigit(c)) {
                    subword.erase();
                    subword.append(1, c);
                    m_state = S_NUMBER;
                    start = i;
                } else if (isspace(c)) {
                    // skip
                } else if (ispunct(c)) {
                    // interpunkcni znakenka povazuju za samostatny token
                    if (isQuote(c)) {
                        quote = c;
                        m_state = S_STRING;
                    } else {
                        subword.erase();
                        subword.append(1, c);
                        addToken(subword, i);
                        subword.erase();
                        m_state = S_BEGIN;
                    }
                } else {
                    std::cerr << "Neocekavany znak \'" << c << "\'" << std::endl;
                }
                break; // BEGIN
            
            case S_WORD:
                if (isalnum(c)) {
                    subword.append(1, c);
                } else if (isspace(c)) {
                    addToken(subword, start);
                    subword.erase();
                } else if (ispunct(c)) {
                    addToken(subword, start);
                    subword.erase();
                    if (isQuote(c)) {
                        m_state = S_STRING;
                        quote = c;
                    } else {
                        subword.append(1, c);
                        addToken(subword, i); 
                        m_state = S_BEGIN;
                    }
                } else {
                    cerr << "Neocekavany znak ve slove \'" << c << "\'" << endl;
                }
                break;

            case S_NUMBER:
                if (isdigit(c)) {
                    subword.append(1, c);
                } else if (isalpha(c)) {
                    // neni cislo
                    subword.append(1, c);
                } else if (ispunct(c)) {
                    addToken(subword, start);
                    subword.erase();
                    if (isQuote(c)) {
                        quote = c;
                        m_state = S_STRING;
                    } else {
                        subword.append(1, c);
                        addToken(subword, i);
                        subword.erase();
                    }
                } else if (isspace(c)) {
                    addToken(subword, i);
                    subword.erase();
                    m_state = S_BEGIN; 
                }
                break;

            case S_STRING:
                if (c != quote) {
                    subword.append(1, c);
                } else {
                    addToken(subword, start);
                    subword.erase();
                    m_state = S_BEGIN;
                }
                break;
        }
    }

    if (subword.size() > 0) {
        if (m_state == S_WORD || m_state == S_NUMBER) {
            addToken(subword, start);
        }
    }
    if (m_state == S_STRING) {
        m_error = "Unterminated quote, started at "+ start;
    }

    reset();
}

void Parser::addToken(const std::string &text, size_t start)
{
    m_tokens.push_back(Token(text, start));
}

bool Parser::isQuote(char c)
{
    return (c=='\"' || c=='\'');
}

void Parser::reset()
{
    m_it = m_tokens.begin();
}

Token Parser::nextToken()
{
    Token t = *m_it;
    m_it++;
    return t;
}

Parser::TokenList Parser::tokens()
{
    return m_tokens;
}




Token::Token(const std::string &text, size_t start)
    : m_text(text), m_start(start)
{
}

size_t Token::start()
{
    return m_start;
}

size_t Token::end()
{
    return m_start + m_text.size();
}

bool Token::isNumeric()
{
    for (size_t i=0; i<m_text.size(); ++i) {
        if (!isdigit(m_text.at(i))) 
            return false;
    }
    return true;
}

int Token::intValue()
{
    return atoi(m_text.c_str());
}

std::string Token::value()
{
    return m_text;
}

