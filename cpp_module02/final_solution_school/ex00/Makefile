# Project Configuration
CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror
INC = -I.
LDFLAGS = 
LIB_DIR = 
LIBS = 

# Add conditional -L flag only when LIB_DIR is set
LIBDIR_FLAG := $(if $(strip $(LIB_DIR)),-L$(LIB_DIR),)

# Source files (auto-detected or provided)
SRC_LIST = Fixed.cpp main.cpp

# Build directories
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

# CRITICAL FIX: Isolate the Python target injection so rules compile to the right folder
EXEC_BIN = $(BIN_DIR)/ex00

# Object files from SRC_LIST
OBJ := $(addprefix $(OBJ_DIR)/,$(addsuffix .o,$(basename $(SRC_LIST))))

# Exclude main.o from the object list so tests don't get double main() functions
OBJ_NO_MAIN := $(filter-out %/main.o %/Main.o,$(OBJ))

GTEST_DIR = ../../vendor/gtest
GTEST_LIB_PATH = $(GTEST_DIR)/lib
# Only add -lgtest flags if the library files actually exist
HAS_GTEST := $(shell [ -f "$(GTEST_LIB_PATH)/libgtest.a" ] && echo 1 || echo 0)

ifeq ($(HAS_GTEST),1)
    GTEST_LIBS = -L$(GTEST_LIB_PATH) -lgtest -lgtest_main -pthread
    GTEST_DEF = -DHAVE_GTEST=1
else
    GTEST_LIBS =
    GTEST_DEF = -DHAVE_GTEST=0
endif

# Test Configuration
TEST_SRCS := $(wildcard tests/*.cpp tests/*.c)
TEST_BINS := $(foreach f,$(TEST_SRCS),$(BIN_DIR)/$(basename $(notdir $(f))))

# libcpp control flag
LCPP ?= 0

# Auto-detect include directories
ifeq ($(strip $(INC)),)
ifeq ($(LCPP),0)
INC :=
else
INC := -I$(LIB_DIR) -I$(LIB_DIR)/include -I$(LIB_DIR)/inc
endif
endif

# Pre-include libcpp header
ifeq ($(LCPP),0)
CPPFLAGS :=
else
ifeq ($(wildcard $(LIB_DIR)/libcpp.h),)
CPPFLAGS :=
else
CPPFLAGS := -include $(LIB_DIR)/libcpp.h
endif
endif

# Colors
RESET = \033[0m
BOLD = \033[1m
DIM = \033[2m
GREEN = \033[92m
CYAN = \033[96m
YELLOW = \033[93m

# ── Main targets ──────────────────────────────────────────────────────────────
all: $(EXEC_BIN)

$(EXEC_BIN): $(OBJ) | $(BIN_DIR)
	@printf "  $(DIM)linking$(RESET)      $(CYAN)$(notdir $@)$(RESET)\n"
	@if [ "$(LCPP)" != "0" ]; then $(MAKE) -C $(LIB_DIR); fi
	@libfile=""; \
	if [ -f "$(LIB_DIR)/libcpp.a" ]; then libfile="$(LIB_DIR)/libcpp.a"; \
	elif [ -f "$(LIB_DIR)/libcpp.so" ]; then libfile="$(LIB_DIR)/libcpp.so"; fi; \
	if [ -n "$$libfile" ]; then \
		$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INC) $(OBJ) -o $@ $(LDFLAGS) $$libfile; \
	else \
		$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INC) $(OBJ) -o $@ $(LIBDIR_FLAG) $(LDFLAGS) $(LIBS); \
	fi
	@printf "\n  $(GREEN)●$(RESET) $(BOLD)Build complete$(RESET) $(DIM)→ $@$(RESET)\n\n"

# Run target to easily execute the root program
run: $(EXEC_BIN)
	@printf "  $(CYAN)Running $(EXEC_BIN)...$(RESET)\n\n"
	@./$(EXEC_BIN)

# Compile main sources
$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@printf "  $(DIM)compiling$(RESET)    $(CYAN)%-30s$(RESET)\n" "$(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INC) -c $< -o $@

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@printf "  $(DIM)compiling$(RESET)    $(CYAN)%-30s$(RESET)\n" "$(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INC) -x c++ -c $< -o $@

# ── Test targets ──────────────────────────────────────────────────────────────
# Compile test sources
$(OBJ_DIR)/tests/%.o: tests/%.cpp | $(OBJ_DIR)/tests
	@mkdir -p $(dir $@)
	@printf "  $(DIM)compiling test$(RESET) $(CYAN)%-30s$(RESET)\n" "$(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INC) $(GTEST_INC) $(GTEST_DEF) -I. -c $< -o $@

$(BIN_DIR)/%: $(OBJ_DIR)/tests/%.o $(OBJ_NO_MAIN) | $(BIN_DIR)
	@printf "  $(DIM)linking test$(RESET)   $(CYAN)%-30s$(RESET)\n" "$(notdir $@)"
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(INC) $(OBJ_NO_MAIN) $< -o $@ $(GTEST_LIBS) $(LDFLAGS) $(LIBS)
	@printf "  $(GREEN)●$(RESET) $(BOLD)Test build complete$(RESET) $(DIM)→ $@$(RESET)\n"

# Dynamically build and execute any tests found in the tests/ directory
test: $(TEST_BINS)
	@if [ -z "$(TEST_BINS)" ]; then \
		printf "\n  $(YELLOW)⚠$(RESET) No tests found in tests/ directory.\n"; \
	else \
		printf "\n  $(GREEN)●$(RESET) Built $(words $(TEST_BINS)) test(s)\n"; \
		for test_bin in $(TEST_BINS); do \
			echo "  $(CYAN)Executing $$test_bin...$(RESET)"; \
		done; \
	fi

# Google Test build helper (Only runs if 'make gtest' is explicitly called)
gtest:
	@if [ ! -d "$(GTEST_DIR)/build" ]; then \
		mkdir -p "$(GTEST_DIR)/build"; \
	fi
	@if [ ! -f "$(GTEST_DIR)/lib/libgtest.a" ] || [ ! -f "$(GTEST_DIR)/lib/libgtest_main.a" ]; then \
		printf "  $(DIM)building$(RESET)     $(YELLOW)Google Test$(RESET)\n"; \
		cd "$(GTEST_DIR)/build" && cmake .. > /dev/null 2>&1 && $(MAKE) > /dev/null 2>&1; \
		mkdir -p "$(GTEST_DIR)/lib"; \
		cp "$(GTEST_DIR)/build/lib/libgtest.a" "$(GTEST_DIR)/lib/" 2>/dev/null || true; \
		cp "$(GTEST_DIR)/build/lib/libgtest_main.a" "$(GTEST_DIR)/lib/" 2>/dev/null || true; \
	fi

# Clean targets
clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -rf $(BUILD_DIR)

re: fclean all

# Directory creation
$(OBJ_DIR) $(BIN_DIR) $(OBJ_DIR)/tests:
	@mkdir -p $@

# ── Code Quality / Linting ────────────────────────────────────────────────────

# Auto-injected by gen_make.py based on relative directory structures
LINTER_SCRIPT = ../../autotools/norminette.sh

norminette:
	@printf "\n  $(BOLD)$(CYAN) Running C++ Code Quality Checks$(RESET)\n"
	@if [ -f "$(LINTER_SCRIPT)" ]; then \
		python3 "$(LINTER_SCRIPT)" . -- $(CXXFLAGS) -I.; \
	else \
		printf "  $(RED)✘ Linter script not found at $(LINTER_SCRIPT)$(RESET)\n"; \
	fi

# Bonus: Automatically fix clang-format issues
format:
	@printf "\n  $(BOLD)$(CYAN) Auto-formatting C++ Source Files$(RESET)\n"
	@if command -v clang-format >/dev/null 2>&1; then \
		find . -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.c" | xargs clang-format -i -style=Google; \
		printf "  $(GREEN)●$(RESET) Files formatted successfully using Google style.\n"; \
	else \
		printf "  $(RED)✘ clang-format is not installed!$(RESET)\n"; \
	fi

# ── MySQLite mode (conditional) ────────────────────────────────────────────────
sqlite: fclean
	@printf "  $(BOLD)$(CYAN)Building with MySQLite REPL support$(RESET)\n"
	@$(MAKE) CXXFLAGS="$(CXXFLAGS) -DHAVE_MY_SQL_LITE=1" LDFLAGS="-lreadline" all

# ── Demo targets ──────────────────────────────────────────────────────────────
# Each target compiles with -DACTIVE_DEMO=N and runs immediately.
# The source macros in main.cpp translate ACTIVE_DEMO into the right #ifdef.
#
#   make demo    → print this menu
#   make demo1   → Deep Copy        (copy ctor creates independent objects)
#   make demo2   → Shallow Copy     (aliasing + double-free, intentional UB)
#   make demo3   → Assignment       (operator= on existing objects)
#   make demo4   → Self-assign      (a = a must be a safe no-op)
#   make demo5   → Pass by value    (hidden copy ctor on function argument)
#   make demo6   → Heap copy        (new Fixed(a) calls copy ctor on heap)

DEMO_BIN = $(BIN_DIR)/demo
DEMO_SRCS = $(SRC_LIST)

demo:
	@printf "\n  $(BOLD)$(CYAN)Available demos$(RESET)\n"
	@printf "  $(DIM)──────────────────────────────────────────────$(RESET)\n"
	@printf "  $(GREEN)make demo1$(RESET)  — Deep Copy        copy ctor → independent objects\n"
	@printf "  $(GREEN)make demo2$(RESET)  — Shallow Copy     aliasing + double-free $(YELLOW)(UB!)$(RESET)\n"
	@printf "  $(GREEN)make demo3$(RESET)  — Assignment       operator= on existing objects\n"
	@printf "  $(GREEN)make demo4$(RESET)  — Self-assign      a = a must be a safe no-op\n"
	@printf "  $(GREEN)make demo5$(RESET)  — Pass by value    hidden copy ctor on function arg\n"
	@printf "  $(GREEN)make demo6$(RESET)  — Heap copy        new Fixed(a) → copy ctor on heap\n"
	@printf "  $(DIM)──────────────────────────────────────────────$(RESET)\n\n"

demo1: | $(BIN_DIR)
	@printf "  $(DIM)compiling$(RESET)    $(CYAN)demo1 — Deep Copy$(RESET)\n"
	@$(CXX) $(CXXFLAGS) $(INC) -DACTIVE_DEMO=1 $(DEMO_SRCS) -o $(DEMO_BIN)
	@printf "\n  $(GREEN)●$(RESET) $(BOLD)DEMO 1 — Deep Copy$(RESET) $(DIM)(copy constructor → independent objects)$(RESET)\n\n"
	@./$(DEMO_BIN)

demo2: | $(BIN_DIR)
	@printf "  $(DIM)compiling$(RESET)    $(CYAN)demo2 — Shallow Copy$(RESET)\n"
	@$(CXX) $(CXXFLAGS) $(INC) -DACTIVE_DEMO=2 $(DEMO_SRCS) -o $(DEMO_BIN)
	@printf "\n  $(GREEN)●$(RESET) $(BOLD)DEMO 2 — Shallow Copy$(RESET) $(YELLOW)(aliasing + double-free — intentional UB)$(RESET)\n\n"
	@./$(DEMO_BIN)

demo3: | $(BIN_DIR)
	@printf "  $(DIM)compiling$(RESET)    $(CYAN)demo3 — Assignment$(RESET)\n"
	@$(CXX) $(CXXFLAGS) $(INC) -DACTIVE_DEMO=3 $(DEMO_SRCS) -o $(DEMO_BIN)
	@printf "\n  $(GREEN)●$(RESET) $(BOLD)DEMO 3 — Assignment$(RESET) $(DIM)(operator= on existing objects)$(RESET)\n\n"
	@./$(DEMO_BIN)

demo4: | $(BIN_DIR)
	@printf "  $(DIM)compiling$(RESET)    $(CYAN)demo4 — Self-assign$(RESET)\n"
	@$(CXX) $(CXXFLAGS) $(INC) -DACTIVE_DEMO=4 $(DEMO_SRCS) -o $(DEMO_BIN)
	@printf "\n  $(GREEN)●$(RESET) $(BOLD)DEMO 4 — Self-assign$(RESET) $(DIM)(a = a must be a safe no-op)$(RESET)\n\n"
	@./$(DEMO_BIN)

demo5: | $(BIN_DIR)
	@printf "  $(DIM)compiling$(RESET)    $(CYAN)demo5 — Pass by value$(RESET)\n"
	@$(CXX) $(CXXFLAGS) $(INC) -DACTIVE_DEMO=5 $(DEMO_SRCS) -o $(DEMO_BIN)
	@printf "\n  $(GREEN)●$(RESET) $(BOLD)DEMO 5 — Pass by value$(RESET) $(DIM)(hidden copy ctor on function argument)$(RESET)\n\n"
	@./$(DEMO_BIN)

demo6: | $(BIN_DIR)
	@printf "  $(DIM)compiling$(RESET)    $(CYAN)demo6 — Heap copy$(RESET)\n"
	@$(CXX) $(CXXFLAGS) $(INC) -DACTIVE_DEMO=6 $(DEMO_SRCS) -o $(DEMO_BIN)
	@printf "\n  $(GREEN)●$(RESET) $(BOLD)DEMO 6 — Heap copy$(RESET) $(DIM)(new Fixed(a) calls copy ctor on heap)$(RESET)\n\n"
	@./$(DEMO_BIN)

.PHONY: all run clean fclean re test gtest norminette format sqlite \
        demo demo1 demo2 demo3 demo4 demo5 demo6
