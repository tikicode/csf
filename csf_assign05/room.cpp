#include "room.h"

#include <pthread.h>

#include "guard.h"
#include "message.h"
#include "message_queue.h"
#include "user.h"

Room::Room(const std::string &room_name) : room_name(room_name) {
  pthread_mutex_init(&lock, nullptr);
}

Room::~Room() { pthread_mutex_destroy(&lock); }

void Room::add_member(User *user) {
  Guard gd(lock);
  members.insert(user);
}

void Room::remove_member(User *user) {
  Guard gd(lock);
  members.erase(user);
}

void Room::broadcast_message(const std::string &sender_username,
                             const std::string &message_text) {
  Guard gd(lock);
  std::string msg =
      get_room_name() + ":" + sender_username + ":" + message_text;
  for (auto i : members) {
    i->mqueue.enqueue(
        new Message(TAG_DELIVERY, msg));  // add message to user msg queue
  }
}
