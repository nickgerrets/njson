CXX := clang++
DEBUG ?= 1

# -------------------      TARGET       -------------------

TARGET ?= test

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src ./include
INC_DIRS ?= ./include ../include
CPPFLAGS ?= -Wall -Wextra -std=c++11
LDFLAGS ?= -L"../" -lnjson

# --------------------------- END -------------------------

SRCS := $(shell find $(SRC_DIRS) -name *.cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS_LIB:.o=.d)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS += $(INC_FLAGS) -MMD -MP

.PHONY: all
all: $(TARGET)

# linking
$(TARGET): $(OBJS)
	@echo "Linking..."
	@$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)
	@echo "Done!"

# compiling
$(BUILD_DIR)/%.cpp.o: %.cpp
	@$(MKDIR_P) $(dir $@)
	@echo "Compiling: " $<
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean fclean re
clean:
	$(RM) -r $(BUILD_DIR)

fclean: clean
	$(RM) $(TARGET)

re: clean all

-include $(DEPS)

MKDIR_P ?= mkdir -p
