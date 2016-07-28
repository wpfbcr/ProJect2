ROOT_PATH=$(shell pwd)
LDFLAGS= -lpthread -static
FLAGS=#-D_DEBUG_-g
CC=gcc
BIN=httpd


SRC=$(shell ls *.c)
OBJ=$(SRC:.c=.o)

$(BIN):$(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
%.o:%.c
	@$(CC) -c $< $(FLAGS)

.PHONY:clean
clean:
	@rm -rf *.o $(BIN)

	
	








