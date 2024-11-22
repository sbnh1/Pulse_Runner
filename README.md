# Pulse Runner

Pulse Runner is a [Geometry Dash](https://en.wikipedia.org/wiki/Geometry_Dash)-inspired video game developed in C.

## Compilation Instructions (Ubuntu/Debian)

These instructions are intended for Ubuntu and Debian-based systems. If you're using a different operating system (such as Windows), you may need to adjust the process accordingly.

If you're using Ubuntu or Debian, you can follow these steps to compile and run the game:

1. **Install the necessary dependencies**:
   Run the following command to install the required libraries:

   ```bash
   sudo apt-get install build-essential libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev
   ```
2. **Compile the game**:
    Navigate to the project directory and run the make command to compile the game:

   ```bash
   make
   ```
3. **Run the game**:
    After the compilation, you can start the game by executing:

    ```bash
    ./main
    ```

## Game User Manual

Pulse Runner is a "die and retry" platformer inspired by Geometry Dash. In the game, you control a cube that moves horizontally to the right, encountering obstacles and spikes that you must avoid throughout each level. The goal is to jump over these obstacles and survive.

When you launch the game, you will land on the main menu where you can choose to press the "shop" button, the "play" button, or press "q" to quit the game (this feature is available from all other menus in the game). 

- The **"shop"** is where you can choose your cube's skin. Initially, three skins are available, and you can unlock three additional skins by playing the game.
- After the **"shop"**, you can press the **"play"** button, which leads you to the level selection menu. Here, you can choose from three available levels or go back using the button in the top left corner. Completing a level will unlock a new skin for your cube.
  
Once you're in the level selection menu, choose the level you want to play. The easiest level is on the left, and the hardest is on the right. In each level, your only action is to press the **SPACE** bar to jump and survive. If you get frustrated with dying repeatedly, you can press **m** to return to the level selection menu.

If you manage to complete a level, you'll be taken to a new page that shows you the skin you used to complete the level and displays the new skin you unlocked (if it's your first time finishing the level). On this page, there are two buttons:
- The **"play"** button takes you back to the level selection menu.
- The **"shop"** button directs you to the shop where you can choose your new unlocked skin.

## Data

You can modify the `data` file to unlock skins by changing the 0s to 1s (and vice versa) for skins that you can unlock by completing a level. You can also use this to lock skins again by changing 1 to 0.

Additionally, you can edit the `map1`, `map2`, and `map3` files in the `maps` folder to create your own levels.

### Explanation of the Files in the Maps Folder

- **Blocks:**
  - `1` = Block where you can walk on.
  - `2` = Spikes (hazards).
  - `3` = Level-ending blocks.

## Issues

**Disclaimer**: This game is not perfect and may contain some issues. After playing several maps, you might experience problems with collisions on specific blocks, which may cause you to die in an endless loop.
