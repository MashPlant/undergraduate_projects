//////////////////////////////////////////////////////////////////////////
// Description: this is the base implementation of a simple maze router
// Author: YHL
// Date: 20120323
// Note: Modify this file and add OptRouter.cpp file for the
//       implementation of the additional functionalities
//////////////////////////////////////////////////////////////////////////
#ifndef OPT_ROUTER_H_
#define OPT_ROUTER_H_
#include "BaseRouter.h"
const int MAX = 0x0fffffff;

struct PointRecord {

  //Store the steps taken to BFS to this point
  int step;

  //Store the turns taken to reach this point, from 4 directions
  int turn[4];

  PointRecord() {
    step = MAX;
    for (auto &e: turn) {
      e = MAX;
    }
  }
  void clear() {
    step = 0;
    for (auto &e: turn) {
      e = 0;
    }
  }
  void resetTurn() {
    for (auto &e: turn) {
      e = MAX;
    }
  }

};

class OptRouter:public BaseRouter
{
 private:
  vector<PointRecord> myGrids;
  int min4(int a, int b, int c, int d) {
    a = min(a, b);
    c = min(c, d);
    return min(a, c);
  }
 public:
  OptRouter(int r, int c, const Point &source, const Point &target, const PointVector &obs);
	~OptRouter() = default;

  bool route();
  bool myMazeSearch();
  void myMazeBacktrace();
  int minTurn(int prevIndex, int direction);
};

#endif
