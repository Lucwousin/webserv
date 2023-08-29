#pragma once

#include "IOTask.h"
#include "http/Response.h"

class SendResponse : public OTask {
 private:
  enum State {
    MSG,
    HEADERS,
    SEPARATOR,
    BODY
  };

  typedef Message::headers_t::const_iterator header_iter_t;
  const Response response_;
  State state_ = MSG;
  header_iter_t header_;

 protected:
  virtual std::unique_ptr<OTask> getBodyTask() const = 0;

 public:
  explicit SendResponse(Response&& response);
  bool operator()(Connection& connection) override;
  void onDone(Connection& connection) final;
};

class SendErrorResponse : public SendResponse {
 public:
  explicit SendErrorResponse(int status) noexcept : SendResponse(ErrorResponse(status)) {}
};