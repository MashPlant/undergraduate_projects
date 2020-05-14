//
// Created by Yixiang Tu on 05/05/2018.
//

#include "OptRouter.h"

const static int X[4] = {1,-1,0,0};
const static int Y[4] = {0,0,1,-1};


OptRouter::OptRouter(int r, int c, const Point &source, const Point &target, const PointVector &obs)
    : BaseRouter(r, c, source, target, obs) {
  myGrids.assign(r * c, PointRecord());
  for (auto e: obs) {
    myGrids[compIndex(e.x(), e.y())].step = OBS;
  }
}

bool OptRouter::route() {

  if (m_sourceIndex < 0 or m_targetIndex < 0 or m_row <= 0 or m_col <= 0) {
    return false;
  }
  m_path.clear();

  myGrids[m_sourceIndex].clear();

  if (myMazeSearch()) {
    myMazeBacktrace();
    /*for (int i = 0; i < 10; i++) {
      for (int j = 0; j < 10; j++) {
        printf("%d", min4(myGrids[10 * i + j].turn[0],
                          myGrids[10 * i + j].turn[1],
                          myGrids[10 * i + j].turn[2],
                          myGrids[10 * i + j].turn[3]) % 10);
      }
      printf("\n");
    }*/
    return true;
  } else {
    return false;
  }

}

bool OptRouter::myMazeSearch() {

  int stepCap = MAX;
  list<int> searchList;
  searchList.push_back(m_sourceIndex);

  do {
    int indexToSearch = searchList.front();
    if (indexToSearch == m_targetIndex) {
      stepCap = myGrids[m_targetIndex].step;
    }
    searchList.pop_front();
    if (myGrids[indexToSearch].step >= stepCap) {
      continue;
    }
    int x, y;
    compXYIndex(indexToSearch, x, y);
    printf("Searching grid [%d,%d] cost: %d\n", x, y, myGrids[indexToSearch].step);
    for (int i = 0; i < 4; i++) {
      int newX = x + X[i];
      int newY = y + Y[i];
      if (newY >= 0 and newY < m_row and newX < m_col and newX >= 0) {
        int newIndex = compIndex(newX, newY);
        if (myGrids[newIndex].step > myGrids[indexToSearch].step + 1) {
          myGrids[newIndex].resetTurn();
          myGrids[newIndex].step = myGrids[indexToSearch].step + 1;
          myGrids[newIndex].turn[i] = minTurn(indexToSearch, i);
          searchList.push_back(newIndex);
        } else if (myGrids[newIndex].step == myGrids[indexToSearch].step + 1) {
          int newTurn = minTurn(indexToSearch, i);
          if (myGrids[newIndex].turn[i] > newTurn) {
            myGrids[newIndex].turn[i] = newTurn;
            searchList.push_back(newIndex);
          }
        }
      }
    }

  } while (!searchList.empty());
  return stepCap < MAX;

}

void OptRouter::myMazeBacktrace() {
  //NOTICE: m_path will store the INVERSED path
  m_path.assign(1, m_targetIndex);
  int x, y;
  compXYIndex(m_targetIndex, x, y);
  printf("Backtracing grid [%d,%d] cost: %d\n", x, y, myGrids[m_targetIndex].step);
  int lastDirection = 0;  //The last direction the backtrace went through
  int lastTotalTurn = myGrids[m_targetIndex].turn[0];  //The turn value of the last grid traced
  for (int i = 1; i < 4; i++) {
    if (myGrids[m_targetIndex].turn[i] < myGrids[m_targetIndex].turn[lastDirection]) {
      lastDirection = i;
      lastTotalTurn = myGrids[m_targetIndex].turn[i];
    }
  }
  int currentIndex = compIndex(x - X[lastDirection], y - Y[lastDirection]);
  do {
    m_path.push_back(currentIndex);
    int thisX, thisY;
    compXYIndex(currentIndex, thisX, thisY);
    printf("Backtracing grid [%d,%d] cost: %d\n", thisX, thisY, myGrids[currentIndex].step);
    int nextDirection;  //The direction to reach currentIndex with the min turn
    for (nextDirection = 0; nextDirection < 4; nextDirection++) {
      if (nextDirection == lastDirection) {
        //No turning involved. Backtrace to a grid with the same "turn" value.
        if (myGrids[currentIndex].turn[nextDirection] == lastTotalTurn) {
          break;
        }
      } else {
        //Turning involved. Backtrace to a grid with one less "turn"
        if (myGrids[currentIndex].turn[nextDirection] == lastTotalTurn - 1) {
          break;
        }
      }
    }
    int nextX = thisX - X[nextDirection];
    int nextY = thisY - Y[nextDirection];

    currentIndex = compIndex(nextX, nextY);
  } while (myGrids[currentIndex].step);

  m_path.push_back(m_sourceIndex);

  printf("Found path: ");
  for (auto index: m_path) {
    int x, y;
    this->compXYIndex(index, x, y);
    printf("[%d,%d] ", x, y);
  }
  printf("\n");
  printf("%d turns taken.\n", min4(myGrids[m_targetIndex].turn[0],
                                   myGrids[m_targetIndex].turn[1],
                                   myGrids[m_targetIndex].turn[2],
                                   myGrids[m_targetIndex].turn[3]));
}

int OptRouter::minTurn(int prevIndex, int direction) {
  int result = MAX;
  for (int i = 0; i < 4; i++) {
    if (i == direction) {
      //No turning involved
      result = min(result, myGrids[prevIndex].turn[i]);
    } else {
      //Make a turn
      result = min(result, myGrids[prevIndex].turn[i] + 1);
    }
  }
  return result;
}
