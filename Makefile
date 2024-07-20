MAKEFLAGS += -j4

# Functions
rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

GTKFLAGS = $(shell pkg-config --cflags gtk+-3.0)
GTKLIBS = $(shell pkg-config --libs gtk+-3.0)

# Compiler flags
CXX      := g++
CXXFLAGS := -std=c++17 -pedantic-errors -Wall -Wno-format-security

# Targets
TARGET   := xme

# Directories
SRC_DIR     := src
INCLUDE_DIR := src
VENDOR_DIR  := vendor
BIN_DIR     := bin
OBJ_DIR     := bin/obj
INCLUDE     := -I$(INCLUDE_DIR) -I$(VENDOR_DIR)/includes

# Sources and objects
#$(VENDOR_DIR)/includes/imgui/sfml/imgui-SFML.cpp
SRC          := $(call rwildcard,$(SRC_DIR),*.cpp) \
				$(VENDOR_DIR)/includes/imgui/sfml/imgui-SFML.cpp \
				$(VENDOR_DIR)/includes/imgui/imgui/imgui.cpp \
				$(VENDOR_DIR)/includes/imgui/imgui/imgui_demo.cpp \
				$(VENDOR_DIR)/includes/imgui/imgui/imgui_widgets.cpp \
				$(VENDOR_DIR)/includes/imgui/imgui/imgui_draw.cpp \
				$(VENDOR_DIR)/includes/imgui/imgui/imgui_tables.cpp 
PCH_HEADER   := $(SRC_DIR)/pch.hpp
PCH          := $(PCH_HEADER:%.h=$(OBJ_DIR)/%.gch)
OBJECTS      := $(SRC:%.cpp=$(OBJ_DIR)/%.o)
DEPENDENCIES := $(OBJECTS:.o=.d)

# Build type (default, debug, release)
BUILD_TYPE := debug
ifeq ($(BUILD_TYPE),debug)
    CXXFLAGS += -O0 -DDEBUG -g
else ifeq ($(BUILD_TYPE),release)
    CXXFLAGS += -O3 -DNDEBUG
endif

# Libraries
LDFLAGS :=  -L$(VENDOR_DIR)/lib/fmt -lfmt \
			-L$(VENDOR_DIR)/lib/backward/ -lbackward \
			-L$(VENDOR_DIR)/lib/nfd/ -lnfd \
			-L/usr/lib -lstdc++ -lm -lbfd -ldl -ldw -lsfml-graphics -lsfml-window -lsfml-system -lGL

.PHONY: all build clean debug release info run
all: build $(BIN_DIR)/$(TARGET)

# Add the PCH target to build the precompiled header
$(PCH): $(PCH_HEADER)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -x c++-header $(PCH_HEADER) -MMD -MP -o $(PCH)

$(OBJ_DIR)/%.o: %.cpp $(PCH)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -MP -o $@ -include $(PCH_HEADER)
	
-include $(DEPENDENCIES)

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(GTKFLAGS) $(CXXFLAGS) -o $(BIN_DIR)/$(TARGET) $^ $(LIB) $(LDFLAGS) $(GTKLIBS)

# Make commands

build:
	@clear
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)

run:
	@./$(BIN_DIR)/$(TARGET)

clean:
	-@rm -rvf $(OBJ_DIR)/* $(BIN_DIR)/*

info:
	@echo "Target: ${TARGET}"
	@echo "Sources: ${SRC}"
	@echo "Application dir: ${BIN_DIR}"
	@echo "Object dir: ${OBJ_DIR}"
	@echo "Objects: ${OBJECTS}"
	@echo "Dependencies: ${DEPENDENCIES}"