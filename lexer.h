#ifndef _LEXER_H_
#define _LEXER_H_

#include <cctype>
#include <cstdint>
#include <exception>
#include <iostream>
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
      if (token.type() != TokenType::NULL_TOKEN)
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
        return Token(match_literal_string(), STRING_LITERAL, line);
      } else if (c == '{') {
        return Token(match_comment(), COMMENT, line);
      } else if (c == ':') {
        return Token(match_assign(), STRING_LITERAL, line);
      } else if (is_symbol(c)) {
        return Token(string(1, next()), SYMPOL, line);
      } else if (isdigit(c)) {
        return Token(match_literal_int(), INT_LITERAL, line);
      } else if (isalpha(c)) {
        string word = match_identifier();
        if (is_reserved(word))
          return Token(word, KEYWORD, line);
        else
          return Token(word, IDENTIFIER, line);
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
      throw UndefindToken();
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
      throw UndefindToken();
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
    throw UndefindToken();
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

  bool eof() const { return index >= m_text.length(); }
  bool is_reserved(string word) const { return reserved.count(word); }
  bool is_symbol(char ch) const { return symbols.count(ch); }

  void log_error(string message) {
    ostringstream os;
    m_status = false;

    os << "Error on line " << line - 1 << ": " << message;
    m_errors.push_back(os.str());
  }

private:
  inline static const set<string> reserved = {"WRITE",   "READ",  "IF",  "ELSE",
                                              "RETURN ", "BEGIN", "END", "MAIN",
                                              "STRING ", "INT",   "REAL"};

  inline static const set<char> symbols = {'+', '-', '/', '*', '=', '(',
                                           ')', '<', '>', ';', ','};

private:
  string m_text;
  uint64_t index;
  uint64_t line;
  vector<string> m_errors;
  bool m_status = true;

  class UndefindToken : public exception {};
};

#endif