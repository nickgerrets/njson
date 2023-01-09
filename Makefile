CXX := clang++
DEBUG ?= 1

# -------------------      TARGET       -------------------

TARGET ?= libnjson.a

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src ./include
INC_DIRS ?= ./include
CPPFLAGS ?= -Wall -Wextra -std=c++11
LDFLAGS ?=

# --------------------------- END -------------------------

SRCS := $(shell find $(SRC_DIRS) -name *.cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS_LIB:.o=.d)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS += $(INC_FLAGS) -MMD -MP

.PHONY: all _tester
all: $(TARGET) _tester

_tester:
	$(MAKE) -C "tester/"
	@mv "tester/test" "./"

# linking
$(TARGET): $(OBJS)
	@echo "Linking..."
	@ar -rv $(TARGET) $(OBJS) $(LDFLAGS)
	@echo "Done!"

# compiling
$(BUILD_DIR)/%.cpp.o: %.cpp
	@$(MKDIR_P) $(dir $@)
	@echo "Compiling: " $<
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean fclean _tester_fclean re
clean:
	$(RM) -r $(BUILD_DIR)

fclean: clean
	$(RM) $(TARGET)
	$(RM) test

_tester_fclean:
	$(RM) "./test"

re: fclean all
	$(MAKE) re -C "tester/"

-include $(DEPS)

MKDIR_P ?= mkdir -p
