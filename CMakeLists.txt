cmake_minimum_required(VERSION 3.24)
project(bookish_eureka_tcp C)

set(CMAKE_C_STANDARD 23)

add_executable(httpserver exercies/httpServer_7.c exercies/DieWithMessage.c)
add_executable(httpserverfile exercies/httpServerFile_8.c exercies/DieWithMessage.c)
add_executable(httpserverhostname exercies/AddressUtility.c exercies/basic_http_client_hostname.c  exercies/DieWithMessage.c)
