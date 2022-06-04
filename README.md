**LAB THREE**

Usage
- vscode
	- Build it by vscode!
	- cd docker
	- sudo docker-compose up && sudo docker-compose build &
	- ./build/server.exe & 
	- ./build/tests.exe
	- kill $(pidof server.exe)
- make
	- make init
	- make test
	- make clean

```
[==========] Running 2 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 1 test from test_create
[ RUN      ] test_create.basic_test_set
[       OK ] test_create.basic_test_set (159 ms)
[----------] 1 test from test_create (164 ms total)

[----------] 1 test from test_add
[ RUN      ] test_add.basic_test_set
Request POST /person from 192.168.1.50:34134
Request POST /person from 192.168.1.50:34138
Request POST /person from 192.168.1.50:34136
[       OK ] test_add.basic_test_set (1010 ms)
[----------] 1 test from test_add (1010 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 2 test suites ran. (1201 ms total)
[  PASSED  ] 2 tests.
```