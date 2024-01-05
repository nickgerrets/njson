CXX := clang++
DEBUG ?= 1

# -------------------      TARGET       -------------------

TARGET ?= libnjson.a

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
INC_DIRS ?= ./include
CPPFLAGS ?= -Wall -Wextra -std=c++11 -Ofast
LDFLAGS ?=

# --------------------------- END -------------------------

SRCS := $(shell find $(SRC_DIRS) -name *.cpp)
HDRS := $(shell find $(INC_DIRS) -name *.h)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS += $(INC_FLAGS)

.PHONY: all
all: $(TARGET)

# linking
$(TARGET): $(OBJS)
	@echo "Linking..."
	@ar -rv $(TARGET) $(OBJS) $(LDFLAGS)
	@echo "Done!"

# compiling
$(BUILD_DIR)/%.cpp.o: %.cpp $(HDRS)
	@$(MKDIR_P) $(dir $@)
	@echo "Compiling: " $<
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean fclean re
clean:
	$(RM) -r $(BUILD_DIR)

fclean: clean
	$(RM) $(TARGET)

re: fclean all

MKDIR_P ?= mkdir -p
