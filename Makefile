CXX=g++
LD=${CXX}

BASE_DIR= .
BASE_INC= ${BASE_DIR}/include

INC= -I${BASE_INC} 

LIBDIR=
LIB= -lpthread

CFLAG= ${INC} -g -O2 -Wall -Wextra -DNDEBUG
LDFLAG= ${LIBDIR} ${LIB}

$(shell if [ ! -d bin ]; then mkdir bin; fi)
$(shell if [ ! -d compile ]; then mkdir compile; fi)

TARGET=bin/bullbench
SRCS=$(wildcard src/*.cpp)
SRCSNOTDIR=$(notdir $(SRCS))
OBJS=$(patsubst %.cpp, compile/%.o, $(SRCSNOTDIR)) 

.PHONY: clean test

$(TARGET):$(OBJS)
	${LD} -o $@ $^ ${LDFLAG}

compile/%.o:src/%.cpp
	${CXX} -o $@ -c $< ${CFLAG} 

clean :
	rm -rf $(TARGET) $(OBJS) 

