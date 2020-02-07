CXX=g++
CXXFLAGS=

game: bullet.o col.o door.o effects.o endinggate.o entitymeta.o forcefield.o gamelayer.o game_main.o layer.o meta.o particles.o physicalentities.o pickups.o player.o rain.o savepoint.o
	g++ -g -o platformer game_main.o bullet.o col.o door.o effects.o endinggate.o entitymeta.o forcefield.o gamelayer.o layer.o meta.o particles.o physicalentities.o pickups.o player.o rain.o savepoint.o -Wall -I/opt/raylib/src -L/opt/raylib/release/libs/linux -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -O3

game_main.o:
	g++ -g -c -I /opt/raylib/src game_main.cpp -o game_main.o -O3

bullet.o:
	g++ -g -c -I /opt/raylib/src bullet.cpp -o bullet.o -O3

col.o:
	g++ -g -c -I /opt/raylib/src col.cpp -o col.o -O3

door.o:
	g++ -g -c -I /opt/raylib/src door.cpp -o door.o -O3

effects.o:
	g++ -g -c -I /opt/raylib/src effects.cpp -o effects.o -O3

endinggate.o:
	g++ -g -c -I /opt/raylib/src endinggate.cpp -o endinggate.o -O3

entitymeta.o:
	g++ -g -c -I /opt/raylib/src entitymeta.cpp -o entitymeta.o -O3

forcefield.o:
	g++ -g -c -I /opt/raylib/src forcefield.cpp -o forcefield.o -O3

gamelayer.o:
	g++ -g -c -I /opt/raylib/src gamelayer.cpp -o gamelayer.o -O3

layer.o:
	g++ -g -c -I /opt/raylib/src layer.cpp -o layer.o -O3

meta.o:
	g++ -g -c -I /opt/raylib/src meta.cpp -o meta.o -O3

particles.o:
	g++ -g -c -I /opt/raylib/src particles.cpp -o particles.o -O3

physicalentities.o:
	g++ -g -c -I /opt/raylib/src physicalentities.cpp -o physicalentities.o -O3

pickups.o:
	g++ -g -c -I /opt/raylib/src pickups.cpp -o pickups.o -O3

player.o: player.cpp
	g++ -g -c -I /opt/raylib/src player.cpp -o player.o -O3

rain.o:
	g++ -g -c -I /opt/raylib/src rain.cpp -o rain.o -O3

savepoint.o:
	g++ -g -c -I /opt/raylib/src savepoint.cpp -o savepoint.o -O3

editor:
	g++ -g -o editor editor_main.o meta.o entitymeta.o layer.o dummyentity.o editables.o charfills.o -Wall -I/opt/raylib/src -L/opt/raylib/release/libs/linux -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -O3

dummyentity.o:
	g++ -g -c -I /opt/raylib/src dummyentity.cpp -o dummyentity.o -O3

editables.o:
	g++ -g -c -I /opt/raylib/src editables.cpp -o editables.o -O3

charfills.o:
	g++ -g -c -I /opt/raylib/src charfills.cpp -o charfills.o -O3

editor_main.o:
	g++ -g -c -I /opt/raylib/src editor_main.cpp -o editor_main.o -O3

clean:
	rm *.o editor platformer
