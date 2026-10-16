# bmi — Student BMI Calculator and Lookup

A terminal tool for entering students' names, heights and weights one after another. After each entry it immediately shows the student's BMI, status (underweight / normal / overweight) and the class averages. When input is finished, you can look up a student's status by name or list every student with a given status.

Runs on Linux and macOS (bash / zsh). Requires a C++17 compiler and make.

## Project Structure

```
bmi-tracker/
├── Makefile
├── README.md
├── include/
│   ├── bmi.hpp         BMI calculation, classification, input validation, exception type
│   ├── classroom.hpp   Class data: add, find by name, filter by status, averages
│   └── logger.hpp      Logging
├── src/
│   ├── bmi.cpp
│   ├── classroom.cpp
│   ├── logger.cpp
│   └── main.cpp        Argument parsing, input phase, query phase
├── tests/
│   ├── unit_test.cpp   Unit tests
│   ├── run_tests.sh    Integration test (feeds input.txt as keyboard input)
│   └── input.txt
└── scripts/
    ├── debug.gdb       Breakpoints loaded by make debug (Linux)
    └── debug.lldb      Same breakpoints for lldb (macOS)
```

## Quick Start

```bash
make setup
make
make run
make test
make install
```

| Target | Description |
|---|---|
| `make setup` | Check the environment (compiler, C++17 support, debugger) |
| `make` | Build `bin/bmi` |
| `make run` | Run the program |
| `make test` | Run unit tests and the integration test |
| `make debug` | Start the program in gdb (lldb on macOS) with breakpoints preset |
| `make install` | Copy `bmi` to `~/.local/bin` so it can be run from any directory |
| `make uninstall` | Remove the installed binary |
| `make clean` | Remove build output and `bmi.log` |

Missing tools on Linux: `sudo apt install build-essential gdb`. On macOS: `xcode-select --install`.

## Usage

```
bmi [-l LOGFILE] [-v] [-h]

  -l, --log FILE   Log file path (default: bmi.log in the current directory)
  -v, --verbose    Log DEBUG messages and also print the log to stderr
  -h, --help       Show help
```

The program runs in two phases:

1. **Input phase** — enter name → height (cm) → weight (kg). After each student the program prints the BMI, the status and the class averages (height, weight, BMI). Press Enter on an empty name (or Ctrl-D) to finish input.
2. **Query phase** — `1` look up a student by name, `2` list students by status (`1`/`underweight`, `2`/`normal`, `3`/`overweight`), `3` list the whole class, `0` quit.

"After input is finished" means the input phase has ended while the program keeps running, so the data stays in memory for queries. The program exits only when you choose `0`.

Prompts are shown only when stdin is a terminal, so input can also be redirected from a file like any other Unix tool:

```bash
bmi < tests/input.txt
```

| Exit code | Meaning |
|---|---|
| 0 | Success |
| 1 | Unexpected runtime error |
| 2 | Invalid command-line argument |

BMI categories follow the Taiwan Health Promotion Administration standard: underweight < 18.5 ≤ normal < 24 ≤ overweight. BMI is rounded to two decimal places, and the category is decided from the rounded value.

## Defensive Programming

| Technique | Purpose | Where |
|---|---|---|
| Exceptions | Expected errors caused by the user: non-numeric input, out-of-range values, duplicate or empty names, invalid status. An `InputError` is thrown and the caller shows the message and asks again | `parseNumber`, `calcBmi`, `parseStatus` in `bmi.cpp`; `Classroom::add` in `classroom.cpp` |
| Top-level try/catch | Any unhandled exception is logged and the program exits with code 1 instead of crashing | `main` in `main.cpp` |
| assert | Programmer errors, i.e. states that should never happen: division by zero, negative BMI, averaging an empty class | `calcBmi`, `classify`, `Classroom::add`, `Classroom::averages` |
| static_assert | Compile-time check that the underweight threshold is below the overweight threshold | `bmi.hpp` |
| Logging | Records program start and exit, every added student, every invalid input (WARN) and every query, with timestamp, level, file name and line number | `logger.hpp`, `logger.cpp` |

Exceptions and asserts serve different purposes. Wrong user input is a normal situation that the program must handle and recover from, so it uses exceptions. An assert checks a condition that is always true if the code is correct; a failure means there is a bug, so the program stops immediately and reports where. Compiling with `-DNDEBUG` removes all asserts, which is why asserts must never be used to validate user input.

Example `bmi.log`:

```
2026-10-16 13:45:02 [INFO ] main.cpp:164  program started
2026-10-16 13:45:10 [INFO ] classroom.cpp:23  added Alice height=170 weight=65 bmi=22.49 status=Normal
2026-10-16 13:45:15 [WARN ] main.cpp:49  invalid input: height 'abc' is not a valid number
```

## Testing

`make test` runs:

- **Unit tests** (`tests/unit_test.cpp`): BMI calculation, category boundaries (18.49 / 18.50 / 23.99 / 24.00), invalid input (empty, `abc`, `170cm`, `nan`, overflow), duplicate names, data left unchanged after a failed add, and averages.
- **Integration test** (`tests/run_tests.sh`): runs the whole program with `tests/input.txt` as keyboard input and checks the screen output, error messages, log contents and exit codes.

## Debugging

`make debug` starts the program in gdb (lldb on macOS) with three breakpoints already set:

1. **Breakpoint** on `bmi::Classroom::add` — stops every time a student is added
2. **Conditional breakpoint** on `bmi::classify if value >= 24` — stops only for overweight students
3. **Catchpoint** `catch throw bmi::InputError` — stops when invalid input throws an exception

Type `run` in gdb to start. Useful commands:

| Command | Action |
|---|---|
| `next` / `n` | Run the next line |
| `step` / `s` | Step into a function |
| `continue` / `c` | Run to the next breakpoint |
| `finish` | Run until the current function returns and show the return value |
| `print name` / `p *this` | Print a variable |
| `info locals` | Show all local variables |
| `backtrace` / `bt` | Show the call stack |
| `watch <var>` | Stop when a variable changes |
| `info breakpoints`, `delete N` | Manage breakpoints |

To skip typing input by hand, run with the sample input: `run < tests/input.txt`

### Triggering an assert

Asserts never fail during normal use. To demonstrate one, change a variable in gdb to simulate a bug:

```
(gdb) break bmi::classify
(gdb) run
Name: Alice
Height (cm): 170
Weight (kg): 65
(gdb) print value
(gdb) set var value = -1
(gdb) continue
(gdb) bt
```

`print value` shows 22.49. After `set var value = -1`, `continue` makes the assert in `classify` fail and the program receives SIGABRT. `bt` shows which assert stopped it.

### Suggested screenshots

1. `make debug` stopped at `Classroom::add`, with `print` or `info args` showing the arguments
2. The conditional breakpoint stopping only for an overweight student
3. Entering `abc` and stopping at the catchpoint, with `bt` showing the throw from `parseNumber`
4. Stepping line by line with `next` / `step`
5. Triggering the assert with `set var`
6. `make test` passing
7. The contents of `bmi.log`
