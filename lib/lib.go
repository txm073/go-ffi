package main

/*
#include "shared.h"
*/
import "C"

//export testAdd
func testAdd(a, b C.int32_t) C.int32_t { return a + b }

func main() {}
