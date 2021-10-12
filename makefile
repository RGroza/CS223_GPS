CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -g3

GPS: GPS.o more_math.o location.o trackpoint.o track.o
	gcc -o $@ -g $^ -lm

TrackUnit: track_unit.o more_math.o location.o trackpoint.o track.o
	gcc -o $@ -g $^ -lm

GPS.o: location.h trackpoint.h track.h
track_unit.o: track.h
trackpoint.o: location.h trackpoint.h
more_math.o: more_math.h
location.o: location.h more_math.h
track.o: location.h trackpoint.h track.h
