gcc -Wall -o explorers.exe^
 ./code/*.c^
 -I "C:\msys64\mingw64\include\SDL2"^
 -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_image^
 -Wl,-Bstatic sdl-config --static-libs -Wl,-Bdynamic^
 -O2