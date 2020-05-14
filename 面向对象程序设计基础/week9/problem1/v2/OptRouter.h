//////////////////////////////////////////////////////////////////////////
// Description: this is the base implementation of a simple maze router
// Author: YHL
// Date: 20120323
// Note: Modify this file and add OptRouter.cpp file for the
//       implementation of the additional functionalities
//////////////////////////////////////////////////////////////////////////
#ifndef OPT_ROUTER_H_
#define OPT_ROUTER_H_

#include <cstdio>
#include <cstring>
#include <algorithm>
#include <memory>
#include "BaseRouter.h"

class OptRouter : public BaseRouter
{
    int dfs(int index, int face, int *memo, int *path);

  public:
    OptRouter(int r, int c, const Point &source, const Point &target, const PointVector &obs)
        : BaseRouter(r, c, source, target, obs) {}
    bool route();
    void mazeBacktrace();
};

#endif
