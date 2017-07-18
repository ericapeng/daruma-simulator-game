# The Darumaotoshi Simulator

A virtual simulation of the Japanese children's game, daruma otoshi. My final project for CSCI-GA 2270, NYU's Graduate Computer Graphics course of Fall 2016.

## Daruma Otoshi

Daruma Otoshi is a single-player game where the player is given a tower of cylindrical blocks and a hammer. The player then has to knock out all the blocks from underneath the top block one-by-one until the top block is on the table, without letting the tower fall over. [Here](https://www.youtube.com/watch?v=zlz0FY8XGig) is a video demonstration of how the game works. I have used the laws of physics and computer graphics to simulate this game.

## Game Details

The hammer is controlled by the mouse location. The speed of the moving mouse is calculated and used to determine the hammer’s force on a cylindrical block. Because the speed of the mouse is relative to the proportion of the tower displayed in the viewport, the possibility to zoom in and out has been added so that the user can have more control of the speed at which they hit the blocks.

### Game Controls

- Space Bar
    * Restarts the game
- Shift +
    * Zooms in
- Shift -
    * Zooms out
- C
    * Turns on cheat mode, in which every hammer swing will be the perfect amount of force from keeping the tower from toppling over.

### In Action

Starting State - blocks are in place and hammer has not yet hit any blocks
![start](/actionshots/start.png)

Zoomed State - a way to easily control the velocity of the hammer
![zoomed](/actionshots/zoomed.png)

Losing State - the top block changes faces when you've hit the bottom block too hard and the remaining blocks have toppled over
![loss](/actionshots/loss.png)

Winning State - you've worked hard to reach the last block, congrats! :)
![win](/actionshots/win.png)

## Computer Graphics Details

Used rasterization to render the scene. I used a darumaotoshi triangle mesh from Turbo Squid and then rendered the objects with texture mapping on the top block and hammer, and phong shading on the remaining blocks.

## Compilation Instructions

Try out the game! Just follow the directions below

```bash
git clone --recursive https://github.com/NYUCG2016/Assignment2
cd Assignment_2
mkdir build
cd build
cmake ../
make
./Assignment2_bin
```

