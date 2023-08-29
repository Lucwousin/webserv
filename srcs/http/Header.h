#pragma once

#include <algorithm>
#include <cassert>
#include <list>
#include <string>
#include <utility>

#include "ErrorResponse.h"
#include "util/WebServ.h"

namespace Header {
enum HStatus {
  OK,
  NO_KEY,
  ERROR
};
static constexpr auto ws = " \t\r\n";

static bool matches(const std::string& key, const std::string& header) {
  size_t keylen = key.size();
  return strncasecmp(key.c_str(), header.c_str(), keylen) == 0 &&
         header[header.find_first_not_of(ws, keylen)] == ':';
}

static int keyCompare(const std::string_view& a, const std::string_view& b) {
  return strncasecmp(a.data(), b.data(), std::max(a.size(), b.size()));
}

HStatus getHeaderKey(const std::string& header, std::string_view& dst) {
  if (isspace(header[0]))
    return NO_KEY;
  size_t end = header.find_first_of(':');
  if (end == 0 || end == std::string::npos)
    return ERROR;
  end = header.find_last_not_of(ws, end - 1);
  dst = {header.c_str(), end + 1};
  return OK;
}

std::string_view getHeaderValue(const std::string_view& header, size_t key_len = 0) {
  constexpr size_t not_found = std::string::npos;
  size_t start = header.find_first_of(':', key_len);
  if (start == not_found)
    start = 0;
  else
    start += 1;
  start = header.find_first_not_of(ws, start);
  assert(start != not_found);
  return {header.data() + start, header.find_last_not_of(ws) - start + 1};
}

};  // namespace Header
