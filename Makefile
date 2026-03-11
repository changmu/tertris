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

PWD      := $(shell pwd)

.PHONY: all clean compile_commands.json

all: $(TARGET) compile_commands.json

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

CXXFLAGS_JSON := $(foreach f,$(CXXFLAGS),"$f",)

compile_commands.json: $(SRCS)
	@echo '[' > $@
	@first=true; \
	for src in $(SRCS); do \
		$$first || echo ',' >> $@; \
		first=false; \
		echo '  {"file":"'$$src'","arguments":["$(CXX)",$(CXXFLAGS_JSON)"-c","-o","'$${src%.cpp}.o'","'$$src'"],"directory":"$(PWD)"}' >> $@; \
	done
	@echo ']' >> $@

clean:
	rm -f $(SRC_DIR)/*.o $(TARGET) compile_commands.json
