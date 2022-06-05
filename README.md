**LAB ONE**

Usage
 - Build it!
 - ./build/server.exe & ./build/tests.exe
 - kill $(pidof server.exe)


**=====>  Browser Results  <=====**

Address: http://192.168.1.50:8080/person?login=hstiv

Response: [{"age":22,"first_name":"Cleopatry","last_name":"Brusnichiy","login":"hstiv"}]

**=====>  Test Results  <=====**
```
[==========] Running 2 tests from 2 test suites.
[----------] Global test environment set-up.
[----------] 1 test from test_create
[ RUN      ] test_create.basic_test_set
[       OK ] test_create.basic_test_set (119 ms)
[----------] 1 test from test_create (119 ms total)

[----------] 1 test from test_add
[ RUN      ] test_add.basic_test_set
[       OK ] test_add.basic_test_set (1031 ms)
[----------] 1 test from test_add (1031 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 2 test suites ran. (1150 ms total)
[  PASSED  ] 2 tests.
```