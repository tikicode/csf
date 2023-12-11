#include "server.h"

#include <pthread.h>

#include <cassert>
#include <cctype>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <vector>

#include "connection.h"
#include "csapp.h"
#include "guard.h"
#include "message.h"
#include "room.h"
#include "user.h"

////////////////////////////////////////////////////////////////////////
// Server implementation data types
////////////////////////////////////////////////////////////////////////
struct communication_data {
  Server *server;
  Connection *connection;
  ~communication_data() { delete connection; }
};

////////////////////////////////////////////////////////////////////////
// Client thread functions
////////////////////////////////////////////////////////////////////////
void chat_with_sender(communication_data *comm, std::string &username);
void chat_with_receiver(communication_data *comm, User *user);

namespace {
  void *worker(void *arg) {
    pthread_detach(pthread_self());
    communication_data *comm = (communication_data *)arg;
    Connection *conn = comm->connection;

    Message message = Message();
    bool conn_status = conn->receive(message);

    if (message.tag == TAG_RLOGIN || message.tag == TAG_SLOGIN) {
      conn->send(Message(TAG_OK, "Successfully logged in"));
    } else if (!conn_status) {
      conn->send(Message(TAG_ERR, "Failure to login"));
      return nullptr;
    } else {
      conn->send(Message(TAG_ERR, "Login before trying other actions"));
      return nullptr;
    }

    if (message.tag == TAG_SLOGIN) {
      User *user = new User(message.data.substr(0, message.data.size() - 1));
      chat_with_receiver(comm, user);
    } else {
      std::string username = message.data.substr(0, message.data.size() - 1);
      chat_with_sender(comm, username);
    }

    return nullptr;
  }
}  // namespace

void chat_with_sender(communication_data *comm, std::string &username) {
  Connection *conn = comm->connection;
  Server *server = comm->server;
  Room *room = nullptr;

  while (true) {
    Message message;
    if (!conn->receive(message)) {
      if (conn->get_last_result() == Connection::EOF_OR_ERROR ||
          conn->get_last_result() == Connection::INVALID_MSG) {
        conn->send(Message(TAG_ERR, "Invalid message form or type"));
      } else {
        conn->send(Message(TAG_ERR, "Error receiving message"));
      }
    }

    // error tag handling
    if (message.tag == TAG_ERR) {
      std::cerr << message.data;
      return;
    }
    // quitting the server handling
    if (message.tag == TAG_QUIT) {
      conn->send(Message(TAG_OK, "Quit connection"));
      return;
    }

    // case where sender is not in a room and does an action other than joining
    // a room
    if (message.tag != TAG_JOIN && !room) {
      conn->send(
          Message(TAG_ERR, "Join a room before attempting other actions"));
      continue;
    }

    // joining a room handling
    if (message.tag == TAG_JOIN) {
      room = server->find_or_create_room(message.data);
      conn->send(Message(TAG_OK, "Joined Room"));
      continue;
    }
    // leaving a room handling
    else if (message.tag == TAG_LEAVE) {
      if (!room) conn->send(Message(TAG_ERR, "Cannot leave when not in a room"));
      else {
        conn->send(Message(TAG_OK, "Left Room"));
        continue;
      }
    }
    // sending to all all receivers in room handling
    else if (message.tag == TAG_SENDALL) {
      room->broadcast_message(username,
                              message.data.substr(0, message.data.size() - 1));
      conn->send(Message(TAG_OK, "Message sent to all in the room"));
      continue;
    }
    // all other issues
    else conn->send(Message(TAG_ERR, "Problem with parsing message"));

    // if message is too large handling
    if (message.data.size() > Message::MAX_LEN) {
      conn->send(Message(TAG_ERR, "Message exceeds max length"));
      continue;
    }
  }
}

void chat_with_receiver(communication_data *comm, User *user) {
  Message msg;
  Connection *conn = comm->connection;
  Server *serv = comm->server;

  // Initial message reception and error handling
  if (!conn->receive(msg)) {
    Connection::Result last_result = conn->get_last_result();
    if (last_result == Connection::INVALID_MSG ||
        last_result == Connection::EOF_OR_ERROR) {
      conn->send(Message(TAG_ERR, "Invalid message form or type"));
      return;
    }
    conn->send(Message(TAG_ERR, "Error receiving message")); // should not shut down
  }

  Room *room = nullptr;
  while (!room) {
    if (msg.tag == TAG_JOIN) {
      room = serv->find_or_create_room(msg.data);
      room->add_member(user);
      conn->send(Message(TAG_OK, "Joined room"));
    } else {
      conn->send(Message(TAG_ERR, "Must first join a room"));
    }
  }

  while (true) {
    Message *receive = user->mqueue.dequeue();
    if (receive) {
      if (!conn->send(*receive)) {
        delete receive;
        break;
      }
      delete receive;
    }
  }
  room->remove_member(user);
}

////////////////////////////////////////////////////////////////////////
// Server member function implementation
////////////////////////////////////////////////////////////////////////

Server::Server(int port) : m_port(port), m_ssock(-1) {
  pthread_mutex_init(&m_lock, nullptr);
}

Server::~Server() { pthread_mutex_destroy(&m_lock); }

bool Server::listen() {
  m_ssock = open_listenfd(std::to_string(m_port).c_str());
  bool isSocketOpen = (m_ssock != -1);

  if (!isSocketOpen) {
    std::cerr << "Error: failed to open server socket\n";
  }

  return isSocketOpen;
}

void Server::handle_client_requests() {
  while (true) {
    int fd = accept(m_ssock, nullptr, nullptr);

    if (fd == -1) {
      std::cerr << "Error: could not accept client connection\n";
      continue;
    }

    communication_data *comm = new communication_data();
    comm->connection = new Connection(fd);
    comm->server = this;

    pthread_t thread;
    if (pthread_create(&thread, nullptr, worker, comm) != 0) {
      std::cerr << "Error: failed to create thread\n";
      return;
    }
  }
}

Room *Server::find_or_create_room(const std::string &room_name) {
  Guard guard(m_lock);
  if (m_rooms[room_name]) {
    return m_rooms[room_name];
  } else {
    std::string adj_room_name = room_name.substr(0, room_name.size() - 1);
    Room *room = new Room(adj_room_name);
    m_rooms[room_name] = room;
    return room;
  }
}
