# =========== Compiler config ===========
CXX			= g++ # clang++ also works
CXX_FLAGS	= -I../inc -std=c++17 -Wall -Wpedantic -Wextra -Werror -O3
LD_FLAGS	=
# =======================================


OBJ_DIR		= tmp_make
OUT_DIR		= out_make
SRC_DIRS	= ./src/
EXEC		= tests
SRCS := $(shell find $(SRC_DIRS) \( -name '*.cpp' \))
OBJS := $(SRCS:%=%.o)
OBJS := $(notdir $(OBJS))
OBJS := $(OBJS:%=$(OBJ_DIR)/%)
vpath %.cpp $(sort $(dir $(SRCS)))

# Launching executable
.PHONY: all
all: $(OUT_DIR)/$(EXEC)
	@printf "\nRunning tests ...\n\n"
	@cd $(OUT_DIR) && ./$(EXEC)

# Final build (using CXX)
$(OUT_DIR)/$(EXEC): $(OBJS)
	@$(CXX) $(LD_FLAGS) $(OBJS) -o $@

# Compliling C++
$(OBJ_DIR)/%.cpp.o: %.cpp
	@printf "%-*s" 75 "Compiling $<"
	@$(CXX) $(CXX_FLAGS) -c -o $@ $< && printf "[\e[0;32mOK\e[0m]\n"

.PHONY: clean
clean:
	@mv tmp_make/.gitignore ./
	@rm -rf tmp_make/*
	@rm -rf out_make/*
	@mv ./.gitignore tmp_make/
	@cp tmp_make/.gitignore out_make/