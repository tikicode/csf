#include <iostream>
#include <string>
#include "connection.h"
#include "message.h"
#include "client_util.h"
#include <sstream>

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
 
std::string trim(const std::string &s) {
  return rtrim(ltrim(s));
}

std::pair<std::string, std::string> split_by_colon(const std::string &s) {
  std::stringstream ss(s);
  std::string store; 
  // Note: originally looped over input using a vector 
  // but felt it was overkill for extracting the 
  // second two portions of the string delimited by ':'
  getline(ss, store, ':'); // throw away first line
  getline(ss, store, ':'); 
  std::string user = store;
  getline(ss, store, ':');
  std::string msg = store;
  return { user, msg }; // return pair for ease of assignment
}
