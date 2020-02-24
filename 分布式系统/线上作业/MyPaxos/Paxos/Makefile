DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin

SRC = $(wildcard ${DIR_SRC}/*.cpp) $(wildcard ${DIR_SRC}/lib/*.cpp) $(wildcard ${DIR_SRC}/Paxos/*.cpp)
OBJ = $(patsubst %.cpp,${DIR_OBJ}/%.o,$(notdir ${SRC})) 

TARGET = Paxos_Test

BIN_TARGET = ${DIR_BIN}/${TARGET}

CC = g++
CFLAGS = -lpthread -lrt

${BIN_TARGET}:${OBJ}
	$(CC) $(OBJ) $(CFLAGS) -o $@

${DIR_OBJ}/Paxos.o:${DIR_SRC}/Paxos.cpp 
	$(CC) -c $^ -o $@

${DIR_OBJ}/atom.o:${DIR_SRC}/lib/atom.cpp 
	$(CC) -c $^ -o $@
${DIR_OBJ}/Executor.o:${DIR_SRC}/lib/Executor.cpp 
	$(CC) -c $^ -o $@
${DIR_OBJ}/Lock.o:${DIR_SRC}/lib/Lock.cpp 
	$(CC) -c $^ -o $@
${DIR_OBJ}/Logger.o:${DIR_SRC}/lib/Logger.cpp 
	$(CC) -c $^ -o $@
${DIR_OBJ}/mapi.o:${DIR_SRC}/lib/mapi.cpp 
	$(CC) -c $^ -o $@
${DIR_OBJ}/Task.o:${DIR_SRC}/lib/Task.cpp 
	$(CC) -c $^ -o $@
${DIR_OBJ}/Thread.o:${DIR_SRC}/lib/Thread.cpp 
	$(CC) -c $^ -o $@

${DIR_OBJ}/Acceptor.o:${DIR_SRC}/Paxos/Acceptor.cpp
	$(CC) -c $^ -o $@
${DIR_OBJ}/Proposer.o:${DIR_SRC}/Paxos/Proposer.cpp
	$(CC) -c $^ -o $@

.PHONY:clean
clean:
	rm -rf ${DIR_OBJ}/*.o
	rm -rf ${DIR_BIN}/*
