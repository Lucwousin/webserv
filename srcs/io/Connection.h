#pragma once

#include <list>
#include <memory>
#include <stack>

#include "ConnectionBuffer.h"
#include "EventQueue.h"
#include "Socket.h"
#include "config/ConfigServer.h"
#include "http/Request.h"
#include "http/Response.h"

class ITask;
class OTask;

class Connection {
 private:
  using host_map_t = std::map<std::string, ConfigServer&>;
  const host_map_t& host_map_;

  Socket socket_;
  ConnectionBuffer buffer_;
  EventQueue& event_queue_;
  std::list<std::unique_ptr<ITask>> iqueue_;
  std::list<std::unique_ptr<OTask>> oqueue_;
  Request request_;

  bool keep_alive_ = true;
  bool client_fin_ = false;

  time_t last_event_;
  uint32_t request_count_ = 0;

 public:
  Connection(Socket&& socket, EventQueue& event_queue, const host_map_t& host_map);
  ~Connection();

  bool handle(EventQueue::event_t& event);
  WS::IOStatus handleIn();
  WS::IOStatus handleOut();

  Socket& getSocket();
  ConnectionBuffer& getBuffer();
  const host_map_t& getHostMap() const;

  void addTask(std::unique_ptr<ITask>&& task);
  void addTask(std::unique_ptr<OTask>&& task);

  inline void setKeepAlive(bool keepAlive) { keep_alive_ = keepAlive; }
  inline bool keepAlive() const { return keep_alive_; };
  // Send the FIN packet, signifying that we're done. After this the peer should
  // also send one, we can then close the socket!
  void shutdown();

  void awaitRequest();
  void enqueueResponse(Response&& response);
  // Enqueue a 408 Request Timeout response
  void timeout();

  // Returns true, if the last event was more than WS::timeout seconds ago
  bool stale(time_t now) const;
  bool idle() const;

  const std::string& getName() const;
};

inline std::ostream& operator<<(std::ostream& stream, const Connection& connection) {
  return stream << connection.getName() << "\t\t";
}
