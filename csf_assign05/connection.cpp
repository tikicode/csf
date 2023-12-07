<<<<<<< HEAD
=======
#include <sstream>
#include <iostream>
#include <cctype>
#include <cassert>
#include "csapp.h"
#include "message.h"
>>>>>>> refs/remotes/origin/main
#include "connection.h"

#include <cassert>
#include <cctype>
#include <sstream>

#include "csapp.h"
#include "message.h"

Connection::Connection() : m_fd(-1), m_last_result(SUCCESS) {}

Connection::Connection(int fd) : m_fd(fd), m_last_result(SUCCESS) {
  rio_readinitb(&m_fdbuf, fd);
}

void Connection::connect(const std::string &hostname, int port) {
  m_fd = open_clientfd(hostname.c_str(), std::to_string(port).c_str());
<<<<<<< HEAD
  if (m_fd <= -1)
    fprintf(stderr, "Error: Connection failed");
  else
=======
  if (m_fd <= -1) 
    std::cerr << "Error: Connection failed\n";
  else 
>>>>>>> refs/remotes/origin/main
    rio_readinitb(&m_fdbuf, m_fd);
}

Connection::~Connection() { close(); }

bool Connection::is_open() const { return m_fd > -1; }

void Connection::close() {
  if (is_open()) {
    Close(m_fd);
    m_fd = -1;  // change status to closed
  }
}

bool Connection::send(const Message &msg) {
  std::string message = msg.tag + ":" + msg.data + "\n";
  ssize_t send_res = rio_writen(m_fd, message.c_str(), message.size());

  if (send_res >= 0) {
    m_last_result = SUCCESS;
    return true;
  }

  m_last_result = EOF_OR_ERROR;
  return false;
}

bool Connection::receive(Message &msg) {
  char buffer[256];
  ssize_t receive_res = rio_readlineb(&m_fdbuf, buffer, 256);

  if (receive_res < 0) {
    m_last_result = INVALID_MSG;
    return false;
  }

  std::string message(buffer);
  int colon_pos = message.find(':');
  msg.tag = message.substr(0, colon_pos);
  msg.data = message.substr(colon_pos + 1);

  m_last_result = SUCCESS;
  return true;
}
