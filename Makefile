all:
	g++ -o game main.cpp -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm
	.\game
debug:
	g++ -g -o platformer main.cpp