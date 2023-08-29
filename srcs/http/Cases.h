#ifndef CASES_H
#define CASES_H
#include <memory>
#include <vector>

#include "ACase.h"
#include "Request.h"
#include "io/task/DiscardBody.h"

namespace get {
class CaseRedirect : public ACase {
  bool test(RequestInfo& ri) const override;
  std::unique_ptr<OTask> getResponse(RequestInfo& ri) const override;
  std::unique_ptr<ITask> bodyReader(RequestInfo& ri) const override;
};
class CaseNoFile : public ACase {
  bool test(RequestInfo& ri) const override;
  std::unique_ptr<OTask> getResponse(RequestInfo& ri) const override;
  std::unique_ptr<ITask> bodyReader(RequestInfo& ri) const override {
    return std::make_unique<DiscardBody>(ri.req.getBodySize());
  }
};
class CaseCGI : public ACase {
  bool test(RequestInfo& ri) const override;
  std::unique_ptr<OTask> getResponse(RequestInfo& ri) const override;
  std::unique_ptr<ITask> bodyReader(RequestInfo& ri) const override {
    return std::make_unique<DiscardBody>(ri.req.getBodySize());
  }
};
class CaseFile : public ACase {
  bool test(RequestInfo& ri) const override;
  std::unique_ptr<OTask> getResponse(RequestInfo& ri) const override;
  std::unique_ptr<ITask> bodyReader(RequestInfo& ri) const override {
    return std::make_unique<DiscardBody>(ri.req.getBodySize());
  }
};
class CaseDir : public ACase {
  bool test(RequestInfo& ri) const override;
  std::unique_ptr<OTask> getResponse(RequestInfo& ri) const override;
  std::unique_ptr<ITask> bodyReader(RequestInfo& ri) const override {
    return std::make_unique<DiscardBody>(ri.req.getBodySize());
  }
};
class CaseFail : public ACase {
  bool test(RequestInfo& ri) const override;
  std::unique_ptr<OTask> getResponse(RequestInfo& ri) const override;
  std::unique_ptr<ITask> bodyReader(RequestInfo& ri) const override;
};
};  // namespace get

class Cases {
 public:
  Cases();
  virtual ~Cases();

  typedef std::vector<std::unique_ptr<ACase> >::const_iterator citerator;
  citerator cbegin() const;
  citerator cend() const;
  size_t size() const;
  ACase& operator[](size_t n) const;

 protected:
  std::vector<std::unique_ptr<ACase> > cases_;

 private:
  Cases(const Cases&);             // = delete
  Cases& operator=(const Cases&);  // delete
};

class CasesGET : public Cases {
 public:
  CasesGET();
  ~CasesGET() override = default;
  CasesGET(const CasesGET&) = delete;
  CasesGET& operator=(const CasesGET&) = delete;
};

class CasesPOST : public Cases {
 public:
  CasesPOST() = default;
  ~CasesPOST() override = default;
  CasesPOST(const CasesPOST&) = delete;
  CasesPOST& operator=(const CasesPOST&) = delete;
};

namespace Case {
static struct {
  const CasesGET get_instance;
  const CasesPOST post_instance;
} instance;

enum Type {
  REDIRECT = 0,
  NO_FILE = 1,
  CGI = 2,
  FILE = 3,
  DIR = 4,
  FAIL = 5
};
static constexpr ACase& getCase(Type type) {
  return instance.get_instance[type];
}

constexpr auto& get = instance.get_instance;
constexpr auto& post = instance.post_instance;
}  // namespace Case

#endif
