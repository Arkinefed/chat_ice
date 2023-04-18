.DEFAULT_GOAL := build

CXX = c++
CXXFLAGS = -std=c++17 -I. -DICE_CPP11_MAPPING
ICE_CPP_DIR = ice/cpp
LIBS = -lIce++11 -pthread
BIN_DIR = bin
BIN_INT_DIR = bin-int

SERVER_SRC = server.cpp $(ICE_CPP_DIR)/Chat2.cpp $(ICE_CPP_DIR)/Chat2I.cpp
CLIENT_SRC = client.cpp $(ICE_CPP_DIR)/Chat2.cpp $(ICE_CPP_DIR)/Chat2I.cpp

SERVER_OBJ = $(patsubst %.cpp,$(BIN_INT_DIR)/%.o,$(notdir $(SERVER_SRC)))
CLIENT_OBJ = $(patsubst %.cpp,$(BIN_INT_DIR)/%.o,$(notdir $(CLIENT_SRC)))

.PHONY: build
build: server client

.PHONY: server
server: $(BIN_DIR)/server

.PHONY: client
client: $(BIN_DIR)/client

$(BIN_DIR)/server: $(SERVER_OBJ) $(BIN_INT_DIR)/Chat2.o $(BIN_INT_DIR)/Chat2I.o
	mkdir -p $(BIN_DIR)
	$(CXX) -Wall -Wextra -o $@ $^ $(LIBS)

$(BIN_DIR)/client: $(CLIENT_OBJ) $(BIN_INT_DIR)/Chat2.o $(BIN_INT_DIR)/Chat2I.o
	mkdir -p $(BIN_DIR)
	$(CXX) -Wall -Wextra -o $@ $^ $(LIBS)

$(BIN_INT_DIR)/server.o: server.cpp
	mkdir -p $(BIN_INT_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BIN_INT_DIR)/client.o: client.cpp
	mkdir -p $(BIN_INT_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BIN_INT_DIR)/Chat2.o: $(ICE_CPP_DIR)/Chat2.cpp
	mkdir -p $(BIN_INT_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BIN_INT_DIR)/Chat2I.o: $(ICE_CPP_DIR)/Chat2I.cpp
	mkdir -p $(BIN_INT_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)/* $(BIN_INT_DIR)/*

.PHONY: ice
ice:
	mkdir -p $(ICE_CPP_DIR)
	slice2cpp --impl-c++11 --output-dir $(ICE_CPP_DIR) --depend-file ./ice/depend ./ice/Chat2.ice

.PHONY: cleani
cleanice:
	rm -rf $(ICE_CPP_DIR)/*

.PHONY: rc
rc: client
	$(BIN_DIR)/client

.PHONY: rs
rs: server
	$(BIN_DIR)/server
