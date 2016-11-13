OBJS = Complex.o Signal.o SocketManager.o SoundPlayer.o main.o
CC = g++
CFLAGS = -std=c++11 -g -Wall -c -O3
LFLAGS = -Wall

airwaves_fm : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o airwaves_fm -lpthread -lpulse-simple

Complex.o : Complex.h Complex.cpp
	$(CC) $(CFLAGS) Complex.cpp

Signal.o : Signal.h Signal.cpp
	$(CC) $(CFLAGS) Signal.cpp

SocketManager.o : SocketManager.h SocketManager.cpp
	$(CC) $(CFLAGS) SocketManager.cpp

SoundPlayer.o : SoundPlayer.h SoundPlayer.cpp
	$(CC) $(CFLAGS) SoundPlayer.cpp -lpulse-simple

main.o : main.cpp
	$(CC) $(CFLAGS) main.cpp -lpthread

clean :
	\rm *.o *~ airwaves_fm core
