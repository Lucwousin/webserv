#pragma once
#include <istream>
#include <string>

#include "Message.h"

class Request : public Message {
 public:
  enum Method {
    INVALID = 0,
    GET = 1,
    POST = 2,
  };
  enum HttpVersion {
    VER_INVALID,
    VER_1_1
  };

 private:
  Method method_ = INVALID;
  std::string uri_;
  HttpVersion version_ = VER_INVALID;

 public:
  Request() = default;
  ~Request() override = default;
  Request(Request&& other) noexcept = default;
  Request& operator=(Request&& rhs) noexcept = default;

  [[nodiscard]] Method getMethod() const;
  [[nodiscard]] const std::string& getUri() const;
  [[nodiscard]] HttpVersion getVersion() const;

  [[nodiscard]] std::string getPath() const;

  bool setMessage(const std::string& msg);
  void setUri(const std::string& uri);
};
