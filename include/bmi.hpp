#pragma once

#include <stdexcept>
#include <string>

namespace bmi {
class InputError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

enum class Status { Under, Normal, Over };

constexpr double kUnderLimit = 18.5;
constexpr double kOverLimit = 24.0;
static_assert(kUnderLimit < kOverLimit, "underweight limit must be below overweight limit");

constexpr double kMinHeight = 50, kMaxHeight = 250;
constexpr double kMinWeight = 10, kMaxWeight = 300;

double calcBmi(double heightCm, double weightKg);

Status classify(double value);

std::string statusName(Status s);

Status parseStatus(const std::string& text);

double parseNumber(const std::string& text, double min, double max, const std::string& field);

std::string trim(const std::string& s);

}
