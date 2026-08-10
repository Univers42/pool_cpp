# Project Configuration
CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror
LDFLAGS = 
LIBS = -lpthread

# ── libcpp (shared library) ──────────────────────────────────────────────────
LIB_DIR  = ../libcpp
LIB_NAME = $(LIB_DIR)/libcpp.a
INC      = -I. -I$(LIB_DIR)

# Source files (exercise sources only — library is in ../libcpp)
SRC_LIST = AMateria.cpp Character.cpp Cure.cpp Ice.cpp MateriaSource.cpp main.cpp

# Build directories
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

EXEC_BIN = $(BIN_DIR)/ex03

OBJ := $(addprefix $(OBJ_DIR)/,$(addsuffix .o,$(basename $(SRC_LIST))))
OBJ_NO_MAIN := $(filter-out %/main.o %/Main.o,$(OBJ))

GTEST_DIR = ../../vendor/gtest
GTEST_LIB_PATH = $(GTEST_DIR)/lib
HAS_GTEST := $(shell [ -f "$(GTEST_LIB_PATH)/libgtest.a" ] && echo 1 || echo 0)
ifeq ($(HAS_GTEST),1)
    GTEST_LIBS = -L$(GTEST_LIB_PATH) -lgtest -lgtest_main -pthread
    GTEST_DEF = -DHAVE_GTEST=1
else
    GTEST_LIBS =
    GTEST_DEF = -DHAVE_GTEST=0
endif

TEST_SRCS := $(wildcard tests/*.cpp tests/*.c)
TEST_BINS := $(foreach f,$(TEST_SRCS),$(BIN_DIR)/$(basename $(notdir $(f))))

RESET = \033[0m
BOLD = \033[1m
DIM = \033[2m
GREEN = \033[92m
CYAN = \033[96m
YELLOW = \033[93m

# ── Main targets ──
all: $(EXEC_BIN)

# ── Build libcpp.a ──
$(LIB_NAME):
	@$(MAKE) --no-print-directory -C $(LIB_DIR)

$(EXEC_BIN): $(OBJ) $(LIB_NAME) | $(BIN_DIR)
	@printf "  $(DIM)linking$(RESET)      $(CYAN)$(notdir $@)$(RESET)\n"
	@$(CXX) $(CXXFLAGS) $(INC) $(OBJ) -o $@ $(LIB_NAME) $(LDFLAGS) $(LIBS)
	@printf "\n  $(GREEN)●$(RESET) $(BOLD)Build complete$(RESET) $(DIM)→ $@$(RESET)\n\n"

run: $(EXEC_BIN)
	@printf "  $(CYAN)Running $(EXEC_BIN)...$(RESET)\n\n"
	@./$(EXEC_BIN)

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@printf "  $(DIM)compiling$(RESET)    $(CYAN)%-30s$(RESET)\n" "$(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

$(OBJ_DIR)/tests/%.o: tests/%.cpp | $(OBJ_DIR)/tests
	@mkdir -p $(dir $@)
	@printf "  $(DIM)compiling test$(RESET) $(CYAN)%-30s$(RESET)\n" "$(notdir $<)"
	@$(CXX) $(CXXFLAGS) $(INC) $(GTEST_DEF) -c $< -o $@

$(BIN_DIR)/%: $(OBJ_DIR)/tests/%.o $(OBJ_NO_MAIN) $(LIB_NAME) | $(BIN_DIR)
	@printf "  $(DIM)linking test$(RESET)   $(CYAN)%-30s$(RESET)\n" "$(notdir $@)"
	@$(CXX) $(CXXFLAGS) $(INC) $(OBJ_NO_MAIN) $< -o $@ $(LIB_NAME) $(GTEST_LIBS) $(LDFLAGS) $(LIBS)
	@printf "  $(GREEN)●$(RESET) $(BOLD)Test build complete$(RESET) $(DIM)→ $@$(RESET)\n"

test: $(TEST_BINS)
	@if [ -z "$(TEST_BINS)" ]; then \
		printf "\n  $(YELLOW)⚠$(RESET) No tests found.\n"; \
	else \
		printf "\n  $(GREEN)●$(RESET) Built $(words $(TEST_BINS)) test(s)\n"; \
		for test_bin in $(TEST_BINS); do \
			echo "  $(CYAN)Executing $$test_bin...$(RESET)"; \
		done; \
	fi

clean:
	@rm -rf $(OBJ_DIR)

fclean: clean
	@rm -rf $(BUILD_DIR)

re: fclean all

$(OBJ_DIR) $(BIN_DIR) $(OBJ_DIR)/tests:
	@mkdir -p $@

.PHONY: all run clean fclean re test
