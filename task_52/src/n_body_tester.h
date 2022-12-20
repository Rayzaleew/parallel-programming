#ifndef OPEN_MP_N_BODY_TESTER_H_
#define OPEN_MP_N_BODY_TESTER_H_

#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

#include "space_time.h"
#include "timer.h"

class NBodyTester
{
public:
    void TestCompare();
    void SetBodyCount(unsigned num) { body_count_ = num; }
    void SetTimeMoment(unsigned num) { time_moment_ = num; }
    void SaveState(const std::vector<Body>&, std::string);

    std::vector<Body> GenerateBodies(unsigned num);

private:
    const int kBodyCountDefault = 1000;
    const int kTimeMomentDefault = 10;
    int body_count_ = -1;
    int time_moment_ = -1;
};

#endif // OPEN_MP_N_BODY_TESTER_H_