#include "Strategy.h"
#include "Point.h"
#include "Policy.h"
#include <chrono>
#include <iostream>

using namespace std;

const u32 MAX_ITER = 7500000;

char policy[sizeof(Policy<12, 12>) + MAX_ITER * sizeof(Node<12, 12>)];


extern "C" __declspec(dllexport) Point *getPoint(const int M, const int N, const int *top, const int *_board, const int lastX, const int lastY, const int noX, const int noY) {
  std::pair<u32, u32> pr;
  ((u32 *)policy)[0] = noX, ((u32 *)policy)[1] = noY;
  switch (M) {
  case 9:
    switch (N) {
    case 9:
      pr = ((Policy<9, 9> *)policy)->mcts(Board<9, 9>(_board, lastX, lastY), MAX_ITER);
      break;
    case 10:
      pr = ((Policy<9, 10> *)policy)->mcts(Board<9, 10>(_board, lastX, lastY), MAX_ITER);
      break;
    case 11:
      pr = ((Policy<9, 11> *)policy)->mcts(Board<9, 11>(_board, lastX, lastY), MAX_ITER);
      break;
    case 12:
      pr = ((Policy<9, 12> *)policy)->mcts(Board<9, 12>(_board, lastX, lastY), MAX_ITER);
      break;
    }
    break;
  case 10:
    switch (N) {
    case 9:
      pr = ((Policy<10, 9> *)policy)->mcts(Board<10, 9>(_board, lastX, lastY), MAX_ITER);
      break;
    case 10:
      pr = ((Policy<10, 10> *)policy)->mcts(Board<10, 10>(_board, lastX, lastY), MAX_ITER);
      break;
    case 11:
      pr = ((Policy<10, 11> *)policy)->mcts(Board<10, 11>(_board, lastX, lastY), MAX_ITER);
      break;
    case 12:
      pr = ((Policy<10, 12> *)policy)->mcts(Board<10, 12>(_board, lastX, lastY), MAX_ITER);
      break;
    }
    break;
  case 11:
    switch (N) {
    case 9:
      pr = ((Policy<11, 9> *)policy)->mcts(Board<11, 9>(_board, lastX, lastY), MAX_ITER);
      break;
    case 10:
      pr = ((Policy<11, 10> *)policy)->mcts(Board<11, 10>(_board, lastX, lastY), MAX_ITER);
      break;
    case 11:
      pr = ((Policy<11, 11> *)policy)->mcts(Board<11, 11>(_board, lastX, lastY), MAX_ITER);
      break;
    case 12:
      pr = ((Policy<11, 12> *)policy)->mcts(Board<11, 12>(_board, lastX, lastY), MAX_ITER);
      break;
    }
    break;
  case 12:
    switch (N) {
    case 9:
      pr = ((Policy<12, 9> *)policy)->mcts(Board<12, 9>(_board, lastX, lastY), MAX_ITER);
      break;
    case 10:
      pr = ((Policy<12, 10> *)policy)->mcts(Board<12, 10>(_board, lastX, lastY), MAX_ITER);
      break;
    case 11:
      pr = ((Policy<12, 11> *)policy)->mcts(Board<12, 11>(_board, lastX, lastY), MAX_ITER);
      break;
    case 12:
      pr = ((Policy<12, 12> *)policy)->mcts(Board<12, 12>(_board, lastX, lastY), MAX_ITER);
      break;
    }
    break;
  }
  return new Point(pr.first - 1, pr.second);
}

extern "C" __declspec(dllexport) void clearPoint(Point *p) {
  delete p;
  return;
}

void clearArray(int M, int N, int **board) {
  for (int i = 0; i < M; i++) {
    delete[] board[i];
  }
  delete[] board;
}
