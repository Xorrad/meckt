MAKEFLAGS += -j4

# Functions
rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

GTKFLAGS = $(shell pkg-config --cflags gtk+-3.0)
GTKLIBS = $(shell pkg-config --libs gtk+-3.0)

# Compiler flags
CXX      := g++-13
CXXFLAGS := -std=c++20 -pedantic-errors -Wall -Wno-format-security -Wno-sign-compare -Wno-unused-but-set-variable -DIMGUI_USE_WCHAR32

# Targets
TARGET   := meckt

# Directories
SRC_DIR     := src
INCLUDE_DIR := src
VENDOR_DIR  := vendor
BIN_DIR     := build/bin
OBJ_DIR     := build/bin/obj
DEB_DIR     := build/deb
INCLUDE     := -I$(INCLUDE_DIR) -I$(VENDOR_DIR)/includes -I$(VENDOR_DIR)/includes/jomini-parser/src

# Sources and objects
#$(VENDOR_DIR)/includes/imgui/sfml/imgui-SFML.cpp
SRC          := $(call rwildcard,$(SRC_DIR),*.cpp) \
				$(VENDOR_DIR)/includes/imgui/sfml/imgui-SFML.cpp \
				$(VENDOR_DIR)/includes/imgui/imgui/imgui.cpp \
				$(VENDOR_DIR)/includes/imgui/imgui/imgui_demo.cpp \
				$(VENDOR_DIR)/includes/imgui/imgui/imgui_widgets.cpp \
				$(VENDOR_DIR)/includes/imgui/imgui/imgui_draw.cpp \
				$(VENDOR_DIR)/includes/imgui/imgui/imgui_tables.cpp \
				$(VENDOR_DIR)/includes/imgui/imgui/misc/cpp/imgui_stdlib.cpp \
				$(VENDOR_DIR)/includes/jomini-parser/src/Jomini.cpp \
				$(VENDOR_DIR)/includes/lodepng/lodepng.cpp
PCH_HEADER   := $(SRC_DIR)/pch.hpp
PCH          := $(PCH_HEADER:%.h=$(OBJ_DIR)/%.gch)
OBJECTS      := $(SRC:%.cpp=$(OBJ_DIR)/%.o)
DEPENDENCIES := $(OBJECTS:.o=.d)

# Build type (default, debug, release)
BUILD_TYPE := release
ifeq ($(BUILD_TYPE),debug)
    CXXFLAGS += -O0 -DDEBUG -g #-fsanitize=address
else ifeq ($(BUILD_TYPE),release)
    CXXFLAGS += -O3 -DNDEBUG
endif

# Building a deb file
CXXFLAGS += -DDEB

# Libraries
LDFLAGS :=  -L$(VENDOR_DIR)/lib/fmt -lfmt \
			-L$(VENDOR_DIR)/lib/backward/ -lbackward \
			-L$(VENDOR_DIR)/lib/nfd/ -lnfd \
			-L/usr/lib -lstdc++ -lm -lbfd -ldl -ldw -lsfml-graphics -lsfml-window -lsfml-system -lGL -lcurl

.PHONY: all build deb clean info run
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
	$(CXX) $(GTKFLAGS) $(CXXFLAGS) -o $(BIN_DIR)/$(TARGET) $^ $(LDFLAGS) $(GTKLIBS)

# Make commands

build:
	@clear
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)/assets
	@cp -ar assets/. $(BIN_DIR)/assets

deb:
	@clear
	@rm -rfd $(DEB_DIR)/usr/
	@mkdir -p $(DEB_DIR)/usr/local/bin
	@cp $(BIN_DIR)/$(TARGET) $(DEB_DIR)/usr/local/bin/$(TARGET)
	@chmod 755 $(DEB_DIR)/usr/local/bin/$(TARGET)
	@mkdir -p $(DEB_DIR)/usr/local/share/meckt
	@mkdir -p $(DEB_DIR)/usr/local/share/meckt/logs
	@cp -rf assets $(DEB_DIR)/usr/local/share/meckt/assets
	dpkg-deb --build $(DEB_DIR)

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