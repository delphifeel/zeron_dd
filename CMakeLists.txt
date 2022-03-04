cmake_minimum_required(VERSION 3.10)
project(ZeronDD LANGUAGES C VERSION 0.0.0.1)
add_compile_options(
	-Wall 
	-pedantic 
	--std=gnu99
)
#set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(curl)


add_executable(ZeronDD 
	src/main.c
	src/proxy.c
	src/http.c
)
target_link_libraries(ZeronDD PUBLIC 
	libcurl
)

target_include_directories(ZeronDD PRIVATE ${CURL_INCLUDE_DIRS})