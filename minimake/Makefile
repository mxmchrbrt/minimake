TARGET = minimake
SRC = basics.c parser.c
OBJ = basics.o parser.o
CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=address -g

all: ${TARGET}

${TARGET}: ${OBJ}
	${CC} ${OBJ} ${CFLAGS} -o ${TARGET}

${OBJ}: ${SRC}
	${CC} ${CFLAGS} -c ${SRC}

clean-all:
	rm -f ${OBJ} ${TARGET}

clean:
	rm -f ${OBJ}
