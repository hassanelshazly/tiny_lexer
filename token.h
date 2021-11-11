#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <cstdint>
#include <iostream>
#include <set>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

enum TokenType {
  KEYWORD,
  IDENTIFIER,
  STRING_LITERAL,
  INT_LITERAL,
  SYMPOL,
  COMMENT,
  NULL_TOKEN
};

class Token {
public:
  Token() : m_value(), m_type(NULL_TOKEN), m_line(0) {}
  Token(const string &str, TokenType type)
      : m_value(str), m_type(type), m_line(0) {}
  Token(const string &str, TokenType type, uint64_t line)
      : m_value(str), m_type(type), m_line(line) {}

  string value() const { return m_value; }
  TokenType type() const { return m_type; }
  uint64_t line() const { return m_line; }

  friend ostream &operator<<(ostream &os, const Token &token) {
    os << "line " << token.m_line << ": <type: " << type_name[token.m_type]
       << " />"
       << " <value: \"" << token.m_value << "\" />";

    return os;
  }

private:
  inline static const string type_name[] = {
      "keyword", "identifier", "literal_string", "number", "special_sympol",
      "comment", "null"};
  string m_value;
  TokenType m_type;
  uint64_t m_line;
};

#endif