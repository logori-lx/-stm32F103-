dealall: execution

server: server.c dealdata.c dealdata.h
	gcc -o server.exe server.c dealdata.c dealdata.h Clientlist.c Clientlist.h -lpthread 
execution: server.exe
	./server.exe 172.26.25.37 1234
test: test.c
	gcc -o test.exe test.cpp -lpthread
	./test.exe
clean:
	rm -rf testServer.exe test.exe

