#set -e    # if error, stop the build
g++ -g -c -I /opt/raylib/src meta.cpp -o meta.o -O3
g++ -g -c -I /opt/raylib/src entitymeta.cpp -o entitymeta.o -O3
g++ -g -c -I /opt/raylib/src layer.cpp -o layer.o -O3
g++ -g -c -I /opt/raylib/src dummyentity.cpp -o dummyentity.o -O3
g++ -g -c -I /opt/raylib/src editables.cpp -o editables.o -O3
g++ -g -c -I /opt/raylib/src charfills.cpp -o charfills.o -O3
g++ -g -c -I /opt/raylib/src editor_main.cpp -o editor_main.o -O3
g++ -g -o editor editor_main.o meta.o entitymeta.o layer.o dummyentity.o editables.o charfills.o -Wall -I/opt/raylib/src -L/opt/raylib/release/libs/linux -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -O3
