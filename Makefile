CXX=g++
CXXLD=${CXX} -o 
BASE_DIR= .
BASE_INC= ${BASE_DIR}/include

INC= -I${BASE_INC} 

LIBDIR=

LIB= -lpthread

FLAG= ${LIBDIR} ${LIB} ${INC} -g -O2 -Wall -Wextra -DNDEBUG

$(shell if [ ! -d bin ]; then mkdir bin; fi)
$(shell if [ ! -d compile ]; then mkdir compile; fi)

TARGET=bin/bullbench
SRCS=$(wildcard src/*.cpp)
SRCSNOTDIR=$(notdir $(SRCS))
OBJS=$(patsubst %.cpp, compile/%.o, $(SRCSNOTDIR)) 

.PHONY: clean test

$(TARGET):$(OBJS)
	${CXXLD} $@ $^ ${FLAG} 

compile/%.o:src/%.cpp
	${CXX} -o $@ -c $< ${FLAG} 

clean :
	rm -rf $(TARGET) $(OBJS) 

