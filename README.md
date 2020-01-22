# Ascii-Platformer
WIP ASCII platformer using Raylib. Design emphasis on beatiful, atmospheric levels using simple, minimalistic elements,

Here's how I (Andrew) explained the technical side of it to Mark. Writing a more polished readme takes a back seat to actually implementing more features.

So basically I wrote this almost from scratch
it's not using an engine
It's using Raylib, which handles key presses and displaying fonts, and nothing else - hence the name, although it uses Unicode really
Originally it was going to use ncurses and a console interface but that was dumb
because the console doesn't handle multiple key presses well
Let's see
So it's all written in C++
The world consists of "entities"
Each entity implements a common interface, with functions called "tickset", "tickget", "finalize", and "print"
Entities are held in entityLists which also implement those functions. really simple
Also entities can have entitylists, for example, you can have a bullet hit a wall and then spawn particles, and the particles are entities in an entitylist inside the bullet
In each tick tickset() is called on everything, then tickget(), then finalize(), then print()
In tickset() entities do most of the processing, and they also write data to the communication interface, called the collider
In tickget() they respond to any data they get from the collider
finalize() just returns true if the entity should be deleted by the entitylist
and print() displays the entities to screen
Not all entities are collideable
The ones that are, must implement an additional interface, collideable
which has the functions doesCollide(), getCollisions(), and stopColliding()
The collider uses doesCollide to determine whether an object collides with another. It will call entity A's doesCollide() with entity B's coordinates, and if it returns true, then it passes a collision object from entity A using getCollision() to entity B using addCollision()
Then it calls stopColliding() to determine whether the object should be deleted. stopColliding() and finalize() should always return true at the same time.
And some entities can be collided with but can't collide. For example, particles can be affected by other objects but can't affect other objects
Oh, and the collider also has data on which tiles in the world are solid
The world visually consists of layers, which are just entities that are extremely large (visually) and don't really do anything
Each layer can only be one color
and the data for each layer is stored in a text file
Also, all layers and objects can be drawn with a different sizeFactor, which allows you to create semi-3D stuff
But collisions always happen in 2D
sizeFactor is essentially just how zoomed in it is
And then there are various interactive objects
The main things I've implemented so far are doors, save points, and force fields
Force fields will attract/repel you, bullets, and whatever else when you're close to them
when you hold shift
I think there are some interesting platforming puzzle possibilities using forcefields
Like using forcefields to fly really high, or using them to bend the path of a bullet to fly around corners
We can also make things like switches that trigger a forcefield when you shoot them
I was also thinking of adding in the bitwise puzzle system
Like, as you moved through a level you would pick up different bitwise operators, and then you could press a key to broadcast the current register status, and if the byte matched a receiver then the receiver would trigger
