# Ascii-Platformer
WIP platformer using Raylib. All of the graphics are composed of overlapping Unicode characters in
different colors and sizes, inspired by ASCII art; hence the title.

![snow.json level at dawn](images/snowjsondawn.png)
![dark.json level](images/darkjson.png)
![matrix-looking level](images/matrix.png)
![options](images/options.png)
![enter name](images/entername.png)

## Gameplay
The final vision for gameplay can be thought of as having three primary components: Combat, 
platforming, and puzzles.

Combat is fairly standard: the player can find and carry multiple guns with
diverse stats (not hard-coded; each gun is specified in the level file) and use them to shoot enemies.

Early in the game, platforming is also fairly standard for a sidescroller; anyone familiar 
with Cave Story will find it mostly familiar, except for the forcefields, which push, pull, 
and fling the player in various directions, and must sometimes be used in addition to just 
jumping between platforms to reach new areas. Later in the game, other avatars or "outfits" are unlocked which
have different abilities or weaknesses, such as outfits that can fly like quadcopters, or which
bounce like in *Within a Deep Forest* by Nifflas, or which can climb vertical walls like in Celeste.
Like the guns, the parameters of any number of outfits can be defined in the level files.

The puzzles consist of using a limited set of bitwise operations, such as AND, OR, XOR, or 
shift, to change a variable's value. Certain special values of the variable can be used like a 
password; when a key is pressed to broadcast them through the level, they activate certain 
level elements, such as forcefields or doors. In effect, the player is "hacking" the level (in 
fact, this mechanic is inspired by how easily certain wireless garage doors can be manipulated 
in a similar way.) These level elements can also be combined to make their own type of 
puzzles, similar to *Portal* or *The Talos Principle* in that each element is well-defined and 
predictable, and the challenge lies in finding out how to combine them.

Obviously, each of these elements can be blended together; for example, solving bit puzzles in order to
be able to control a forcefield which repels an enemy's bullets away from you.

## Under the hood

The project relies on Raylib for handling basic graphical effects, as well as keyboard, mouse, 
and gamepad input. Also, RapidJSON is used for handling save, level, and configuration files.

Maybe I'll do a detailed writeup here some other time; right now it's a solo project, and I'm
working on it frequently enough that I'm not forgetting details.
