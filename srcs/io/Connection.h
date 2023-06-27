#pragma once

#include <list>
#include <memory>
#include <stack>

#include "BufferPool.h"
#include "ConnectionBuffer.h"
#include "EventQueue.h"
#include "Socket.h"
#include "http/Request.h"

class ITask;
class OTask;

class Connection {
 private:
  Socket socket_;
  ConnectionBuffer buffer_;
  EventQueue& event_queue_;
  std::list<std::unique_ptr<ITask>> iqueue_;
  std::list<std::unique_ptr<OTask>> oqueue_;
  Request request_;

  bool writing_ = false;
  bool keep_alive_ = true;

 public:
  Connection(int fd, EventQueue& event_queue, BufferPool<>& buf_mgr);
  ~Connection();

  void handle(EventQueue::event_t& event);
  void handleIn(WS::IOStatus& status);
  void handleOut(WS::IOStatus& status);

  Socket& getSocket();
  ConnectionBuffer& getBuffer();

  void addTask(ITask* task);
  void addTask(OTask* task);

  inline void setKeepAlive(bool keepAlive) { keep_alive_ = keepAlive; }
  inline bool keepAlive() const { return keep_alive_; };
  inline bool shouldClose() const { return should_close_; }
  void close();
};
