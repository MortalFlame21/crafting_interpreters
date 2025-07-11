TARGET := lox.out
SRC_DIR := ./src
BUILD_DIR := ./build
OBJ_DIR := $(BUILD_DIR)/obj

SRCS := $(shell find $(SRC_DIR) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS :=

CXX := g++

CXXFLAGS := -pedantic -Wall -Werror -Weffc++ -Wextra -Wconversion -Wsign-conversion\
			-I ./include
CPPFLAGS :=
LDFLAGS :=

all: $(BUILD_DIR)/$(TARGET)

debug: clean $(BUILD_DIR)/$(TARGET)
debug: CXXFLAGS += -g

$(info SRCS = $(SRCS))
$(info OBJS = $(OBJS))

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)