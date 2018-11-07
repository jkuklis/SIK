COMP = g++ -O2 -Wall -std=c++14

LINE = $(COMP) $^ -o $@

all: siktacka-server siktacka-client

siktacka-server: siktacka-server.cc server_input.o input.o server_establish.o \
		communication_server.o game_logic.o
	$(LINE)

server_input.o: siktacka-input-server.cc
	$(LINE) -c

server_establish.o: siktacka-establish-server.cc
	$(LINE) -c

game_logic.o: siktacka-game-logic.cc
	$(LINE) -c


siktacka-client: siktacka-client.cc client_input.o input.o client_establish.o \
		communication_server.o communication_gui.o server_input.o
	$(LINE)

client_input.o: siktacka-input-client.cc
	$(LINE) -c

client_establish.o: siktacka-establish-client.cc
	$(LINE) -c

communication_gui.o: siktacka-communication-gui.cc
	$(LINE) -c


input.o: siktacka-input.cc
	$(LINE) -c

communication_server.o: siktacka-communication-server.cc
	$(LINE) -c


clean:
	rm -f *.o server client
