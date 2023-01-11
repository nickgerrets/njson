CXX := clang++
DEBUG ?= 1

# -------------------      TARGET       -------------------

TARGET ?= test

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
INC_DIRS ?=
CPPFLAGS ?= -Wall -Wextra -std=c++11
LDFLAGS ?=

# -------------------        LIB        -------------------

LIB_TARGET := libnjson.a
LIB_DIR := ./lib/njson
LDFLAGS += -L"./lib/njson" -lnjson
INC_DIRS += $(LIB_DIR)/include

# --------------------------- END -------------------------

LIB := $(LIB_DIR)/$(LIB_TARGET)
SRCS := $(shell find $(SRC_DIRS) -name *.cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS += $(INC_FLAGS) -MMD -MP

.PHONY: all lib
all: lib $(TARGET)

lib:
	$(MAKE) -C $(LIB_DIR)

# linking
$(TARGET): $(OBJS)
	@echo "Linking..."
	@$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)
	@echo "Done!"

# compiling
$(BUILD_DIR)/%.cpp.o: %.cpp $(LIB)
	@$(MKDIR_P) $(dir $@)
	@echo "Compiling: " $<
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean fclean re
clean:
	$(RM) -r $(BUILD_DIR)

fclean: clean
	$(RM) $(TARGET)
	$(MAKE) fclean -C $(LIB_DIR)

re: clean all

MKDIR_P ?= mkdir -p
