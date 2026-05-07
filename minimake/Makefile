TARGET = minimake
SRC = basics.c
OBJ = basics.o
CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=address -g
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(CFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
