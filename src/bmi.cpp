#include "bmi.hpp"

#include "logger.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <sstream>

namespace bmi {
double calcBmi(double heightCm, double weightKg) {
    if (!(heightCm >= kMinHeight && heightCm <= kMaxHeight))
        throw InputError("height out of range");
    if (!(weightKg >= kMinWeight && weightKg <= kMaxWeight))
        throw InputError("weight out of range");

    const double m = heightCm / 100.0;
    assert(m > 0 && "height must be positive after validation");

    const double value = std::round(weightKg / (m * m) * 100.0) / 100.0;
    assert(value > 0 && std::isfinite(value));

    LOG_DEBUG("calcBmi(" << heightCm << ", " << weightKg << ") = " << value);
    return value;
}

Status classify(double value) {
    assert(value > 0 && std::isfinite(value) && "classify() received an invalid BMI");

    if (value < kUnderLimit) return Status::Under;
    if (value < kOverLimit) return Status::Normal;
    return Status::Over;
}

std::string statusName(Status s) {
    switch (s) {
    case Status::Under:  return "Underweight";
    case Status::Normal: return "Normal";
    case Status::Over:   return "Overweight";
    }
    assert(false && "unhandled Status value");
    return "?";
}

Status parseStatus(const std::string& text) {
    std::string t = trim(text);
    std::transform(t.begin(), t.end(), t.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (t == "1" || t == "underweight") return Status::Under;
    if (t == "2" || t == "normal") return Status::Normal;
    if (t == "3" || t == "overweight") return Status::Over;
    throw InputError("status must be 1 (underweight), 2 (normal) or 3 (overweight)");
}

double parseNumber(const std::string& text, double min, double max, const std::string& field) {
    const std::string t = trim(text);
    if (t.empty()) throw InputError(field + " must not be empty");

    double value = 0;
    std::size_t used = 0;
    try {
        value = std::stod(t, &used);
    } catch (const std::exception&) {
        throw InputError(field + " '" + t + "' is not a valid number");
    }
    if (used != t.size()) throw InputError(field + " '" + t + "' contains extra characters");
    if (!std::isfinite(value)) throw InputError(field + " '" + t + "' is not a valid number");

    if (value < min || value > max) {
        std::ostringstream os;
        os << field << " " << value << " is out of range (" << min << " - " << max << ")";
        throw InputError(os.str());
    }
    return value;
}

std::string trim(const std::string& s) {
    const auto begin = s.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) return "";
    const auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(begin, end - begin + 1);
}

}
