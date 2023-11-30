#include "client_util.h"

#include <iostream>
#include <sstream>
#include <string>

#include "connection.h"
#include "message.h"

// string trim functions shamelessly stolen from
// https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s) {
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s) {
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s) { return rtrim(ltrim(s)); }

std::pair<std::string, std::string> split_by_colon(const std::string &s) {
  std::stringstream ss(s);
  std::string store;
  std::vector<std::string> res_buf;
  while (getline(ss, store, ':')) res_buf.push_back(store);
  return {res_buf[0], res_buf[1]};
}
