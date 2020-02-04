set -e    # if error, stop the build
g++ -g -c -I /opt/raylib/src meta.cpp -o meta.o -O3
g++ -g -c -I /opt/raylib/src game_meta_classes.cpp -o game_meta_classes.o -O3
g++ -g -c -I /opt/raylib/src game_classes.cpp -o game_classes.o -O3
g++ -g -c -I /opt/raylib/src game_main.cpp -o game_main.o -O3
g++ -g -o platformer meta.o game_meta_classes.o game_classes.o game_main.o -Wall -I/opt/raylib/src -L/opt/raylib/release/libs/linux -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -O3
./platformer
