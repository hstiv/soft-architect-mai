.PHONY:			init test clean

init:
	@cd ./docker && sudo docker-compose build
	@cd ./docker && sudo docker-compose up &
	@cmake -S . -B build

test:
	@./build/server.exe &
	./build/tests.exe

clean:
	kill $(pidof server.exe)
	@cd ./docker && sudo docker-compose down