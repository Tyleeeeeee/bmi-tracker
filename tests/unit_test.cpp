#include "bmi.hpp"
#include "classroom.hpp"

#include <cmath>
#include <iostream>

using namespace bmi;

static int g_failed = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        if (!(cond)) {                                                           \
            std::cerr << "  FAIL " << __FILE__ << ':' << __LINE__ << "  " #cond "\n"; \
            ++g_failed;                                                          \
        }                                                                        \
    } while (0)

#define CHECK_THROWS(expr)                                                       \
    do {                                                                         \
        bool thrown_ = false;                                                    \
        try { (void)(expr); } catch (const InputError&) { thrown_ = true; }      \
        CHECK(thrown_ && "expected InputError: " #expr);                         \
    } while (0)

static bool near(double a, double b) { return std::fabs(a - b) < 1e-9; }

static void testCalcBmi() {
    CHECK(near(calcBmi(170, 65), 22.49));
    CHECK(near(calcBmi(160, 45), 17.58));
    CHECK(near(calcBmi(175, 80), 26.12));
    CHECK_THROWS(calcBmi(0, 60));
    CHECK_THROWS(calcBmi(-170, 60));
    CHECK_THROWS(calcBmi(170, 0));
    CHECK_THROWS(calcBmi(170, 1000));
}

static void testClassifyBoundary() {
    CHECK(classify(18.49) == Status::Under);
    CHECK(classify(18.50) == Status::Normal);
    CHECK(classify(23.99) == Status::Normal);
    CHECK(classify(24.00) == Status::Over);
}

static void testParseNumber() {
    CHECK(near(parseNumber(" 170 ", 50, 250, "height"), 170));
    CHECK_THROWS(parseNumber("", 50, 250, "height"));
    CHECK_THROWS(parseNumber("abc", 50, 250, "height"));
    CHECK_THROWS(parseNumber("170cm", 50, 250, "height"));
    CHECK_THROWS(parseNumber("1.7", 50, 250, "height"));
    CHECK_THROWS(parseNumber("nan", 50, 250, "height"));
    CHECK_THROWS(parseNumber("1e999", 50, 250, "height"));
}

static void testParseStatus() {
    CHECK(parseStatus("1") == Status::Under);
    CHECK(parseStatus("Normal") == Status::Normal);
    CHECK(parseStatus(" 3 ") == Status::Over);
    CHECK(parseStatus("OVERWEIGHT") == Status::Over);
    CHECK_THROWS(parseStatus("4"));
    CHECK_THROWS(parseStatus(""));
}

static void testClassroom() {
    Classroom c;
    c.add("Alice", 170, 65);
    c.add("Bob", 160, 45);
    c.add("Carol", 175, 80);
    c.add("Dave", 165, 70);

    CHECK(c.size() == 4);
    CHECK(c.find("Alice") != nullptr);
    CHECK(c.find(" Alice ") != nullptr);
    CHECK(c.find("Eve") == nullptr);

    CHECK(c.byStatus(Status::Over).size() == 2);
    CHECK(c.byStatus(Status::Under).size() == 1);
    CHECK(c.byStatus(Status::Over)[0].name == "Carol");

    const Averages a = c.averages();
    CHECK(near(a.height, 167.5));
    CHECK(near(a.weight, 65.0));
    CHECK(near(a.bmi, 22.975));

    CHECK_THROWS(c.add("Alice", 180, 70));
    CHECK_THROWS(c.add("  ", 180, 70));
    CHECK_THROWS(c.add("Frank", -1, 70));
    CHECK(c.size() == 4);
}

int main() {
    testCalcBmi();
    testClassifyBoundary();
    testParseNumber();
    testParseStatus();
    testClassroom();

    if (g_failed == 0) {
        std::cout << "Unit tests: all passed\n";
        return 0;
    }
    std::cout << "Unit tests: " << g_failed << " failed\n";
    return 1;
}
