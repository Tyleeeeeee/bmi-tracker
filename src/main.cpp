#include "bmi.hpp"
#include "classroom.hpp"
#include "logger.hpp"

#include <iomanip>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace bmi;

namespace {
bool g_interactive = true;

void printUsage() {
    std::cout << "Usage: bmi [-l LOGFILE] [-v] [-h]\n"
                 "\n"
                 "Enter each student's name, height (cm) and weight (kg). The BMI, status and\n"
                 "class averages are shown after every entry. Press Enter on an empty name to\n"
                 "finish input, then look up students by name or by status.\n"
                 "\n"
                 "  -l, --log FILE   log file path (default: bmi.log in the current directory)\n"
                 "  -v, --verbose    log DEBUG messages and also print the log to stderr\n"
                 "  -h, --help       show this help\n"
                 "\n"
                 "Examples:\n"
                 "  bmi\n"
                 "  bmi -l /tmp/bmi.log\n"
                 "  bmi < input.txt\n";
}

bool readLine(const std::string& prompt, std::string& line) {
    if (g_interactive) std::cout << prompt << std::flush;
    if (!std::getline(std::cin, line)) {
        if (g_interactive) std::cout << '\n';
        return false;
    }
    return true;
}

bool askNumber(const std::string& prompt, double min, double max, const std::string& field,
               double& out) {
    std::string line;
    while (readLine(prompt, line)) {
        try {
            out = parseNumber(line, min, max, field);
            return true;
        } catch (const InputError& e) {
            LOG_WARN("invalid input: " << e.what());
            std::cerr << "  error: " << e.what() << ", please try again\n";
        }
    }
    return false;
}

void printStudent(const Student& s) {
    std::cout << "  " << s.name << "  height " << s.height << " cm  weight " << s.weight
              << " kg  BMI " << s.bmi << "  " << statusName(s.status) << '\n';
}

void inputPhase(Classroom& cls) {
    if (g_interactive) std::cout << "== Input (press Enter on an empty name to finish) ==\n";

    std::string name;
    while (readLine("\nName: ", name)) {
        name = trim(name);
        if (name.empty()) break;

        if (cls.find(name)) {
            std::cerr << "  error: name \"" << name << "\" already exists\n";
            LOG_WARN("duplicate name: " << name);
            continue;
        }

        double height = 0, weight = 0;
        if (!askNumber("Height (cm): ", kMinHeight, kMaxHeight, "height", height)) break;
        if (!askNumber("Weight (kg): ", kMinWeight, kMaxWeight, "weight", weight)) break;

        try {
            const Student& s = cls.add(name, height, weight);
            const Averages avg = cls.averages();
            std::cout << "  -> " << s.name << ": BMI " << s.bmi << ", " << statusName(s.status)
                      << "\n  -> class of " << cls.size() << ": average height " << avg.height
                      << " cm, average weight " << avg.weight << " kg, average BMI " << avg.bmi
                      << '\n';
        } catch (const InputError& e) {
            LOG_WARN("add failed: " << e.what());
            std::cerr << "  error: " << e.what() << '\n';
        }
    }

    LOG_INFO("input finished, " << cls.size() << " students");
    if (g_interactive) std::cout << "\nInput finished, " << cls.size() << " students.\n";
}

void queryPhase(const Classroom& cls) {
    if (cls.empty()) {
        std::cout << "No data.\n";
        return;
    }

    std::string choice, line;
    while (true) {
        if (g_interactive)
            std::cout << "\n== Query ==  1) find by name  2) list by status  3) list all  0) quit\n";
        if (!readLine("Choice: ", choice)) break;
        choice = trim(choice);

        if (choice == "0") {
            break;
        } else if (choice == "1") {
            if (!readLine("Name: ", line)) break;
            LOG_INFO("query name: " << trim(line));
            if (const Student* s = cls.find(line)) {
                printStudent(*s);
            } else {
                std::cerr << "  error: \"" << trim(line) << "\" not found\n";
            }
        } else if (choice == "2") {
            if (!readLine("Status (1 underweight / 2 normal / 3 overweight): ", line)) break;
            try {
                const Status st = parseStatus(line);
                const auto list = cls.byStatus(st);
                LOG_INFO("query status: " << statusName(st) << ", " << list.size() << " found");
                std::cout << statusName(st) << ": " << list.size()
                          << (list.size() == 1 ? " student\n" : " students\n");
                for (const auto& s : list) printStudent(s);
            } catch (const InputError& e) {
                LOG_WARN("invalid query: " << e.what());
                std::cerr << "  error: " << e.what() << '\n';
            }
        } else if (choice == "3") {
            for (const auto& s : cls.all()) printStudent(s);
        } else if (!choice.empty()) {
            std::cerr << "  error: please enter 0 to 3\n";
        }
    }
}

}

int main(int argc, char* argv[]) {
    std::string logPath = "bmi.log";
    bool verbose = false;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printUsage();
            return 0;
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if ((arg == "-l" || arg == "--log") && i + 1 < argc) {
            logPath = argv[++i];
        } else {
            std::cerr << "bmi: invalid argument '" << arg << "', see bmi --help\n";
            return 2;
        }
    }

    if (!logOpen(logPath, verbose)) {
        std::cerr << "bmi: warning: cannot open log file '" << logPath << "', logging disabled\n";
    }
    LOG_INFO("program started");

    g_interactive = isatty(STDIN_FILENO);
    std::cout << std::fixed << std::setprecision(2);

    try {
        Classroom cls;
        inputPhase(cls);
        std::cin.clear();
        queryPhase(cls);
    } catch (const std::exception& e) {
        LOG_ERROR("unexpected error: " << e.what());
        std::cerr << "bmi: error: " << e.what() << '\n';
        return 1;
    }

    LOG_INFO("program exited");
    return 0;
}
