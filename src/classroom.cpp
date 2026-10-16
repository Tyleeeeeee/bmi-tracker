#include "classroom.hpp"

#include "logger.hpp"

#include <cassert>

namespace bmi {
const Student& Classroom::add(const std::string& name, double height, double weight) {
    Student s;
    s.name = trim(name);
    if (s.name.empty()) throw InputError("name must not be empty");
    if (find(s.name)) throw InputError("name \"" + s.name + "\" already exists");

    s.height = height;
    s.weight = weight;
    s.bmi = calcBmi(height, weight);
    s.status = classify(s.bmi);

    const std::size_t before = students_.size();
    students_.push_back(s);
    assert(students_.size() == before + 1);

    LOG_INFO("added " << s.name << " height=" << height << " weight=" << weight << " bmi=" << s.bmi
                       << " status=" << statusName(s.status));
    return students_.back();
}

const Student* Classroom::find(const std::string& name) const {
    const std::string key = trim(name);
    for (const auto& s : students_) {
        if (s.name == key) return &s;
    }
    return nullptr;
}

std::vector<Student> Classroom::byStatus(Status st) const {
    std::vector<Student> result;
    for (const auto& s : students_) {
        if (s.status == st) result.push_back(s);
    }
    return result;
}

Averages Classroom::averages() const {
    assert(!students_.empty() && "averages() requires at least one student");

    Averages a;
    for (const auto& s : students_) {
        a.height += s.height;
        a.weight += s.weight;
        a.bmi += s.bmi;
    }
    const double n = static_cast<double>(students_.size());
    a.height /= n;
    a.weight /= n;
    a.bmi /= n;
    return a;
}

}
