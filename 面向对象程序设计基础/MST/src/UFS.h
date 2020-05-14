//
// Created by MashPlant on 18-5-7.
//

#ifndef MST_UFS_H
#define MST_UFS_H

#include <memory>

class UFS
{
	const int N;
	std::unique_ptr<int[]> fa;

	int find(int x)
	{ return x == fa[x] ? x : fa[x] = find(fa[x]); }

public:
	void reset()
	{
		for (int i = 0; i < N; ++i)
			fa[i] = i;
	}

	UFS(int N) : N(N), fa(std::make_unique<int[]>(N))
	{ reset(); }

	bool connected(int x, int y)
	{ return find(x) == find(y); }

	void connect(int x,int y)
	{
		x = find(x),y = find(y);
		if (x != y)
			fa[x] = y;
	}

};


#endif //MST_UFS_H
