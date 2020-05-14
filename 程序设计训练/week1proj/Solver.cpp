//
// Created by mashplant on 18-8-27.
//

#include <cstdio>
#include <cmath>
#include <algorithm>

#include <vector>
#include <Eigen/Dense>

#define rep(i, r) for(int i=0; i<r; i++)

#define eps 1e-6

int num, EDGENUM, inputID[2], outputID[3];
double R[128];

inline int getID(int x, int y)
{ return x * num + y; }

inline double Left(int x, int y, int &ID)
{
    if (x == 0) return 0;
    ID = getID(x - 1, y);
    return R[num * (num - 1) + (x - 1) * num + y];
}

inline double Right(int x, int y, int &ID)
{
    if (x == num - 1) return 0;
    ID = getID(x + 1, y);
    return R[num * (num - 1) + x * num + y];
}

inline double Up(int x, int y, int &ID)
{
    if (y == 0)
    {
        if (x != inputID[0] && x != inputID[1]) return 0;
        int tmp = (x == inputID[1]);
        ID = num * num + tmp;
        return R[EDGENUM - 5 + tmp];
    }
    ID = getID(x, y - 1);
    return R[x * (num - 1) + y - 1];
}

inline double Down(int x, int y, int &ID)
{
    if (y == num - 1)
    {
        if (x != outputID[0] && x != outputID[1] && x != outputID[2]) return 0;
        int tmp = (x == outputID[0] ? 0 : (x == outputID[1] ? 1 : 2));
        ID = num * num + 3;
        return R[EDGENUM - 3 + tmp];
    }
    ID = getID(x, y + 1);
    return R[x * (num - 1) + y];
}

Eigen::VectorXd solve()
{
    Eigen::MatrixXd Mat(100, 100);
    Mat.setZero();
    int MatSize = num * num + 2;
    int ID, itID;
    double r;
    rep(i, num) rep(j, num)
        {
            itID = getID(i, j);
            if ((r = Left(i, j, ID)) > eps)
                Mat(itID, ID) -= 1.0 / r, Mat(itID, itID) += 1.0 / r;
            if ((r = Right(i, j, ID)) > eps)
                Mat(itID, ID) -= 1.0 / r, Mat(itID, itID) += 1.0 / r;
            if ((r = Up(i, j, ID)) > eps)
                Mat(itID, ID) -= 1.0 / r, Mat(itID, itID) += 1.0 / r;
            if ((r = Down(i, j, ID)) > eps)
                Mat(itID, ID) -= 1.0 / r, Mat(itID, itID) += 1.0 / r;
        }
    Mat(num * num, num * num + 2) -= 200;
    Mat(num * num, num * num) += 1.0 / R[EDGENUM - 5];
    Mat(num * num, getID(inputID[0], 0)) -= 1.0 / R[EDGENUM - 5];
    Mat(num * num + 1, num * num + 2) -= 200;
    Mat(num * num + 1, num * num + 1) += 1.0 / R[EDGENUM - 4];
    Mat(num * num + 1, getID(inputID[1], 0)) -= 1.0 / R[EDGENUM - 4];
    Eigen::VectorXd B = Mat.col(MatSize);
    B.conservativeResize(MatSize);
    Mat.conservativeResize(MatSize, MatSize);

    return Mat.lu().solve(B);
}

std::vector<double> caluconspeed(int _num, const std::vector<double> &length, int i1, int i2, int o1, int o2, int o3)
{
    num = _num;
    EDGENUM = (num - 1) * num * 2 + 5;
    rep(i, EDGENUM) R[i] = length[i];

    inputID[0] = i1;
    inputID[1] = i2;
    outputID[0] = o1;
    outputID[1] = o2;
    outputID[2] = o3;

    Eigen::VectorXd V = solve();

    std::vector<double> ans;
    rep(i, num) rep(j, num - 1) if (R[i * (num - 1) + j] > eps)
                ans.push_back(fabs(V[getID(i, j)] - V[getID(i, j + 1)]) / R[i * (num - 1) + j]);
            else ans.push_back(0);
    rep(i, num - 1) rep(j, num) if (R[num * (num - 1) + i * num + j] > eps)
                ans.push_back(fabs(V[getID(i, j)] - V[getID(i + 1, j)]) / R[num * (num - 1) + i * num + j]);
            else ans.push_back(0);
    ans.push_back(200);
    ans.push_back(200);
    ans.push_back(fabs(V[getID(outputID[0], num - 1)]) / R[(num - 1) * num * 2 + 2]);
    ans.push_back(fabs(V[getID(outputID[1], num - 1)]) / R[(num - 1) * num * 2 + 3]);
    ans.push_back(fabs(V[getID(outputID[2], num - 1)]) / R[(num - 1) * num * 2 + 4]);
    return ans;
}

std::vector<double> sa(int num,
                       int i1, int i2,
                       int o1, int o2, int o3,
                       double eps1, double eps2, double eps3,
                       int sa_times)
{
    auto get_error = [=](const std::vector<double> &cur_ans)
    {
        auto res = caluconspeed(num, cur_ans, i1, i2, o1, o2, o3);
        double res1 = res.end()[-3], res2 = res.end()[-2], res3 = res.end()[-1];
        return (res1 - eps1) * (res1 - eps1) + (res2 - eps2) * (res2 - eps2) + (res3 - eps3) * (res3 - eps3);
    };
    auto check = [](double x) { return x > 200.0 ? 200.0 : x < 20.0 ? 0.0 : x; };
    const int N = 2 * num * num - 2 * num + 5;
    const double inv_rand_max = 1.0 / RAND_MAX;
    std::vector<double> ans(N, 200);
    double res = 1e9;
    for (int i = 0; i < sa_times; ++i)
    {
        std::vector<double> cur_ans = ans;
        double cur_res = res;
        double t = 1;
        while (t > 1e-10)
        {
            int which = rand() % N;
            double delta = ((rand() << 1) - RAND_MAX) * t;
            double tmp = cur_ans[which];
            cur_ans[which] = check(cur_ans[which] + delta);
            double n_res = get_error(cur_ans);
            if (n_res < cur_res || exp((cur_res - n_res) / t) > rand() * inv_rand_max)
                cur_res = n_res;
            else
                cur_ans[which] = tmp;
            t *= 0.99;
        }
        if (cur_res < res)
            ans = cur_ans, res = cur_res;
    }
    return ans;
}

