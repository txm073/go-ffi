package main

/*
#include "shared.h"
*/
import "C"
import (
	"archive/zip"
	"bytes"
	"io"
	"os"
	"path/filepath"
	"runtime/cgo"
	"sync"
)

var (
	goArchives = sync.Map{}
)

type Archive struct {
	buffer    []byte
	zipReader *zip.Reader
}

func createArchive(buffer []byte) (Archive, error) {
	reader := bytes.NewReader(buffer)
	zipReader, err := zip.NewReader(reader, int64(len(buffer)))
	if err != nil {
		return Archive{}, err
	}
	archive := Archive{buffer, zipReader}
	return archive, nil
}

func (archive Archive) getFileSize(path string) (uint64, bool) {
	for index := range archive.zipReader.File {
		if archive.zipReader.File[index].Name == path {
			return archive.zipReader.File[index].UncompressedSize64, true
		}
	}
	return 0, false
}

func (archive Archive) extract(path string) ([]byte, error) {
	file, err := archive.zipReader.Open(path)
	if err != nil {
		return []byte{}, err
	}
	defer file.Close()
	buffer := []byte{}
	_, err = file.Read(buffer)
	return buffer, err
}

func (archive Archive) extractToDisk(path string, dest string) bool {
	for index := range archive.zipReader.File {
		f := archive.zipReader.File[index]
		if f.Name == path {
			if f.Mode().IsDir() {
				return false
			}
			err := archive.writeFile(dest, f)
			if err != nil {
				return false
			}
		}
	}
	return true
}

func (archive Archive) writeFile(path string, f *zip.File) error {
	rc, err := f.Open()
	if err != nil {
		return err
	}
	defer func() {
		if err := rc.Close(); err != nil {
			panic(err)
		}
	}()

	if f.FileInfo().IsDir() {
		os.MkdirAll(path, f.Mode())
	} else {
		os.MkdirAll(filepath.Dir(path), f.Mode())
		f, err := os.OpenFile(path, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, f.Mode())
		if err != nil {
			return err
		}
		defer func() {
			if err := f.Close(); err != nil {
				panic(err)
			}
		}()

		_, err = io.Copy(f, rc)
		if err != nil {
			return err
		}
	}
	return nil
}

func (archive Archive) extractAll(destDir string) error {
	os.MkdirAll(destDir, 0755)
	for _, f := range archive.zipReader.File {
		path := filepath.Join(destDir, f.Name)
		err := archive.writeFile(path, f)
		if err != nil {
			return err
		}
	}
	return nil
}

//export go_Archive_create
func go_Archive_create(buffer []byte) C.go_handle_t {
	archive, err := createArchive(buffer)
	if err != nil {
		return C.go_handle_t(-1)
	}
	handle := cgo.NewHandle(archive)
	goArchives.Store(handle, archive)
	return C.go_handle_t(handle)
}

//export go_Archive_extract
func go_Archive_extract(handle C.go_handle_t, path string, buffer []byte) C.int32_t {
	obj, ok := goArchives.Load(cgo.Handle(handle))
	if !ok {
		return C.INVALID_HANDLE
	}
	archive := obj.(Archive)
	file, err := archive.zipReader.Open(path)
	if err != nil {
		return C.int32_t(-1)
	}
	defer file.Close()
	_, err = file.Read(buffer)
	return C.SUCCESS
}

//export go_Archive_extractAll
func go_Archive_extractAll(handle C.go_handle_t, dstPath string) C.int32_t {
	obj, ok := goArchives.Load(cgo.Handle(handle))
	if !ok {
		return C.INVALID_HANDLE
	}
	archive := obj.(Archive)
	err := archive.extractAll(dstPath)
	if err != nil {
		return C.int32_t(-2)
	}
	return C.SUCCESS
}

//export go_Archive_extractToDisk
func go_Archive_extractToDisk(handle C.go_handle_t, filePath string, dstPath string) C.int32_t {
	obj, ok := goArchives.Load(cgo.Handle(handle))
	if !ok {
		return C.INVALID_HANDLE
	}
	archive := obj.(Archive)
	ok = archive.extractToDisk(filePath, dstPath)
	if !ok {
		return C.int32_t(-2)
	}
	return C.SUCCESS
}

//export go_Archive_getFileSize
func go_Archive_getFileSize(handle C.go_handle_t, path string) C.int64_t {
	obj, ok := goArchives.Load(cgo.Handle(handle))
	if !ok {
		return C.int64_t(C.INVALID_HANDLE)
	}
	archive := obj.(Archive)
	size, ok := archive.getFileSize(path)
	if !ok {
		return C.int64_t(-1)
	}
	return C.int64_t(size)
}

//export go_Archive_delete
func go_Archive_delete(handle C.go_handle_t) {
	goHandle := cgo.Handle(handle)
	goArchives.Delete(goHandle)
	goHandle.Delete()
}
