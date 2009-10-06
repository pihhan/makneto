
#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <list>

/** @brief Class for one word in input string. */
class Token
{
    public:
    Token(const std::string &text, size_t start = 0);

    std::string value();
    int         intValue();
    bool        isNumeric();
    size_t      start();
    size_t      end();

    private:
    std::string m_text;
    size_t      m_start;
};

/** @brief Class for parsing input string into words. */
class Parser
{
    public:

    typedef std::list<Token>    TokenList;
    typedef enum {
        S_BEGIN,
        S_WORD,
        S_NUMBER,
        S_STRING
    } ParseStates;

    Parser(const std::string &text);

    Token   nextToken();
    bool    isQuote(char c);
    void    reset();
    bool    atEnd() { return (m_it == m_tokens.end()); }

    TokenList   tokens();

    protected:
    void addToken(const std::string &text, size_t position);

    private:
    std::string m_text;
    TokenList   m_tokens;
    size_t      m_position;
    ParseStates m_state;
    std::string m_error;
    TokenList::iterator   m_it;

};

#endif

