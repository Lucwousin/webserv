#ifndef ACASE_H
#define ACASE_H
#include <sys/stat.h>
#include "Request.h"
#include "Response.h"
#include "io/task/IOTask.h"

class ACase {
 public:
  using stat_t = struct stat;

  struct RequestInfo {
   public:
    explicit RequestInfo(const Request& req)
      : req(req),
        path(req.getPath()),
        is_dir(*path.rbegin() == '/') {}

    const Request& req;
    const std::string path;
    const bool is_dir;
   private:

  };

   ACase() = default;
   virtual ~ACase() = default;

   virtual bool test(RequestInfo& req) const = 0;
   virtual std::unique_ptr<ITask> bodyReader(RequestInfo& req) const = 0;
   virtual std::unique_ptr<OTask> getResponse(RequestInfo& req) const = 0;
};

#endif
