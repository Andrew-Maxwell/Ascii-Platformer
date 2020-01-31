IF EXIST editor.exe del /F editor.exe
IF EXIST meta.o del /F meta.o
IF EXIST editor_classes.o del /F editor_classes.o
IF EXIST editor_main.o del /F editor_main.o

g++ -g -c -I /opt/raylib/src meta.cpp -o meta.o -O3 -Wall
g++ -g -c -I /opt/raylib/src editor_classes.cpp -o editor_classes.o -O3 -Wall
g++ -g -c -I /opt/raylib/src editor_main.cpp -o editor_main.o -O3 -Wall
g++ -g -o editor.exe meta.o editor_classes.o editor_main.o -Wall -I/opt/raylib/src -L/opt/raylib/release/libs/linux -O3 -lraylib -lopengl32 -lgdi32 -lwinmm -Iexternal -lwinpthread -static-libgcc -static-libstdc++ -Wl,--subsystem,windows

IF EXIST editor.exe editor.exe test.txt

PAUSE