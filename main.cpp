#include <cstdint>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "lexer.h"
#include "token.h"

using namespace std;

int main(int argc, char **argv) {
  if (argc != 2) {
    cout << "Usage: " << argv[0] << " <file_path>\n";
    return 1;
  }

  string code;
  string path = argv[1];
  ifstream input_file(path);
  input_file.seekg(0, ios::end);
  code.resize(input_file.tellg());
  input_file.seekg(0, ios::beg);
  input_file.read(&code[0], code.size());

  Lexer lexer(code);
  vector<Token> tokens = lexer.get_tokens();

  bool display_comments = true;

  if (lexer.status()) {
    for (const Token &token : tokens)
      if (token.type() == TokenType::COMMENT && !display_comments)
        continue;
      else
        cout << token << "\n";
  } else {
    cout << "Can't parse: too many errors\n";
    for (const string &err : lexer.errors())
      cout << err << "\n";
  }
}