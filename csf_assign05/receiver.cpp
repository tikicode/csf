#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include "client_util.h"
#include "connection.h"
#include "csapp.h"
#include "message.h"

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr
        << "Usage: ./receiver [server_address] [port] [username] [room]\n";
    return 1;
  }

  std::string server_hostname = argv[1];
  int server_port = std::stoi(argv[2]);
  std::string username = argv[3];
  std::string room_name = argv[4];

  /* Connect to server */
  Connection conn;
  conn.connect(server_hostname, server_port);
  if (!conn.is_open()) return 1;

  /* Send rlogin and join messages and get responses */
  bool login_status = conn.send(Message(TAG_RLOGIN, username));
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

  bool join_status = conn.send(Message(TAG_JOIN, room_name));
  if (!join_status) {
    std::cerr << "Error: Failed send join message\n";
    conn.close();
    return 1;
  }

  Message join_msg = Message();
  conn.receive(join_msg);
  if (join_msg.tag == TAG_ERR) {
    std::cerr << join_msg.data;
    conn.close();
    return 1;
  }

  /* Message receiving loop */
  while (true) {
    Message msg = Message();
    bool msg_status = conn.receive(msg);
    if (!msg_status)
      break;
    if (msg.tag == TAG_DELIVERY) {
      std::string user, message;
      std::tie(user, message) = split_by_colon(msg.data); // perform assignment on one line
      std::cout << user << ": " << message;
    }
  }

  conn.close();
  return 0;
}
