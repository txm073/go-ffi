package main

/*
#include "shared.h"
*/
import "C"
import (
	"io"
	"net/http"
	"net/url"
	"runtime/cgo"
	"sync"
)

type HttpClient struct {
	client  http.Client
	request *http.Request
	params  url.Values
}

var (
	httpClients = sync.Map{}
)

//export go_HttpClient_create
func go_HttpClient_create() C.go_handle_t {
	httpClient := HttpClient{http.Client{}, nil, url.Values{}}
	handle := cgo.NewHandle(httpClient)
	httpClients.Store(handle, httpClient)
	return C.go_handle_t(handle)
}

//export go_HttpClient_delete
func go_HttpClient_delete(handle C.go_handle_t) {
	goHandle := cgo.Handle(handle)
	httpClients.Delete(goHandle)
	goHandle.Delete()
}

//export go_HttpClient_newRequest
func go_HttpClient_newRequest(
	handle C.go_handle_t,
	url string,
	method string,
) C.int32_t {
	goHandle := cgo.Handle(handle)
	obj, ok := httpClients.Load(goHandle)
	if !ok {
		return C.INVALID_HANDLE
	}
	client := obj.(HttpClient)
	newRequest, err := http.NewRequest(method, url, nil)
	if err != nil {
		return C.int32_t(-1)
	}
	client.request = newRequest
	httpClients.Store(goHandle, client)
	return C.SUCCESS
}

//export go_HttpClient_setHeader
func go_HttpClient_setHeader(
	handle C.go_handle_t,
	header string,
	value string,
) C.int32_t {
	obj, ok := httpClients.Load(cgo.Handle(handle))
	if !ok {
		return C.INVALID_HANDLE
	}
	client := obj.(HttpClient)
	client.request.Header.Set(header, value)
	return C.SUCCESS
}

//export go_HttpClient_setParam
func go_HttpClient_setParam(
	handle C.go_handle_t,
	param string,
	value string,
) C.int32_t {
	obj, ok := httpClients.Load(cgo.Handle(handle))
	if !ok {
		return C.INVALID_HANDLE
	}
	client := obj.(HttpClient)
	client.params.Add(param, value)
	return C.SUCCESS
}

//export go_HttpClient_performRequest
func go_HttpClient_performRequest(handle C.go_handle_t, resp *C.resp_t) C.int32_t {
	obj, ok := httpClients.Load(cgo.Handle(handle))
	if !ok {
		return C.INVALID_HANDLE
	}
	client := obj.(HttpClient)
	client.request.URL.RawQuery = client.params.Encode()
	httpResp, err := client.client.Do(client.request)
	resp.code = C.int32_t(httpResp.StatusCode)
	if err != nil {
		errMsg := err.Error()
		resp.len = C.size_t(len(errMsg))
		resp.content = C.CString(errMsg)
		return C.int32_t(-1)
	}
	if httpResp.StatusCode == http.StatusOK {
		bodyBytes, err := io.ReadAll(httpResp.Body)
		if err != nil {
			return C.int32_t(-1)
		}
		resp.len = C.size_t(httpResp.ContentLength)
		resp.content = C.CString(string(bodyBytes))
	}
	return C.SUCCESS
}
