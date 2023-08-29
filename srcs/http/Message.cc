#include "Message.h"
#include "Header.h"

#include <ostream>
#include <cstring>

const std::string& Message::getMessage() const {
  return message_;
}

const Message::headers_t& Message::getHeaders() const {
  return headers_;
}

const Message::body_t& Message::getBody() const {
  return body_;
}

size_t Message::getBodySize() const {
  return body_size_;
}

bool Message::hasBody() const {
  return body_size_ != no_body_magic;
}

void Message::setBody(std::unique_ptr<const char[]>&& body, size_t body_size) {
  body_ = std::move(body);
  body_size_ = body_size;
}

void Message::addHeader(const std::string& kv_pair) {
  headers_.push_back(kv_pair);
}

void Message::addHeader(const std::string& key, const std::string& val) {
  headers_.push_back(key + ": " + val + "\r\n");
}

std::string_view Message::getHeader(const std::string& key) const {
  // rfind searches the string for the last occurrence of its arguments.
  // When pos is specified, the search only includes sequences of characters
  // that begin at or before position pos, ignoring any possible match beginning after pos.
  // In other words, rfind(str, 0) == startswith (shoutout Ludovic Aubert on stackoverflow)
  for (const auto& elem : headers_)
    if (Header::matches(key, elem))
      return elem;
  return {};
}

std::ostream& Message::write(std::ostream& out) const {
  out << message_;
  for (const auto& header : headers_)
    out << header;
  out << "\r\n";
  if (body_)
    out.write(body_.get(), std::streamsize(body_size_));
  return out;
}

std::ostream& operator<<(std::ostream& out, const Message& msg) {
  msg.write(out);
  return out;
}
