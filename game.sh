set -e    # if error, stop the build
g++ -g -c -I /opt/raylib/src platformer.cpp -o platformer.o -O3
g++ -g -c -I /opt/raylib/src platformer_game_classes.cpp -o platformer_game_classes.o -O3
g++ -g -c -I /opt/raylib/src platformer_game.cpp -o platformer_game.o -O3
g++ -g -o platformer platformer.o platformer_game_classes.o platformer_game.o -Wall -I/opt/raylib/src -L/opt/raylib/release/libs/linux -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -O3
./platformer
