CC = gcc
CFLAGS = -ansi -std=c99 -g
LIBS = -L./SDL2_tff/.libs
LDFLAGS = `sdl2-config --cflags --libs` -lSDL2_ttf -lSDL2_image -lSDL2_mixer
INCLUDES = -I./SDL2_tff -I./include
EXEC = main
SRC_DIR = src
SRC = $(SRC_DIR)/main.c $(SRC_DIR)/fonctions_SDL.c $(SRC_DIR)/utils.c
OBJ = $(SRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIBS) $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

clean:
	rm -rf $(SRC_DIR)/*.o *~

mrproper: clean
	rm -rf $(EXEC)
