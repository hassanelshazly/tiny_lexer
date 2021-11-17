#ifndef _LEXER_H_
#define _LEXER_H_

#include <cctype>
#include <cstdint>
#include <exception>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "token.h"

using namespace std;

class Lexer {
public:
  Lexer(const string &text) : m_text(text), index(0), line(1) {}
  Lexer(string &&text) : m_text(move(text)), index(0), line(1) {}

  vector<Token> get_tokens() {
    vector<Token> res;
    int x = 0;
    while (!eof()) {
      Token token = next_token();
      if (token.type() != TokenType::UNKNOWN)
        res.push_back(token);
    }
    return res;
  }

  vector<string> errors() const { return m_errors; }
  bool status() const { return m_status; }

  Token next_token() {
    ignore_white_space();
    char c = peek();
    if (eof())
      return Token();

    try {
      if (c == '"') {
        return Token(match_literal_string(), STRING, line);
      } else if (c == '{') {
        return Token(match_comment(), COMMENT, line);
      } else if (c == ':') {
        return Token(match_assign(), ASSIGN, line);
      } else if (pair<bool, TokenType> res = is_symbol(c); res.first) {
        return Token(string(1, next()), res.second, line);
      } else if (isdigit(c)) {
        return Token(match_literal_int(), NUM, line);
      } else if (isalpha(c)) {
        string word = match_identifier();
        pair<bool, TokenType> res = is_keyword(to_upper(word));
        return Token(word, res.first ? res.second : ID, line);
      } else {
        log_error("Unexpected character: '" + string(1, next_line()) + "'");
        return Token();
      }
    } catch (...) {
      return Token();
    }
  }

private:
  string match_literal_string() {
    string res = "";
    if (peek() == '"')
      next();

    while (!eof() && peek() != '"' && peek() != '\n') {
      res += next();
    }

    if (peek() == '\n' || eof()) {
      next_line();
      log_error("Unterminated string");
      throw UnknownToken();
    }

    next();
    return res;
  }

  string match_literal_int() {
    string res = "";
    while (isdigit(peek())) {
      res += next();
    }
    return res;
  }

  string match_identifier() {
    string res = "";
    while (isalnum(peek()))
      res += next();
    return res;
  }

  string match_comment() {
    string res = "";
    if (peek() == '{')
      next();

    while (!eof() && peek() != '}')
      res += next();

    if (eof()) {
      next_line();
      log_error("Unterminated comment");
      throw UnknownToken();
    }

    next();
    return res;
  }

  string match_assign() {
    if (peek() == ':' && next() && peek() == '=') {
      next();
      return ":=";
    }

    log_error("Unexpected character: '" + string(1, next_line()) + "'");
    throw UnknownToken();
  }

  void ignore_white_space() {
    while (peek() == ' ' || peek() == '\n' || peek() == '\t') {
      if (next() == '\n')
        line++;
    }
  }

  char next() {
    if (eof())
      return '\0';
    return m_text[index++];
  }

  char peek() const {
    if (eof())
      return '\0';
    return m_text[index];
  }

  char next_line() {
    if (eof())
      return '\0';

    char ch = m_text[index];

    while (m_text[++index] != '\n')
      ;
    line++;

    return ch;
  }

  string to_upper(string str) {
    for (auto &c : str) {
      c = toupper(c);
    }
    return str;
  }

  pair<bool, TokenType> is_keyword(string word) const {
    return keywords.find(word) != keywords.end()
               ? make_pair(true, keywords.at(word))
               : make_pair(false, TokenType::UNKNOWN);
  }

  pair<bool, TokenType> is_symbol(char ch) const {
    return symbols.find(ch) != symbols.end()
               ? make_pair(true, symbols.at(ch))
               : make_pair(false, TokenType::UNKNOWN);
  }

  bool eof() const { return index >= m_text.length(); }

  void log_error(string message) {
    ostringstream os;
    m_status = false;

    os << "Error on line " << line - 1 << ": " << message;
    m_errors.push_back(os.str());
  }

private:
  inline static const map<string, TokenType> keywords = {
      {"IF", IF},       {"THEN", THEN},     {"ELSE", ELSE},   {"READ", READ},
      {"WRITE", WRITE}, {"REPEAT", REPEAT}, {"UNTIL", UNTIL}, {"END", END}};

  inline static const map<char, TokenType> symbols = {
      {'+', PLUS},   {'-', MINUS},  {'/', DIV}, {'*', MUL},       {'=', EQ},
      {'(', LPAREN}, {')', RPAREN}, {'<', LT},  {';', SEMICOLON}, {',', COMMA}};

private:
  string m_text;
  uint64_t index;
  uint64_t line;
  vector<string> m_errors;
  bool m_status = true;

  class UnknownToken : public exception {};
};

#endif