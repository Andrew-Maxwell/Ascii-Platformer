set -e    # if error, stop the build
g++ -g -c -I /opt/raylib/src -Wall game_main.cpp -o game_main.o -O3
g++ -g -c -I /opt/raylib/src -Wall bullet.cpp -o bullet.o -O3
g++ -g -c -I /opt/raylib/src -Wall col.cpp -o col.o -O3
g++ -g -c -I /opt/raylib/src -Wall door.cpp -o door.o -O3
g++ -g -c -I /opt/raylib/src -Wall effects.cpp -o effects.o -O3
g++ -g -c -I /opt/raylib/src -Wall endinggate.cpp -o endinggate.o -O3
g++ -g -c -I /opt/raylib/src -Wall entitymeta.cpp -o entitymeta.o -O3
g++ -g -c -I /opt/raylib/src -Wall forcefield.cpp -o forcefield.o -O3
g++ -g -c -I /opt/raylib/src -Wall gamelayer.cpp -o gamelayer.o -O3
g++ -g -c -I /opt/raylib/src -Wall layer.cpp -o layer.o -O3
g++ -g -c -I /opt/raylib/src -Wall meta.cpp -o meta.o -O3
g++ -g -c -I /opt/raylib/src -Wall particles.cpp -o particles.o -O3
g++ -g -c -I /opt/raylib/src -Wall physicalentities.cpp -o physicalentities.o -O3
g++ -g -c -I /opt/raylib/src -Wall pickups.cpp -o pickups.o -O3
g++ -g -c -I /opt/raylib/src -Wall player.cpp -o player.o -O3
g++ -g -c -I /opt/raylib/src -Wall rain.cpp -o rain.o -O3
g++ -g -c -I /opt/raylib/src -Wall savepoint.cpp -o savepoint.o -O3
g++ -g -c -I /opt/raylib/src -Wall water.cpp -o water.o -O3
g++ -g -o platformer game_main.o bullet.o col.o door.o effects.o endinggate.o entitymeta.o forcefield.o gamelayer.o layer.o meta.o particles.o physicalentities.o pickups.o player.o rain.o savepoint.o water.o -Wall -I/opt/raylib/src -L/opt/raylib/release/libs/linux -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -O3
./platformer
