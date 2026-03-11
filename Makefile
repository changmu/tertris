UNAME_S  := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
  CXX := clang++
else
  CXX := g++
endif
CXXFLAGS := -std=c++23 -Wall -Wextra -O2
LDFLAGS  := -lncurses

SRC_DIR := src
SRCS    := $(wildcard $(SRC_DIR)/*.cpp)
OBJS    := $(SRCS:.cpp=.o)
TARGET  := tetris

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(SRC_DIR)/*.o $(TARGET)
