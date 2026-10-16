CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -g -O0
CPPFLAGS += -Iinclude
PREFIX   ?= $(HOME)/.local

SRC      := $(wildcard src/*.cpp)
OBJ      := $(SRC:src/%.cpp=build/%.o)
LIB_OBJ  := $(filter-out build/main.o,$(OBJ))
HEADERS  := $(wildcard include/*.hpp)

.PHONY: all setup run test debug install uninstall clean

all: bin/bmi

bin/bmi: $(OBJ)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $^

bin/unit_test: tests/unit_test.cpp $(LIB_OBJ)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^

build/%.o: src/%.cpp $(HEADERS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

setup:
	@echo "Checking environment:"
	@command -v $(CXX) >/dev/null && echo "  ok   $(CXX)" || { echo "  missing $(CXX) (Linux: sudo apt install build-essential, macOS: xcode-select --install)"; exit 1; }
	@echo 'int main(){ if constexpr (true) {} }' | $(CXX) -std=c++17 -x c++ - -o /dev/null 2>/dev/null \
		&& echo "  ok   C++17 support" || { echo "  missing C++17 support"; exit 1; }
	@echo "  ok   make"
	@if command -v gdb >/dev/null; then echo "  ok   gdb"; \
	 elif command -v lldb >/dev/null; then echo "  ok   lldb"; \
	 else echo "  missing debugger (Linux: sudo apt install gdb)"; fi
	@mkdir -p bin build
	@echo "Ready. Run make to build."

run: bin/bmi
	./bin/bmi

test: bin/unit_test bin/bmi
	@./bin/unit_test
	@bash tests/run_tests.sh

debug: bin/bmi
	@if command -v gdb >/dev/null; then gdb -q -x scripts/debug.gdb ./bin/bmi; \
	 else lldb -s scripts/debug.lldb ./bin/bmi; fi

install: bin/bmi
	mkdir -p $(PREFIX)/bin
	cp bin/bmi $(PREFIX)/bin/bmi
	@echo "Installed to $(PREFIX)/bin/bmi (add $(PREFIX)/bin to PATH if bmi is not found)"

uninstall:
	rm -f $(PREFIX)/bin/bmi

clean:
	rm -rf bin build bmi.log
