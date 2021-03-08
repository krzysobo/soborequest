##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR                       := /usr/bin/ar rcu
CXX                      := /usr/bin/g++
# can be set in the env. vars, ie. export CC='ccccc'
CC                       := /usr/bin/gcc
CXXFLAGS                 :=  -g -O0 -Wall -Werror
CFLAGS                   :=  -g -O0 -Wall -Werror -std=c89 -pedantic
CFLAGS_99                :=  -g -O0 -Wall -Werror -std=c99 -pedantic
ASFLAGS                  := 
AS                       := /usr/bin/as
# can be set in the env. vars, ie. export TARGET_PATH='xxxxx'
TARGET_PATH              ?= 'bin'

SOBOREQUEST_INSTALL_INCLUDE_PATH ?= '/usr/local/share/soborequest'
SOBOREQUEST_LIB_PATH     ?= '/usr/local/lib/soborequest'
SOBO_COMMON_INCLUDE_PATH ?= '/usr/local/share'   # upper directory - always for using in other projects
SOBOLOGGER_LIB_PATH      ?= '/usr/local/lib/sobologger'
SOBOUTILS_LIB_PATH       ?= '/usr/local/lib/soboutils'

OPENSSL_INCLUDE_PATH     ?= "/usr/local/Cellar/node/13.1.0/include/node"


.PHONY: clean

all: requestlib_static 
alltest: requestlib_static testrequest testsocket test_ssl


request: src/request.c
	$(CC) $(CFLAGS) -fpic -o $(TARGET_PATH)/request.o \
	-c src/request.c \
	-I include

	chmod 777 $(TARGET_PATH)/request.o


request_methods: src/request_methods.c
	$(CC) $(CFLAGS) -fpic -o $(TARGET_PATH)/request_methods.o \
	-c src/request_methods.c \
	-I include

	chmod 777 $(TARGET_PATH)/request_methods.o


request_cb_functions: src/request_cb_functions.c
	$(CC) $(CFLAGS) -fpic -o $(TARGET_PATH)/request_cb_functions.o \
	-c src/request_cb_functions.c \
	-I include

	chmod 777 $(TARGET_PATH)/request_cb_functions.o


socket_functions: src/socket_functions.c
	$(CC) $(CFLAGS) -fpic -o $(TARGET_PATH)/socket_functions.o \
	-c src/socket_functions.c \
	-I include \
	-I $(SOBO_COMMON_INCLUDE_PATH)

	chmod 777 $(TARGET_PATH)/socket_functions.o


ssl_functions: src/ssl_functions.c
	$(CC) $(CFLAGS_99) \
	-fpic -o $(TARGET_PATH)/ssl_functions.o \
	-c src/ssl_functions.c \
	-I include \
	-I $(SOBO_COMMON_INCLUDE_PATH) \
	`pkg-config --cflags openssl`

	chmod 777 $(TARGET_PATH)/ssl_functions.o


requestlib_static: request request_methods request_cb_functions \
socket_functions ssl_functions
	ar rcs $(TARGET_PATH)/libsoborequest.a \
	$(TARGET_PATH)/*.o

	rm -f $(TARGET_PATH)/*.o


testrequest: src/test/test_request.c
	$(CC) $(CFLAGS) -fpic -o bin/test_request \
	src/test/test_request.c \
	$(TARGET_PATH)/libsoborequest.a \
	-I include \
	-lcurl


testsocket: src/test/test_socket.c
	$(CC) $(CFLAGS) -fpic -o bin/test_socket \
	src/test/test_socket.c \
	$(TARGET_PATH)/libsoborequest.a \
	$(SOBOLOGGER_LIB_PATH)/libsobologger.a \
	-I include \
	-I $(SOBO_COMMON_INCLUDE_PATH) \
	-lcurl
	
test_ssl: src/test/test_ssl.c
	$(CC) $(CFLAGS_99) -fpic -o bin/test_ssl \
	src/test/test_ssl.c \
	$(TARGET_PATH)/libsoborequest.a \
	$(SOBOLOGGER_LIB_PATH)/libsobologger.a \
	-I include \
	-I $(SOBO_COMMON_INCLUDE_PATH) \
	`pkg-config --cflags openssl` \
	`pkg-config --libs openssl`


install:
	@echo "\nThis operation requires the SUDO access.\n"
	sudo mkdir -p $(SOBOREQUEST_LIB_PATH)
	sudo mkdir -p $(SOBOREQUEST_INSTALL_INCLUDE_PATH)
	sudo cp -rf $(TARGET_PATH)/libsoborequest.a $(SOBOREQUEST_LIB_PATH)/
	sudo cp -rf include/soborequest/* $(SOBOREQUEST_INSTALL_INCLUDE_PATH)/

clean:
	rm -rf *.o
	rm -rf $(TARGET_PATH)/*
