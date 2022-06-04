Usage
- vscode
	- Build it by vscode!
	- cd docker
	- sudo docker-compose up && sudo docker-compose build &
	- ./build/server.exe --ip 192.168.1.50 & ./build/tests.exe --ip 192.168.1.50
	- kill $(pidof server.exe)
- make
	- make lab2

```
[==========] Running 2 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 1 test from test_create
[ RUN      ] test_create.basic_test_set
WARNING: not find --ip arg, DEFAULT_IP=192.168.1.50 will be used
Started server on 192.168.1.50:8080
[       OK ] test_create.basic_test_set (106 ms)
[----------] 1 test from test_create (106 ms total)

[----------] 1 test from test_add
[ RUN      ] test_add.basic_test_set
```