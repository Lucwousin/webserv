#pragma once

#include <list>
#include <memory>
#include <set>
#include <string>
#include <vector>

class Message {
 public:
  using headers_t = std::vector<std::string>;
  using body_t = std::unique_ptr<const char[]>;

 protected:
  static constexpr size_t no_body_magic = 0xB0D135B0D135B00B;

  std::string message_;
  headers_t headers_;
  body_t body_;
  size_t body_size_ = no_body_magic;

 public:
  Message() = default;
  virtual ~Message() = default;
  Message(Message&& other) noexcept = default;
  Message& operator=(Message&& rhs) noexcept = default;

  [[nodiscard]] const std::string& getMessage() const;
  [[nodiscard]] const headers_t& getHeaders() const;
  [[nodiscard]] const body_t& getBody() const;
  [[nodiscard]] size_t getBodySize() const;
  [[nodiscard]] bool hasBody() const;

  void setBody(std::unique_ptr<const char[]>&& body, size_t body_size);  // ? This will change
                                                                         // this comment was still accurate
  void addHeader(const std::string& key, const std::string& val);
  void addHeader(const std::string& kv_pair);

  [[nodiscard]] std::string_view getHeader(const std::string& key) const;

  std::ostream& write(std::ostream& out) const;
};

std::ostream& operator<<(std::ostream& out, const Message& msg);
