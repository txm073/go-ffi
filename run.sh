cd lib
go build -buildmode=c-shared -o goapi.so
mv goapi.so libgoapi.so
cd ..
g++ main.cpp cpp/http.cpp cpp/zip.cpp -o main -Llib -lgoapi -Wl,-rpath=./lib
./main
