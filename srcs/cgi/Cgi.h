#ifndef CGI_H
#define CGI_H
#include <string>
#include <algorithm>
#include "http/Request.h"
#include "http/Response.h"

class Cgi {
  public:
    Cgi(Request&);
    ~Cgi();

    std::string execute();
    static void makeDocumentResponse(std::string&&, Response&);
    static void makeLocalRedirResponse(std::string&&, Response&, Request&);
    static void makeClientRedirResponse(std::string&&, Response&);

  private:
    struct cgi_env {
      std::vector<std::string> strs;
      cgi_env(const Request& req, const std::string& path) {
        strs = {
            "AUTH_TYPE=",
            "CONTENT_LENGTH=" + std::to_string(req.getBodySize()),
            "CONTENT_TYPE=" + std::string(req.getHeader("content-type")),
            "GATEWAY_INTERFACE=CGI/1.1",
            "PATH_INFO=" + path,
            "PATH_TRANSLATED=", // root path_info based on confi
            "QUERY_STRING=" + req.getUri().substr(req.getUri().find('?') + 1),
            "REMOTE_ADDR=127.0.0.1", // for now just hardcode localhost, ask lucas to pass the real thing
            "REMOTE_HOST=" + std::string(req.getHeader("Host")),
            "REMOTE_USER=", // not sure that we need this as we're not doing authentication?
            "REQUEST_METHOD=" + std::string(req.getMethod() == Request::GET ? "GET" : "POST"),
            "SCRIPT_NAME=" + path,
            "SERVER_NAME=SuperWebserv10K/0.9.1 (Unix)",
            "SERVER_PORT=6969",
            "SERVER_PROTOCOL=HTTP/1.1",
            "SERVER_SOFTWARE=SuperWebserv10K/0.9.1 (Unix)"
        };
      }
      [[nodiscard]] std::unique_ptr<const char*[]> asArray() const {
        auto rv = std::make_unique<const char*[]>(strs.size() + 1);
        std::transform(strs.begin(), strs.end(), rv.get(), [](auto& s)->auto{return s.c_str();});
        rv[strs.size()] = nullptr;
        return rv;
      }
    };

    const std::string body_;
    const std::string path_;
    cgi_env           envp_;
    int               pipe_in_[2];
    int               pipe_out_[2];

    void exec_child();
    std::string exec_parent(int pid);
};

#endif
