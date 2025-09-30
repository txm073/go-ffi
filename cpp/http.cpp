#include "http.hpp"
#include "../lib/goapi.h"

#include <stdexcept>
#include <cstdlib>
#include <cstring>

namespace hn::http {

HNAPI Session::Session() {
  handle = go_HttpClient_create();
}

HNAPI Session::~Session() {
  go_HttpClient_delete(handle);
}

void Session::setHeaders(const Headers& headers) {
  for (const auto& [k, v]: headers) {
    GOSTRING(goHeader, k.c_str());
    GOSTRING(goValue, v.c_str());
    checkReturn(go_HttpClient_setHeader(handle, goHeader, goValue));
  }
}

void Session::setParams(const Parameters& parameters) {
  for (const auto& [k, v]: parameters) {
    GOSTRING(goParam, k.c_str());
    GOSTRING(goValue, v.c_str());
    checkReturn(go_HttpClient_setParam(handle, goParam, goValue));
  }
}

void Session::setPostData(const PostData& data, ContentType contentType) {}

const char* Session::getMethodString(Method method) {
  switch (method) {
    case Method::Get:
      return "GET";
    case Method::Put:
      return "PUT";
    case Method::Post:
      return "POST";
    case Method::Delete:
      return "DELETE";
  }
  throw std::runtime_error("invalid method");
}

void Session::checkReturn(int ret) {
  if (ret != SUCCESS) {
    std::string msg = "http::Session error: " + std::to_string(ret);
    throw std::runtime_error(msg.c_str());
  }
}

Response Session::request(
  std::string& url,
  Method method,
  const Parameters& params,
  const Headers& headers,
  const PostData& data
) {

  GOSTRING(goUrl, url.c_str());
  GOSTRING(goMethod, getMethodString(method));
  checkReturn(go_HttpClient_newRequest(handle, goUrl, goMethod));
  setParams(params);
  setHeaders(headers);
  Response resp;
  checkReturn(go_HttpClient_performRequest(handle, resp.raw()));
  return resp;
}

}

