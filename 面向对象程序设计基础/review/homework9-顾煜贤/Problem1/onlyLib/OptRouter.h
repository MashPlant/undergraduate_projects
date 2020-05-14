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
#include <vector>
using std::vector;

class OptRouter : public BaseRouter {
private:
  int m_minTurns;    //the number of turns
  vector<int> m_minTurnsPath;   //to store the minimal path
  vector<int> m_tempMinTurnsPath; //store the temp path in every recursive finding
  bool m_foundRoute;         //if the maze has a solution
  void recurBackTrace (int direction, int turns, int index);   //find the path recursively
public:
  OptRouter(int r, int c, const Point &source, const Point &target, const PointVector &obs)
        : BaseRouter(r, c, source, target, obs), m_foundRoute(false), m_minTurns(0x7fffffff)
    {
    }
  void mazeBacktrace();            //overriding the function mazeBacktrace()
  void showMinTurnPath();         // show the result of the path
  bool route() ;                  //compute the minimal turns path
	~OptRouter() {}
};

#endif
