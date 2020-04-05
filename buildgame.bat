IF EXIST bullet.o del /F bullet.o
IF EXIST charfills.o del /F charfills.o
IF EXIST world.o del /F world.o
IF EXIST door.o del /F door.o
IF EXIST effects.o del /F effects.o
IF EXIST endinggate.o del /F endinggate.o
IF EXIST entity.o del /F entity.o
IF EXIST forcefield.o del /F forcefield.o
IF EXIST gamelayer.o del /F gamelayer.o
IF EXIST game_main.o del /F game_main.o
IF EXIST layer.o del /F layer.o
IF EXIST meta.o del /F meta.o
IF EXIST particles.o del /F particles.o
IF EXIST physicalentities.o del /F physicalentities.o
IF EXIST pickups.o del /F pickups.o
IF EXIST player.o del /F player.o
IF EXIST rain.o del /F rain.o
IF EXIST savepoint.o del /F savepoint.o
IF EXIST water.o del /F water.o
IF EXIST canvas.o del /F canvas.o
IF EXIST data.o del /F data.o
IF EXIST gameleveldata.o del /F gameleveldata.o
IF EXIST savedata.o del /F savedata.o
IF EXIST game.exe del /F game.exe


g++ -g -c -I /opt/raylib/src meta.cpp -o meta.o -O3 -Wall
g++ -g -c -I /opt/raylib/src game_meta_classes.cpp -o game_meta_classes.o -O3 -Wall
g++ -g -c -I /opt/raylib/src game_classes.cpp -o game_classes.o -O3 -Wall
g++ -g -c -I /opt/raylib/src game_main.cpp -o game_main.o -O3 -Wall
g++ -g -o game.exe meta.o game_meta_classes.o game_classes.o game_main.o -Wall -I/opt/raylib/src -L/opt/raylib/release/libs/linux -O3 -lraylib -lopengl32 -lgdi32 -lwinmm -Iexternal -lwinpthread -static-libgcc -static-libstdc++

IF EXIST game.exe game.exe

PAUSE