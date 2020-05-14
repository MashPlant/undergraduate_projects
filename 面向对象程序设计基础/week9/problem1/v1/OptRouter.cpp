#include "OptRouter.h"

const static int X[4] = {1, -1, 0, 0};
const static int Y[4] = {0, 0, 1, -1};
const static int inf = 0x3f3f3f3f;

//BaseRouter中的route函数的copy
//因为mazeBacktrace不是虚函数,而且BaseRouter不能修改,所以route函数也必须重写一遍
bool OptRouter::route()
{
	if (m_sourceIndex < 0 || m_targetIndex < 0 || m_row <= 0 || m_col <= 0)
		return false;

	m_path.clear();

	//initialize the cost of the source grid
	m_grids[m_sourceIndex] = 0;

	//maze expansion
	if (mazeSearch())
	{
		mazeBacktrace();
		return true;
	}
	else
		return false;
}

void OptRouter::mazeBacktrace()
{
	const int searchSize = m_col * m_row * 4;
	std::unique_ptr<int[]> memo(new int[searchSize]), path(new int[m_col * m_row]);
	std::fill(memo.get(), memo.get() + searchSize, inf);
	int minTurn = inf;
	for (int f = 0; f < 4; ++f)
		minTurn = std::min(minTurn, dfs(m_targetIndex, f, memo.get(), path.get()));
	int cur = m_targetIndex;
	while (true)
	{
		int x, y;
		m_path.push_back(cur);
		if (cur == m_sourceIndex)
			break;
		compXYIndex(cur, x, y);
		printf("BackTrace grid [%d,%d] cost %d\n", x, y, m_grids[cur]);
		x += X[path[cur]], y += Y[path[cur]];
		cur = compIndex(x, y);
	}
	printf("Found path: ");
	for (auto i : m_path)
	{
		int x, y;
		compXYIndex(i, x, y);
		printf("[%d,%d] ", x, y);
	}
}

int OptRouter::dfs(int index, int face, int *memo, int *path)
{
	if (index == m_sourceIndex)
		return 0;
	if (memo[index * 4 + face] != inf)
		return memo[index * 4 + face];
	int curCost = m_grids[index];
	int x, y;
	compXYIndex(index, x, y);
	int curMin = inf;
	for (int i = 0; i < 4; ++i)
	{
		int newX = x + X[i];
		int newY = y + Y[i];
		if (newY >= 0 && newY < m_row && newX < m_col)
		{
			int newIndex = compIndex(newX, newY), checkMin;
			if (m_grids[newIndex] + 1 == curCost)
				if (curMin > (checkMin = dfs(newIndex, i, memo, path) + (i != face)))
					curMin = checkMin, path[index] = i;
		}
	}
	return memo[index * 4 + face] = curMin;
}