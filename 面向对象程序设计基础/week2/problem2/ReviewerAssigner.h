#ifndef REVIEWERASSIGNER_H
#define REVIEWERASSIGNER_H
#include <algorithm>
#include <fstream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class ReviewerAssigner
{
    using Data = std::pair<std::string, std::string>;
    std::vector<Data> lst;

  public:
    void load(const std::string &file = "ContactEmail.txt")
    {
        std::ifstream fin(file);
        if (!fin)
            throw std::runtime_error("fail to open " + file);
        Data dt;
        fin >> dt.first >> dt.second;
        if (dt.first != "StudentNo." || dt.second != "Email")
            throw std::runtime_error("not the correct format " + dt.first + " " + dt.second);
        while (fin >> dt.first >> dt.second)
            lst.push_back(dt);
    }
    std::vector<std::vector<Data>> choose() const
    {
        std::vector<std::vector<Data>> ret(lst.size());
        int rnd;
        for (int i = 0; i < ret.size(); ++i)
            while (ret[i].size() != 3)
                if ((rnd = rand() % lst.size()) != i && find(ret[i].begin(), ret[i].end(), lst[rnd]) == ret[i].end())
                    ret[i].push_back(lst[rnd]);
        return ret;
    }
    void output(const std::string &file = "reviewer.txt") const
    {
        const auto &out = choose();
        std::ofstream fout(file);
        if (!fout)
            throw std::runtime_error("fail to open " + file);
        for (int i = 0; i < out.size(); ++i)
        {
            fout << lst[i].first << " reviewed by : ";
            for (int j = 0; j < out[i].size(); ++j)
                fout << out[i][j].first << '\t';
            fout << std::endl;
        }
    }
};
#endif