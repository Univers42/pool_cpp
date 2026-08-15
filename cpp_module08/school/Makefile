# Module-level Makefile for the LAB ONLY (all.cpp).
#
# The exercises are not built from here — each of ex00/ ex01/ ex02/ has its own
# Makefile and is built from inside its own directory, as the subject requires
# and as the rest of this repo does. This file exists because all.cpp is C++17
# scratch work that goes deliberately wider than the C++98 the exercises are
# held to, so it needs different flags and must never be swept into a turn-in
# build.
#
#   make            build build/bin/lab
#   make run        build and run it
#   make check      run the whole module's verification (every exercise too)
#   make fclean     remove build/

CXX = c++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror
SRC = all.cpp

BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin
NAME = $(BIN_DIR)/lab

RESET = \033[0m
BOLD = \033[1m
DIM = \033[2m
GREEN = \033[92m
CYAN = \033[96m

all: $(NAME)

lab: $(NAME)

$(NAME): $(SRC) | $(BIN_DIR)
	@printf "  $(DIM)compiling$(RESET)    $(CYAN)%s$(RESET)\n" "$(SRC)"
	@$(CXX) $(CXXFLAGS) $(SRC) -o $@
	@printf "\n  $(GREEN)*$(RESET) $(BOLD)Lab built$(RESET) $(DIM)-> $@$(RESET)\n\n"

run: $(NAME)
	@./$(NAME)

# The lab asserts its own answers, so a clean exit is the whole test.
test: $(NAME)
	@./$(NAME) > /dev/null && \
		printf "  $(GREEN)*$(RESET) lab: every assertion passed\n"

# Everything: all three exercises plus this lab, under valgrind and the
# sanitizers. See autotools/check_all.sh.
check:
	@sh ../autotools/check_all.sh

$(BIN_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR)/obj

fclean:
	@rm -rf $(BUILD_DIR)

re: fclean all

.PHONY: all lab run test check clean fclean re
