CC = gcc
CXX = g++

OBJECTS := main.o src/HTTPClient.o src/HuaweiModem.o src/cJSON.o src/RFProtocol.o src/ProtocolHandler.o src/Device.o

# libRF24 Core(SHMS)
OBJECTS += src/spi.o src/nRF24L01.o src/bcm2835.o
# OBJECTS += src/SockRF.o

C_OBJ := $(OBJECTS:%.o=obj/%.o)
CFLAGS = -lpthread -pthread -Ofast -Wno-all

world: $(OBJECTS) hub-daemon

hub-daemon:
	# Compiling main hub_daemon...
	$(CXX) $(CFLAGS) -o hub_daemon $(C_OBJ)

%.o: %.cpp
	# Compiling $< to object file...
	$(CXX) $(CFLAGS) -g -O -c $< -fPIC -o obj/$@ -Iinclude

%.o: %.c
	# Compiling $< to object file...
	$(CC) $(CFLAGS) -g -O -c $< -fPIC -o obj/$@ -Iinclude

clean:
	rm -rf obj/*.o *.o hub_daemon

modem-test: modem_test.o src/HuaweiModem.o
	$(CXX) $(CFLAGS) -o modem-test obj/modem_test.o obj/src/HuaweiModem.o