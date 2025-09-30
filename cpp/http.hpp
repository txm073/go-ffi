#pragma once

#ifndef HNAPI
#define HNAPI
#endif

#include "../lib/shared.h"
#include <vector>
#include <cstdint>
#include <map>
#include <string>

namespace hn::http {

class Response {
private:
  resp_t resp;
public:
  inline Response() {}
  inline ~Response() { free(resp.content); }
  inline const char* text() const { return resp.content; }
  inline int length() const { return resp.len; }
  inline int code() const { return resp.code; }
  inline resp_t* raw() { return &resp; }
};

typedef std::map<std::string, std::string> Parameters;
typedef Parameters Headers;
typedef Headers PostData;

enum class Method {
  Get, Post, Delete, Put
};

enum class ContentType {
  ApplicationJSON,
  ApplicationFormUrlEncoded
};

class Session {
private:
  go_handle_t handle;
  void setHeaders(const Headers& headers);
  void setParams(const Parameters& params);
  void setPostData(const PostData& data, ContentType contentType);
  const char* getMethodString(Method method);
  void checkReturn(int ret);
  ContentType getContentType(const Headers& headers);
  Response request(
    std::string& url, 
    Method method,
    const Parameters& params,
    const Headers& headers,
    const PostData& data
  );

public:
  Session();
  ~Session();

  inline Response get(
    std::string url, 
    const Parameters& params = {}, 
    const Headers& headers = {}
  ) {
    return request(url, Method::Get, params, headers, {});
  }

  inline Response put(
    std::string url, 
    const Parameters& params = {}, 
    const Headers& headers = {}
  ) {
    return request(url, Method::Put, params, headers, {});
  }

  inline Response del(
    std::string url, 
    const Parameters& params = {}, 
    const Headers& headers = {}
  ) {
    return request(url, Method::Delete, params, headers, {});
  }

  inline Response post(
    std::string url, 
    const Parameters& params = {}, 
    const Headers& headers = {}, 
    const PostData& data = {}
  ) {
    return request(url, Method::Post, params, headers, data);
  }

};

HNAPI inline Response get(
  std::string url, 
  const Parameters& params = {}, 
  const Headers& headers = {}
) {
  Session s;
  return s.get(url, params, headers);
}

HNAPI inline Response put(
  std::string url, 
  const Parameters& params = {}, 
  const Headers& headers = {}
) {
  Session s;
  return s.put(url, params, headers);
}

HNAPI inline Response del(
  std::string url, 
  const Parameters& params = {}, 
  const Headers& headers = {}
) {
  Session s;
  return s.del(url, params, headers);
}

HNAPI inline Response post(
  std::string url, 
  const Parameters& params = {}, 
  const Headers& headers = {}, 
  const PostData& data = {}
) {
  Session s;
  return s.post(url, params, headers, data);
}

}

