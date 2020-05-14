//
// Created by mashplant on 18-8-27.
//

#ifndef MCS_SOLVER_HPP
#define MCS_SOLVER_HPP

#include <vector>

// return a vector of SAME size as length (rather than 3)
// the last 3 element of which is the outflow speed
std::vector<double> caluconspeed(int num,
                                 const std::vector<double> &length,
                                 int i1, int i2,
                                 int o1, int o2, int o3);

std::vector<double> sa(int num,
                       int i1, int i2,
                       int o1, int o2, int o3,
                       double exp1, double exp2, double exp3,
                       int sa_times);

#endif //MCS_SOLVER_HPP
