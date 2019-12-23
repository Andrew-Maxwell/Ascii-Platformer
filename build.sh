set -e    # if error, stop the build
g++ -g -c -I /opt/raylib/src platformerRaylib.cpp -o platformer.o -O3
g++ -g -o platformer platformer.o -Wall -I/opt/raylib/src -L/opt/raylib/release/libs/linux -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -O3
./platformer
