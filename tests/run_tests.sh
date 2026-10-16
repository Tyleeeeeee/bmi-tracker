#!/usr/bin/env bash
cd "$(dirname "$0")/.."

BIN=./bin/bmi
OUT=$(mktemp)
ERR=$(mktemp)
LOG=$(mktemp)
trap 'rm -f "$OUT" "$ERR" "$LOG"' EXIT

"$BIN" -l "$LOG" < tests/input.txt > "$OUT" 2> "$ERR"
code=$?

failed=0
expect() {
    if grep -qF -- "$3" "$2"; then
        echo "  PASS $1"
    else
        echo "  FAIL $1 (missing: $3)"
        failed=$((failed + 1))
    fi
}

if [ "$code" -eq 0 ]; then
    echo "  PASS program exits normally"
else
    echo "  FAIL exit code = $code"
    failed=$((failed + 1))
fi

expect "shows BMI and status after each entry" "$OUT" "Alice: BMI 22.49, Normal"
expect "shows class averages after each entry" "$OUT" "class of 4: average height 167.50 cm, average weight 65.00 kg, average BMI 22.98"
expect "rejects non-numeric input"             "$ERR" "height 'abc' is not a valid number"
expect "rejects out-of-range input"            "$ERR" "height 1.65 is out of range"
expect "rejects duplicate names"               "$ERR" "name \"Alice\" already exists"
expect "finds a student by name"               "$OUT" "Alice  height 170.00 cm  weight 65.00 kg  BMI 22.49  Normal"
expect "reports unknown names"                 "$ERR" "\"Eve\" not found"
expect "lists overweight students"             "$OUT" "Overweight: 2 students"
expect "lists underweight students by word"    "$OUT" "Underweight: 1 student"
expect "rejects invalid status"                "$ERR" "status must be 1"
expect "logs added students"                   "$LOG" "[INFO ] classroom.cpp"
expect "logs invalid input"                    "$LOG" "[WARN ]"

"$BIN" --bogus > /dev/null 2>&1
if [ $? -eq 2 ]; then
    echo "  PASS invalid argument returns exit code 2"
else
    echo "  FAIL invalid argument does not return exit code 2"
    failed=$((failed + 1))
fi

if [ "$failed" -eq 0 ]; then
    echo "Integration tests: all passed"
else
    echo "Integration tests: $failed failed"
    exit 1
fi
