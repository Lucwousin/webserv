#pragma once
#include <map>

#include "IOTask.h"

class ReadRequest : public ITask {
 private:
  enum State {
    MSG,
    HEADERS,
    BODY,
  };

  Request request_;
  State state_ = MSG;
  int error_ = 0;
  size_t req_len_ = 0;

  bool use_line(Connection& connection, std::string& line);
  bool handle_msg(Connection&, std::string& line);
  bool handle_header(Connection& connection, std::string& line);

 public:
  ReadRequest() = default;
  explicit ReadRequest(Request&& req);
  bool operator()(Connection& connection) final;
  void onDone(Connection& connection) final;

 private:
  using header_lambda = int (*)(ReadRequest&, const std::string_view&, Connection&);
  using header_lambda_map = std::multimap<std::string, header_lambda>;
  static const header_lambda_map hhooks_;
};
