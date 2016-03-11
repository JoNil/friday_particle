all:
	g++ -Wall friday_particle.cpp -o friday_particle -lGL -lglfw

clean:
	rm -f friday_particle

.PHONY: all clean