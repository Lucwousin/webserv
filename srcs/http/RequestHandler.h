#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H
#include "Request.h"
#include "Response.h"
#include "io/task/IOTask.h"
#include "ACase.h"
#include "Cases.h"

class RequestHandler : public ITask, public OTask {
 public:

  static ACase& findCase(Request& req) {
    const Cases& cases = Case::instance.get_instance;
    for (auto it = cases.cbegin(); it < cases.cend(); it++) {
      const std::unique_ptr<ACase>& ptr = *it;
      if (ptr->test(req)) {
        return *ptr;
      }
    }
    __builtin_unreachable();
  }

  explicit RequestHandler(Connection& connection, Request&& req)
  : connection_(connection), request_(std::move(req)) {
    case_ = findCase(req);

    if (request_.hasBody())
      connection.addTask(case_.bodyReader(request_));

  };
  RequestHandler(RequestHandler&& that) = default;
  ~RequestHandler() override = default;

  // maybe some return values or something to know it's ok to proceed?
  // void            readRequest(int fd);
  void            execRequest();
  Response&& getResponse();

  bool operator()(Connection& connection) override;
  void onDone(Connection& connection) override;
 private:
  Connection& connection_;
  Request   request_;
  Response  response_;

  ACase& case_ = Case::getCase(Case::FAIL);

  void doGET_();
  void doPOST_();
};

#endif
