gcc sound_of_circle.cpp -framework IOKit -framework Cocoa -framework OpenGL `pkg-config --libs --cflags raylib` -o sound_of_circle --std=c++11