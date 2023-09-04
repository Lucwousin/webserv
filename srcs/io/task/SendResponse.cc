#include "SendResponse.h"

SendResponse::SendResponse(Response&& response)
  : response_(std::forward<Response>(response)),
    header_(response_.getHeaders().begin()) {}

bool SendResponse::operator()(Connection& connection) {
  while (!connection.getBuffer().needWrite()) {
    switch (state_) {
      case MSG:
        Log::info(connection, "OUT:\t", ((std::string_view)response_.getMessage()).substr(0, response_.getMessage().find_last_not_of("\n\r") + 1), '\n');
        connection.getBuffer() << response_.getMessage();
        state_ = HEADERS;
        break;
      case HEADERS:
        Log::trace(connection, "H:\t\t", ((std::string_view)*header_).substr(0, header_->find_last_not_of("\n\r") + 1), '\n');
        connection.getBuffer() << *header_++;
        if (header_ == response_.getHeaders().end())
          state_ = SEPARATOR;
        break;
      case SEPARATOR:
        connection.getBuffer() << "\r\n";
        return true;
    }
  }
  return false;
}

void SendResponse::onDone(Connection&) {
  Log::trace("Completed response\n");
}
