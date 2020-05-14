#ifndef PRED_H
#define PRED_H

#include <cstring>

struct PredI
{
    bool operator()(const char *lhs, const char *rhs) const 
    {
        return strcmp(lhs, rhs) < 0;
    }
};

struct PredND
{
    bool operator()(const char *lhs, const char *rhs) const 
    {
        return strcmp(lhs, rhs) <= 0;
    }
};

#endif // PRED_H