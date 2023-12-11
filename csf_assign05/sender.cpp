#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>

#include "client_util.h"
#include "connection.h"
#include "csapp.h"
#include "message.h"

int main(int argc, char **argv) {
  if (argc != 4) {
    std::cerr << "Usage: ./sender [server_address] [port] [username]\n";
    return 1;
  }

  std::string server_hostname;
  int server_port;
  std::string username;

  server_hostname = argv[1];
  server_port = std::stoi(argv[2]);
  username = argv[3];

  /* Connect to server */
  Connection conn;
  conn.connect(server_hostname, server_port);
  if (!conn.is_open()) return 1;

  /* Send rlogin message and get response */
  bool login_status = conn.send(Message(TAG_SLOGIN, username));
  if (!login_status) {
    std::cerr << "Error: Failed to send login message\n";
    conn.close();
    return 1;
  }

  Message login_msg = Message();
  conn.receive(login_msg);
  if (login_msg.tag == TAG_ERR) {
    std::cerr << login_msg.data;
    conn.close();
    return 1;
  }

  /* Loop reading user input */
  while (true) {
    Message msg = Message();
    std::string input;
    getline(std::cin, input);

    // quit case
    if (input == "/quit") {
      msg.tag = TAG_QUIT;
      msg.data = "cheers";
      bool quit_status = conn.send(msg);
      if (!quit_status) {
        std::cerr << "Error: Failed to send quit message\n";
        conn.close();
        return 1;
      }

      Message quit_msg = Message();
      conn.receive(quit_msg);
      if (quit_msg.tag == TAG_ERR) {
        std::cerr << login_msg.data;
        conn.close();
        return 1;
      }
      break;
    }

    // join, leave, and send all cases
    if (input.substr(0, 5) == "/join") {
      msg.tag = TAG_JOIN;
      msg.data = input.substr(6);
    } else if (input == "/leave") {
      msg.tag = TAG_LEAVE;
      msg.data = "yeet";
    } else {
      msg.tag = TAG_SENDALL;
      msg.data = input;
    }

    bool msg_status = conn.send(msg);
    if (!msg_status) {
      std::cerr << "Error: Failed to send sender message\n";
      conn.close();
      return 1;
    }

    Message sent_msg = Message();
    conn.receive(sent_msg);
    if (sent_msg.tag == TAG_ERR) {
      std::cerr << sent_msg.data;
    }
  }

  conn.close();
  return 0;
}
