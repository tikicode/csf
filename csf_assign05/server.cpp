#include <pthread.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <set>
#include <vector>
#include <cctype>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "connection.h"
#include "user.h"
#include "room.h"
#include "guard.h"
#include "server.h"

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////
struct communication_data {
  Server *server;
  Connection *connection;
  ~communication_data() {delete connection;}
};

// TODO: add any additional data types that might be helpful
//       for implementing the Server member functions

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////
void handle_client_comm_sender (communication_data *comm, std::string &username);
void handle_client_comm_receiver (communication_data *comm, User *user);

namespace {

  void *worker(void *arg) {
    pthread_detach(pthread_self());
    communication_data *comm = (communication_data*)arg;
    Connection *conn = comm->connection;
    Message message = Message();
    bool conn_status = conn->receive(message);
    if (message.tag == TAG_RLOGIN || message.tag == TAG_SLOGIN) {
      conn->send(Message(TAG_OK, "Successfully logged in"));
    } else if (!conn_status) {
    conn->send(Message(TAG_ERR, "Failure to login"));
    return NULL;
    } else {
      conn->send(Message(TAG_ERR, "Login first"));
    }
    if (message.tag == TAG_RLOGIN) {
      User *user = new User(message.data.substr(0, message.data.size()-1));
      handle_client_comm_receiver(comm, user);
    }
    else {
      std::string username = message.data.substr(0, message.data.size()-1);
      handle_client_comm_sender(comm, username);
    }

    return nullptr;
  }
}

void handle_client_comm_sender (communication_data *comm, std::string &username) {
  Connection *conn = comm->connection;
  Server *server = comm->server;
  Room *room = nullptr;

  while (true) {
    Message message;
    if (!conn->receive(message)) {
      if (conn->get_last_result() == Connection::EOF_OR_ERROR || conn->get_last_result() == Connection::INVALID_MSG) {
        conn->send(Message(TAG_ERR, "Invalid message form or type"));
      } else {
        conn->send(Message(TAG_ERR, "Error receiving message"));
      }
    }
    if (message.tag == TAG_ERR) {
      std::cerr << message.data;
      return;
    }
    if (message.tag == TAG_JOIN){
      room = server->find_or_create_room(message.data);
      conn->send(Message(TAG_OK, "Joined Room"));
      continue;
    }
    if (message.tag != TAG_JOIN && !room) {
      conn->send(Message(TAG_ERR, "Join a room before attempting other actions"));
      continue;
    }
    if (message.tag == TAG_LEAVE) {
      //idt I need to check if !room because of prev statement
      room = nullptr;
      conn->send(Message(TAG_OK, "Left Room"));
      continue;
    }
    if (message.tag == TAG_SENDALL) {
      room->broadcast_message(username, message.data);
      conn->send(Message(TAG_OK, "Message sent to all in the room"));
      continue;
    }
    conn->send(Message(TAG_ERR, "Problem with parsing message"));
  }
}
void handle_client_comm_receiver (communication_data *comm, User *user) {
  
}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port)
  : m_port(port)
  , m_ssock(-1) {
  pthread_mutex_init(&m_lock, NULL);
}

Server::~Server() {
  pthread_mutex_destroy(&m_lock);
}

bool Server::listen() {
  if (open_listenfd(std::to_string(m_port).c_str()) == 0) return true;
  std::cerr << "Error: failed to create server socket\n";
  return false;
}

void Server::handle_client_requests() {
  // TODO: infinite loop calling accept or Accept, starting a new
  //       pthread for each connected client
  // while (true) {
  //   accept()
  // }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  Guard guard(m_lock);
  if (m_rooms[room_name]) {
    return m_rooms[room_name];
  } else {
    Room *room = new Room(room_name);
    m_rooms[room_name] = room;
    return room;
  }
}
