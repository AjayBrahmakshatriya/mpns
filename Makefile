include deps.mk
LIBRARY_NAME=mpns
BASE_DIR=$(shell pwd)
SRC_DIR=$(BASE_DIR)/src

BUILD_DIR?=$(BASE_DIR)/build

INCLUDE_DIR=$(BASE_DIR)/include
INCLUDE_DIR+=$(BUILDIT_DIR)/include

INCLUDE_FLAG=-I$(BASE_DIR)/include -I$(BUILDIT_DIR)/include
INCLUDES=$(wildcard $(INCLUDE_DIR)/*.h) $(wildcard $(INCLUDE_DIR)/*/*.h)

$(shell mkdir -p $(BUILD_DIR))



DEBUG ?= 0

ifeq ($(DEBUG),1)
CFLAGS=-g -std=c++11
else
CFLAGS=-std=c++11 -O3
endif

#CFLAGS+=-Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wmissing-declarations -Woverloaded-virtual -pedantic-errors -Wno-deprecated -Wdelete-non-virtual-dtor -Werror

SRCS=$(wildcard $(SRC_DIR)/*.cpp)
OBJS=$(subst $(SRC_DIR),$(BUILD_DIR),$(SRCS:.cpp=.o))

LIBRARY=$(BUILD_DIR)/$(LIBRARY_NAME).a

all: $(LIBRARY)

.PRECIOUS: $(BUILD_DIR)/%.o

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDES)
	$(CXX) $(CFLAGS) $< -o $@ $(INCLUDE_FLAG) -c

$(LIBRARY): $(OBJS)
	ar rv $(LIBRARY) $(OBJS)
	echo $(OBJS)
	echo $(SRCS)


clean:
	rm -rf $(BUILD_DIR)
