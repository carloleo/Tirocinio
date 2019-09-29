
project(Server C)

set(CMAKE_C_STANDARD 99)



add_executable(Server Server.c rsa.c utility.c)
add_executable(Client Client.c rsa.c utility.c)

add_library(RSA  rsa.c rsa.h)
add_library(Utility utility.c utility.h)
