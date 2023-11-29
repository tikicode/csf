#include <sstream>
#include <cctype>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "connection.h"

Connection::Connection()
  : m_fd(-1)
  , m_last_result(SUCCESS) {
}

Connection::Connection(int fd)
  : m_fd(fd)
  , m_last_result(SUCCESS) {
  rio_readinitb(&m_fdbuf, fd);
}

void Connection::connect(const std::string &hostname, int port) {
  m_fd = open_clientfd(hostname.c_str(), std::to_string(port).c_str());
  if (m_fd <= -1) fprintf(stderr, "Error: Connection failed");
  else rio_readinitb(&m_fdbuf, m_fd);
}

Connection::~Connection() {
  close();
}

bool Connection::is_open() const {
  return m_fd > -1;
}

void Connection::close() {
  if (is_open()) {
    Close(m_fd);
    m_fd = -1; // change status to closed
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

  // TODO: receive a message, storing its tag and data in msg
  // return true if successful, false if not
  // make sure that m_last_result is set appropriately
}
