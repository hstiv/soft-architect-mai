cmake_minimum_required(VERSION 3.2)

project(LR1 C CXX)

SET (EXE_SERVER "server.exe")
SET (EXE_TESTS "tests.exe")

find_package(OpenSSL)
find_package(GTest REQUIRED)
find_package(Threads)
find_package(ZLIB)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL)

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})
include_directories(${GTEST_INCLUDE_DIRS})
include_directories("/usr/include/mysql")
include_directories("/usr/local/include/mysql")
include_directories(${Boost_INCLUDE_DIR})
link_directories("/usr/local/lib")

add_executable(${EXE_SERVER} server.cpp)
add_executable(${EXE_TESTS} tests.cpp)

foreach(EXE_NAME ${EXE_SERVER} ${EXE_TESTS}) # Сборка двух исполняемых файлов
    target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_BINARY_DIR}")
    target_compile_options(${EXE_NAME} PRIVATE)
    target_link_libraries(${EXE_NAME} PRIVATE 
                          ${CMAKE_THREAD_LIBS_INIT} 
                          ${Poco_LIBRARIES}
                          ${GTEST_LIBRARIES}
                          "PocoData"
                          "PocoDataMySQL"
                          "mysqlclient"
                          ZLIB::ZLIB)
    set_target_properties(${EXE_NAME} PROPERTIES LINKER_LANGUAGE CXX)
    set_target_properties(${EXE_NAME} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
    install(TARGETS ${EXE_NAME} RUNTIME DESTINATION bin)
endforeach()

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT satmak335@gmail.com)
include(CPack)