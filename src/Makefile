# -----------------------------------------------------------------------------
# @file Makefile
# @brief Build system for the lcblog library and associated tests.
#
# This Makefile automates the process of building the lcblog static library, 
# compiling the test executable with debug flags enabled, and running static 
# code analysis using cppcheck.
#
# @targets
# - `all`: Builds the static library `liblcblog.a`.
# - `test`: Builds the test executable `lcblog_test` with debug flags enabled.
# - `debug`: Builds the static library with debugging symbols enabled.
# - `lint`: Runs cppcheck for static code analysis on the source files.
# - `clean`: Cleans up build artifacts (object files, library, test executable).
# - `help`: Displays available make targets and their descriptions.
# -----------------------------------------------------------------------------
 
# Target names
NAME        = lcblog
LIB_NAME    = lib$(NAME).a
TEST_NAME   = $(NAME)_test
TEST_LOGS   = test_log.*

# Compiler settings
CXX         = g++
CXXFLAGS    += -Wall -Werror -fmax-errors=5
CXXFLAGS    += -Wno-psabi -std=c++17 -MMD -MP

# Linker flags (if needed)
LDLIBS      = 

# Source files
SRC         = $(NAME).cpp
OBJ         = $(SRC:.cpp=.o)
TEST_DEP 	= $(TEST_NAME).d
DEP 		= $(OBJ:.o=.d) $(TEST_DEP)

# Ensure parallel compilation
MAKEFLAGS += -j$(shell nproc)

# Default build target: creates the static library
all: $(LIB_NAME)

# Create the static library
$(LIB_NAME): $(OBJ)
	ar rcs $@ $(OBJ)

# Clean old test logs before running tests
clean_logs:
	$(RM) $(TEST_LOGS)

# Build the test executable (Only compiles main.cpp here)
test: clean_logs $(OBJ)
	$(CXX) $(CXXFLAGS) main.cpp $(OBJ) -o $(TEST_NAME) $(LDLIBS)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	-$(RM) $(OBJ) $(DEP) $(LIB_NAME) $(TEST_NAME) $(TEST_LOGS)

# Include dependency files
-include $(DEP)

# Debug build: compiles the library with debug flags enabled
debug: clean
debug: CXXFLAGS += -g
debug: $(LIB_NAME)

# Lint target: performs static code analysis with cppcheck
lint:
	@command -v cppcheck >/dev/null 2>&1 || { \
	    echo "Error: cppcheck not found. Please install it using:"; \
	    echo "  sudo apt install cppcheck   # Debian/Ubuntu"; \
	    echo "  sudo dnf install cppcheck   # Fedora"; \
	    echo "  brew install cppcheck       # macOS (Homebrew)"; \
	    echo "  choco install cppcheck      # Windows (Chocolatey)"; \
	    exit 1; \
	}
	cppcheck --enable=all --inconclusive --max-configs=100 --std=c++17 --language=c++ $(SRC) --suppress=missingIncludeSystem

# Help target: displays available make targets and their descriptions
help:
	@echo "Available Make targets:"
	@echo "  all        - Build the static library."
	@echo "  test       - Build the test executable with debug flags."
	@echo "  debug      - Build the static library with debugging symbols."
	@echo "  lint       - Run cppcheck for static code analysis."
	@echo "  clean      - Clean up build artifacts (object files, library, etc.)."
	@echo "  help       - Display this help message."

.PHONY: all test debug lint clean help clean_logs