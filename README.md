**LAB FOUR**

Usage
- vscode
	- Build it by vscode!
	- cd docker
	- sudo docker-compose up && sudo docker-compose build &
	- ./build/server.exe & 
	- ./build/kafka.exe & 
	- ./build/tests.exe
	- kill $(pidof server.exe kafka.exe)
- make
	- make init
	- make test
	- make clean

```
[==========] Running 2 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 1 test from test_create
[ RUN      ] test_create.basic_test_set
[       OK ] test_create.basic_test_set (438 ms)
[----------] 1 test from test_create (438 ms total)

[----------] 1 test from test_add
[ RUN      ] test_add.basic_test_set
Request POST /person from 192.168.1.50:38584
Request POST /person from 192.168.1.50:38586
Request POST /person from 192.168.1.50:38588
```

kafka-logs
```
kafka-node-1    | [2022-06-04 23:53:37,195] INFO [GroupCoordinator 1]: Assignment received from leader for group 0 for generation 1. The group has 1 members, 0 of which are static. (kafka.coordinator.group.GroupCoordinator)
Got assigned: [ mon_server[0:#] ]
{"age":22,"first_name":"Cleopatry","last_name":"Brusnichiy","login":"hstiv"}
{"age":21,"first_name":"Dora","last_name":"Dura","login":"duradura"}
{"age":20,"first_name":"Vasya","last_name":"Pupkin","login":"pup"}
```