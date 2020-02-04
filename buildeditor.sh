set -e    # if error, stop the build
g++ -g -c -I /opt/raylib/src meta.cpp -o meta.o -O3
g++ -g -c -I /opt/raylib/src editor_classes.cpp -o editor_classes.o -O3
g++ -g -c -I /opt/raylib/src editor_main.cpp -o editor_main.o -O3
g++ -g -o editor editor_main.o editor_classes.o meta.o -Wall -I/opt/raylib/src -L/opt/raylib/release/libs/linux -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -O3
