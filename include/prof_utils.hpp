#pragma once

#include <chrono>
#include <iostream>

#define PROF_EXEC_TIME \
    Profiler profiler(__FUNCTION__);

class Profiler {
public:
    explicit Profiler(const char* funcName)
        : functionName(funcName), startTime(std::chrono::high_resolution_clock::now()) {}

    ~Profiler() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        std::cout << "Execution time of " << functionName << ": " << duration << " microseconds" << std::endl;
    }

private:
    const char* functionName;
    std::chrono::high_resolution_clock::time_point startTime;
};
