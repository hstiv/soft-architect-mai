.PHONY:			init test clean

init:
	@cmake -S . -B build

test:
	@./build/server.exe & 
	./build/tests.exe

clean:
	kill $(pidof server.exe)