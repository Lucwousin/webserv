#pragma once

#include "IOTask.h"

class SpliceIn : public ITask {
 public:
  SpliceIn(Connection& connection, int fd, size_t size) : conn_(connection), pipe_fd_(fd), remaining_(size) {}
  ~SpliceIn() { close(pipe_fd_); }

  bool operator()(Connection& connection) override {
    sock_rdy_ = true;

    if (pipe_rdy_) {
      while (!connection.getBuffer().readFailed()) {
        if (connection.getBuffer().writeTo(pipe_fd_, remaining_)) {
          pipe_rdy_ = false;
          connection.getQueue().add(pipe_fd_, EventQueue::out, this);
          break;
        }
      }
      sock_rdy_ = !connection.getBuffer().readFailed();
    }
    if (sock_rdy_)
      connection.getQueue().del(connection.getSocket().get_fd(), EventQueue::in);
    return done() || !pipe_rdy_;
  }
  bool done() override { return remaining_ == 0; }
  void onDone(Connection& connection) override {}

 private:
  Connection& conn_;
  int pipe_fd_;
  size_t remaining_;
  bool pipe_rdy_ = false;
  bool sock_rdy_ = false;
};