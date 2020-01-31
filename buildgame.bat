IF EXIST meta.o del /F meta.o
IF EXIST game_meta_classes.o del /F game_meta_classes.o
IF EXIST game_classes.o del /F game_classes.o
IF EXIST game_main.o del /F game_main.o
IF EXIST game.exe del /F game.exe

g++ -g -c -I /opt/raylib/src meta.cpp -o meta.o -O3 -Wall
g++ -g -c -I /opt/raylib/src game_meta_classes.cpp -o game_meta_classes.o -O3 -Wall
g++ -g -c -I /opt/raylib/src game_classes.cpp -o game_classes.o -O3 -Wall
g++ -g -c -I /opt/raylib/src game_main.cpp -o game_main.o -O3 -Wall
g++ -g -o game.exe meta.o game_meta_classes.o game_classes.o game_main.o -Wall -I/opt/raylib/src -L/opt/raylib/release/libs/linux -O3 -lraylib -lopengl32 -lgdi32 -lwinmm -Iexternal -lwinpthread -static-libgcc -static-libstdc++

IF EXIST game.exe game.exe

PAUSE