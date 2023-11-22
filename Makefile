CC = gcc
CFLAGS = -W -Wall -ansi -std=c99 -g
LIBS = -L./SDL2_tff/.libs
LDFLAGS = `sdl2-config --cflags --libs` -lSDL2_ttf	-lSDL2_image -lSDL2_mixer
INCLUDES = -I./SDL2_tff
EXEC = main
SRC = main.c fonctions_SDL.c
OBJ = $(SRC:.c=.o)
all: $(EXEC)
main: $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIBS) $(LDFLAGS)
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<
clean:
	rm -rf *.o *~
mrproper: clean
	rm -rf $(EXEC)

