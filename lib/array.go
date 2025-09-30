package main

/*
#include "shared.h"
*/
import "C"
import (
	"runtime/cgo"
	"slices"
	"sync"
	"unsafe"
)

// Global map to hold Go maps and a counter for handles.
var (
	goSlices           = sync.Map{}
	sliceHandleCounter int64
)

//export go_insertElement
func go_insertElement(handle C.go_handle_t, elemType C.type_t, index C.int32_t, elem unsafe.Pointer) C.int32_t {
	m, ok := goSlices.Load(cgo.Handle(handle))
	if !ok {
		return C.int32_t(C.INVALID_HANDLE)
	}
	goIndex := int(index)
	goSlice := m.([]any)
	if elem == nil {
		goSlice = slices.Delete(goSlice, goIndex, goIndex+1)
	} else {
		var castedElem any
		switch elemType {
		case C.GO_STRING:
			castedElem = C.GoString((*C.buffer_t)(elem).data)
		case C.GO_INT32:
			castedElem = int32(*(*C.int32_t)(elem))
		case C.GO_INT64:
			castedElem = int64(*(*C.int64_t)(elem))
		case C.GO_FLOAT32:
			castedElem = float32(*(*C.float32_t)(elem))
		case C.GO_FLOAT64:
			castedElem = float64(*(*C.float64_t)(elem))
		}
		if goIndex == len(goSlice) {
			goSlice = append(goSlice, castedElem)
		} else {
			goSlice = slices.Insert(goSlice, goIndex, castedElem)
		}
	}
	goHandle := cgo.Handle(handle)
	goSlices.Store(goHandle, goSlice)
	return C.int32_t(C.SUCCESS)
}

//export go_setElement
func go_setElement(handle C.go_handle_t, elemType C.type_t, index C.int32_t, elem unsafe.Pointer) C.int32_t {
	m, ok := goSlices.Load(cgo.Handle(handle))
	if !ok {
		return C.int32_t(C.INVALID_HANDLE)
	}
	goSlice := m.([]any)
	var castedElem any
	switch elemType {
	case C.GO_STRING:
		castedElem = C.GoString((*C.buffer_t)(elem).data)
	case C.GO_INT32:
		castedElem = int32(*(*C.int32_t)(elem))
	case C.GO_INT64:
		castedElem = int64(*(*C.int64_t)(elem))
	case C.GO_FLOAT32:
		castedElem = float32(*(*C.float32_t)(elem))
	case C.GO_FLOAT64:
		castedElem = float64(*(*C.float64_t)(elem))
	}
	goSlice[index] = castedElem
	return C.int32_t(C.SUCCESS)
}

//export go_getElement
func go_getElement(handle C.go_handle_t, elemType C.type_t, index C.int32_t, outElem unsafe.Pointer) C.int32_t {
	m, ok := goSlices.Load(cgo.Handle(handle))
	if !ok {
		return C.int32_t(C.INVALID_HANDLE)
	}
	goSlice := m.([]any)
	goIndex := int(index)
	switch elemType {
	case C.GO_STRING:
		val := goSlice[index].(string)
		buffer := (*C.buffer_t)(outElem)
		buffer.len = C.size_t(len(val))
		buffer.data = C.CString(val)
	case C.GO_INT32:
		*(*C.int32_t)(outElem) = C.int32_t(goSlice[goIndex].(int32))
	case C.GO_INT64:
		*(*C.int64_t)(outElem) = C.int64_t(goSlice[goIndex].(int64))
	case C.GO_FLOAT32:
		*(*C.float32_t)(outElem) = C.float32_t(goSlice[goIndex].(float32))
	case C.GO_FLOAT64:
		*(*C.float64_t)(outElem) = C.float64_t(goSlice[goIndex].(float64))
	}
	return C.int32_t(C.SUCCESS)
}

//export go_getSliceLength
func go_getSliceLength(handle C.go_handle_t) C.int32_t {
	m, ok := goSlices.Load(cgo.Handle(handle))
	if !ok {
		return C.int32_t(C.INVALID_HANDLE)
	}
	return C.int32_t(len(m.([]any)))
}

//export go_createSlice
func go_createSlice() C.go_handle_t {
	goSlice := []any{}
	handle := cgo.NewHandle(goSlice)
	goSlices.Store(handle, goSlice)
	sliceHandleCounter++
	return C.go_handle_t(handle)
}

//export go_deleteSlice
func go_deleteSlice(handle C.go_handle_t) {
	goHandle := cgo.Handle(handle)
	goSlices.Delete(goHandle)
	goHandle.Delete()
	sliceHandleCounter--
}
