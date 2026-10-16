#pragma once

#include "bmi.hpp"

#include <string>
#include <vector>

namespace bmi {
struct Student {
    std::string name;
    double height = 0;
    double weight = 0;
    double bmi = 0;
    Status status = Status::Normal;
};

struct Averages {
    double height = 0, weight = 0, bmi = 0;
};

class Classroom {
public:
    const Student& add(const std::string& name, double height, double weight);

    const Student* find(const std::string& name) const;

    std::vector<Student> byStatus(Status s) const;

    Averages averages() const;

    const std::vector<Student>& all() const { return students_; }
    std::size_t size() const { return students_.size(); }
    bool empty() const { return students_.empty(); }

private:
    std::vector<Student> students_;
};

}
