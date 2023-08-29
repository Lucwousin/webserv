#include <array>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <sstream>
#include "Cgi.h"
#include "util/WebServ.h"
#include "http/ErrorResponse.h"
#include "http/RequestHandler.h"
#include "http/Header.h"

static void st_del_arr(char** arr)
{
  size_t i = 0;
  if (!arr)
    return;
  while (arr[i]) {
    delete[] arr[i];
    i++;
  }
  delete[] arr;
}

Cgi::Cgi(Request& req) :
    body_(req.getBody().get()),
    path_("." + req.getPath().substr(0, req.getPath().find(".cgi") + 4)), // fix getPath() !!! (or confirm that it's working)
    envp_(req, path_),
    pipe_in_(), pipe_out_()
{}

Cgi::~Cgi() = default;

// read from parent process on stdin
// execute script
// write output of script back to parent
// make sure to test for leaking fd's!!!
void Cgi::exec_child()
{
  if (this->body_.length() > 0) {
    dup2(this->pipe_in_[0], STDIN_FILENO);
    close(this->pipe_in_[0]);
    close(this->pipe_in_[1]);
  }
  dup2(this->pipe_out_[1], STDOUT_FILENO);
  close(this->pipe_out_[0]);
  close(this->pipe_out_[1]);
  char* argv[] = {nullptr};
  execve(this->path_.c_str(), argv, (char**)envp_.asArray().get());
  // if we get here execve failed
  Log::error("executing CGI `", this->path_, "' failed: ", strerror(errno), '\n');
  exit(1);
}

// write request body to child's stdin
// wait for child then read child's stdout
// these read/write ops should go through epoll?!
std::string Cgi::exec_parent(int pid)
{
  if (this->body_.length() > 0) {
    close(this->pipe_in_[0]);
    if (write(this->pipe_in_[1], this->body_.c_str(), this->body_.length()) == -1) { // this won't work for cte
      throw (ErrorResponse(500));
    }
    close(this->pipe_in_[1]);
  }
  close(this->pipe_out_[1]);
  waitpid(pid, nullptr, 0);

  std::stringstream body;
  const ssize_t buf_size = 4096; // what would be optimal here?
  char buf[buf_size];
  ssize_t read_bytes;
  do {
    read_bytes = read(this->pipe_out_[0], buf, buf_size);
    if (read_bytes < 0)
      throw (ErrorResponse(500));
    body.write(buf, read_bytes);
  } while (read_bytes == buf_size);

  close(this->pipe_out_[0]);
  return (body.str());
}

std::string Cgi::execute()
{
  // open pipes, input pipe is only necessary if there is a body to write
  // both stdin are redirected the the other process stdout
  if (this->body_.length() > 0) {
    if (pipe(this->pipe_in_) == -1) {
      throw (ErrorResponse(500));
    }
  }
  if (pipe(this->pipe_out_) == -1) {
    throw (ErrorResponse(500));
  }

  // fork and run the parent and child process in separate functions
  int pid = fork();
  if (pid < 0) {
    throw (ErrorResponse(500));
  }
  if (pid == 0) {
    exec_child();
  }
  std::string result = exec_parent(pid);
  return (result);
}

// function to process raw cgi document response into http response
void Cgi::makeDocumentResponse(std::string&& raw, Response& res)
{
  size_t body_begin = HTTP::find_header_end(raw);
  // size_t body_begin = raw.find("\n\n");
  if (body_begin == std::string::npos) { // not compliant with rfc
    throw (ErrorResponse(500));
  }
  body_begin += 2;
  char* dup;
  try {
    dup = new char[raw.length() - body_begin];
  }
  catch (std::exception&) {
    throw (ErrorResponse(500));
  }
  raw.copy(dup, raw.length(), body_begin);
  res.setBody(dup, raw.length() - body_begin);
  size_t substr_start = raw.find("Content-Type: ") + "Content-Type: ".length();
  size_t substr_len = raw.find(';', substr_start) - substr_start; // spec does not specify ";" as delimiter?
  std::string content_type = raw.substr(substr_start, substr_len);
  res.addHeader("Server", "SuperWebserv10K/0.9.1 (Unix)");
  res.addHeader("Content-Length", std::to_string(raw.length() - body_begin));
  res.addHeader("Content-Type", content_type);
  res.setMessage(200);
}

// function to process raw cgi local redirect response into http response
// this function has not been tested at all!!!
void Cgi::makeLocalRedirResponse(std::string&& raw, Response& res, Request& req)
{
  req.setUri(Header::get(raw, "Location: "));
  RequestHandler rh(req);
  rh.execRequest();
  res = rh.getResponse();
}

// function to process raw cgi client redirect response into http response
void Cgi::makeClientRedirResponse(const std::string& raw, Response& res)
{
  // This is done to add the 302 body!
  res = ErrorResponse(302);
  res.addHeader("Location", st_find_header_value(raw, "Location: "));
  res.addHeader("Server", "SuperWebserv10K/0.9.1 (Unix)");
  // does not contain body
  if (raw.find("Content-Type") == std::string::npos) {
    return;
  }
  // contains body
  res.addHeader("Content-Type", st_find_header_value(raw, "Content-Type: "));
  const std::string body = raw.substr(HTTP::find_header_end(raw) + 2);
  res.addHeader("Content-Length", std::to_string(body.length()));
  char* dup;
  try {
    dup = new char[body.length() + 1];
  }
  catch (std::exception&) {
    throw (ErrorResponse(500));
  }
  body.copy(dup, body.length());
  dup[body.length()] = '\0';
  res.setBody(dup, body.length());
}
