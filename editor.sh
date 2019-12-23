set -e    # if error, stop the build
g++ -g -c -I /opt/raylib/src platformer.cpp -o platformer.o -O3
g++ -g -c -I /opt/raylib/src platformer_editor.cpp -o platformer_editor.o -O3
g++ -g -o editor platformer.o platformer_editor.o -Wall -I/opt/raylib/src -L/opt/raylib/release/libs/linux -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -O3
