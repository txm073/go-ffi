package main

/*
#include "shared.h"
*/
import "C"
import (
	"runtime/cgo"
	"sync"
	"unsafe"
)

// Global map to hold Go maps and a counter for handles.
var (
	goMaps           = sync.Map{}
	mapHandleCounter int64
)

//export go_getMapValue
func go_getMapValue(
	handle C.go_handle_t,
	keyType C.type_t,
	valType C.type_t,
	key unsafe.Pointer,
	outVal unsafe.Pointer) C.int32_t {
	m, ok := goMaps.Load(cgo.Handle(handle))
	if !ok {
		return C.int(C.INVALID_HANDLE)
	}
	switch keyType {
	case C.GO_STRING:
		goMap := m.(map[any]any)
		castedKey := C.GoString((*C.char)(key))
		return getMapValueString(castedKey, valType, goMap, (*C.buffer_t)(outVal))
	default:
		_ = m
		return C.int(C.INVALID_TYPE)
	}
}

func getMapValueString(key string, valType C.type_t, goMap map[any]any, outVal *C.buffer_t) C.int {
	val, ok := goMap[key]
	if !ok {
		return C.int(C.INVALID_KEY)
	}
	switch valType {
	case C.GO_STRING:
		castedVal := val.(string)
		outVal.len = C.size_t(len(castedVal))
		outVal.data = (*C.char)(C.malloc(outVal.len))
		tempString := C.CString(castedVal)
		C.memcpy((unsafe.Pointer)(outVal.data), (unsafe.Pointer)(tempString), outVal.len)
		C.free((unsafe.Pointer)(tempString))
	}
	return C.int(C.SUCCESS)
}

//export go_setMapValue
func go_setMapValue(
	handle C.go_handle_t,
	keyType C.type_t,
	valType C.type_t,
	key unsafe.Pointer,
	val unsafe.Pointer) C.int32_t {
	m, ok := goMaps.Load(cgo.Handle(handle))
	if !ok {
		return C.int(C.INVALID_HANDLE)
	}
	switch keyType {
	case C.GO_STRING:
		goMap := m.(map[any]any)
		castedKey := C.GoString((*C.char)(key))
		return setMapValueString(castedKey, valType, goMap, val)
	default:
		_ = m
		return C.int(C.INVALID_TYPE)
	}
}

func setMapValueString(key string, valType C.type_t, goMap map[any]any, inVal unsafe.Pointer) C.int {
	switch valType {
	case C.GO_STRING:
		castedVal := C.GoString((*C.char)(inVal))
		goMap[key] = castedVal
	}
	return C.int(C.SUCCESS)
}

//export go_createMap
func go_createMap() C.go_handle_t {
	goMap := map[any]any{}
	handle := cgo.NewHandle(goMap)
	goMaps.Store(handle, goMap)
	mapHandleCounter++
	return C.go_handle_t(handle)
}

//export go_deleteMap
func go_deleteMap(handle C.go_handle_t) {
	goHandle := cgo.Handle(handle)
	goMaps.Delete(goHandle)
	goHandle.Delete()
	mapHandleCounter--
}
