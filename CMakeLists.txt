cmake_minimum_required(VERSION 3.10)
project(ZeronDD LANGUAGES C VERSION 0.0.0.1)

add_subdirectory(curl)


add_executable(ZeronDD 
	src/main.c
	src/proxy.c
	src/http.c
)
target_compile_options(ZeronDD PRIVATE
	$<$<NOT:$<CONFIG:RELEASE>>:
		-std=c89 -pedantic -Wall 
    	-Wno-missing-braces -Wextra -Wno-missing-field-initializers -Wformat=2 
    	-Wswitch-default -Wswitch-enum -Wcast-align -Wpointer-arith 
    	-Wbad-function-cast -Wstrict-overflow=5 -Wstrict-prototypes -Winline 
    	-Wundef -Wnested-externs -Wcast-qual -Wshadow -Wunreachable-code 
    	-Wlogical-op -Wfloat-equal -Wstrict-aliasing=2 -Wredundant-decls 
    	-Wold-style-definition 
    	-ggdb3 
    	-O0 
    	-fno-omit-frame-pointer -ffloat-store -fno-common -fstrict-aliasing 
    	-lm
    >
    $<$<CONFIG:RELEASE>:
    	-lm -std=c89 -pedantic -O3 -DNDEBUG
     	-funroll-loops
    >
)
target_link_libraries(ZeronDD PUBLIC 
	libcurl
)

target_include_directories(ZeronDD PRIVATE ${CURL_INCLUDE_DIRS})