#set -e    # if error, stop the build
g++ -g -c -I /opt/raylib/src game_main.cpp -o game_main.o -O3
g++ -g -c -I /opt/raylib/src bullet.cpp -o bullet.o -O3
g++ -g -c -I /opt/raylib/src col.cpp -o col.o -O3
g++ -g -c -I /opt/raylib/src door.cpp -o door.o -O3
g++ -g -c -I /opt/raylib/src effects.cpp -o effects.o -O3
g++ -g -c -I /opt/raylib/src endinggate.cpp -o endinggate.o -O3
g++ -g -c -I /opt/raylib/src entitymeta.cpp -o entitymeta.o -O3
g++ -g -c -I /opt/raylib/src forcefield.cpp -o forcefield.o -O3
g++ -g -c -I /opt/raylib/src gamelayer.cpp -o gamelayer.o -O3
g++ -g -c -I /opt/raylib/src layer.cpp -o layer.o -O3
g++ -g -c -I /opt/raylib/src meta.cpp -o meta.o -O3
g++ -g -c -I /opt/raylib/src particles.cpp -o particles.o -O3
g++ -g -c -I /opt/raylib/src physicalentities.cpp -o physicalentities.o -O3
g++ -g -c -I /opt/raylib/src pickups.cpp -o pickups.o -O3
g++ -g -c -I /opt/raylib/src player.cpp -o player.o -O3
g++ -g -c -I /opt/raylib/src rain.cpp -o rain.o -O3
g++ -g -c -I /opt/raylib/src savepoint.cpp -o savepoint.o -O3
g++ -g -o platformer game_main.o bullet.o col.o door.o effects.o endinggate.o entitymeta.o forcefield.o gamelayer.o layer.o meta.o particles.o physicalentities.o pickups.o player.o rain.o savepoint.o -Wall -I/opt/raylib/src -L/opt/raylib/release/libs/linux -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -O3
./platformer
