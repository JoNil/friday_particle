all:
	g++ -std=c++14 -Wall friday_particle.cpp -o friday_particle -lGLEW -lGL -lglfw

clean:
	rm -f friday_particle

.PHONY: all clean