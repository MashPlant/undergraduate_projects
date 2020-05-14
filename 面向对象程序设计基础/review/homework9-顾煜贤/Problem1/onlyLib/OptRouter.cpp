#include "OptRouter.h"

#include <iostream>

const static int dX[4] = {1, 0, -1 ,0};     //the for directions to move
const static int dY[4] = {0 ,-1, 0, 1};
const static int dTurn[3] = {0, 1 ,-1}; //there are 3 ways to turn, not turn, turn CW, turn CCW
const static int increaseTurns[3] = {0, 1, 1};   //if not turn, turns doesn't increase, if turn, increase by 1

void OptRouter::recurBackTrace(int d, int turns, int index) {
  //do the traceback recursively
  if (turns >= m_minTurns) return;
  else if (index == m_sourceIndex) {
    m_minTurns = turns;
    m_minTurnsPath = m_tempMinTurnsPath;
    return;
  }
  int x = 0, y = 0;
  compXYIndex(index, x, y);
  for (int i = 0; i < 3; i++) {
    int newd = (d + dTurn[i]) % 4;
    int newTurns = turns;
    if (index != m_targetIndex) newTurns += increaseTurns[i];   //Won't count the turns at the begining.
    int newX = x + dX[newd], newY = y + dY[newd];
    int newIndex = compIndex(newX, newY);
    if (newX >= 0 && newX < m_col && newY >= 0 && newY < m_row && (m_grids[newIndex] == m_grids[index] - 1)) {
      m_tempMinTurnsPath.push_back(newIndex);
      recurBackTrace(newd, newTurns, newIndex);
      m_tempMinTurnsPath.pop_back();
    }
  }
}

void OptRouter::mazeBacktrace() {
  m_tempMinTurnsPath.push_back(m_targetIndex);
  recurBackTrace(0, 0, m_targetIndex);
}

void OptRouter::showMinTurnPath() {
  if (m_foundRoute) {
    std::cout << "The minimal turns: " << m_minTurns << "\n";
    std::cout << "The minimal turns path: \n";
    for (int i = 0; i < m_minTurnsPath.size(); i++) {
      int x = 0, y = 0;
      compXYIndex(m_minTurnsPath[i], x, y);
      std::cout << "(" << x << ", " << y << ") ";
    }
  }
  else std::cout << "No solution\n";
}

bool OptRouter::route() {
  //only do traceback if there is a route solution
  if (this->m_sourceIndex < 0 || this->m_targetIndex < 0 || this->m_row <= 0 || this->m_col <= 0)
		return false;

	this->m_path.clear();

	//initialize the cost of the source grid
	m_grids[this->m_sourceIndex] = 0;

    //maze expansion
	if (m_foundRoute = mazeSearch())
	{
		mazeBacktrace();
		return true;
	}
	else
		return false;
}
