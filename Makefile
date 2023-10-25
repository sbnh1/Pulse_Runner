CC = gcc
CFLAGS = -Wall -W
LDFLAGS = 
SRC = main.c
OBJ = $(SRC:.c=.o)
EXE = main

all: $(SRC) $(EXE)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXE)