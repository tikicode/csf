#include <cassert>
#include <ctime>
#include <semaphore.h>
#include <pthread.h>
#include "message_queue.h"
#include "message.h"
#include "csapp.h"
#include "guard.h"


MessageQueue::MessageQueue() {
  pthread_mutex_init(&m_lock, NULL);
  sem_init(&m_avail,0,0);

}

MessageQueue::~MessageQueue() {
  pthread_mutex_destroy(&m_lock);
  sem_destroy(&m_avail);
}

void MessageQueue::enqueue(Message *msg) {
  Guard guard(m_lock); // protect mutex with lock
  m_messages.push_back(new Message(msg->tag, msg->data));
  sem_post(&m_avail); // increment sem thread
}

Message *MessageQueue::dequeue() {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec += 1; // compute a time one second in the future

  Message *msg = NULL;
  if (!sem_timedwait(&m_avail, &ts)) {
    Guard guard(m_lock);
    if (!m_messages.empty()) {
      msg = m_messages.front();
      m_messages.pop_front();
    }
  }
  return msg;
}
