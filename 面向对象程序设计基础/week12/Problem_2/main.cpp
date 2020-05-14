#include <fstream>
#include <iostream>
#include <random>
#include <chrono>
#include <ctime>

#include "HashFinder.h"
#include "KMPFinder.h"

const int PatternLen = 10, ArrayLen = 1e8;
const std::string ArrayFile = "arrar.txt", PatternFile = "pattern.txt";
const auto &now = std::chrono::system_clock::now;

using TimeUnit = decltype((now() - now()).count());

void generate(const std::string &fileName, int N)
{
    static std::mt19937 mt(time(nullptr));
    std::uniform_int_distribution<> dis(0, 1);
    std::ofstream fout(fileName);
    for (int i = 0; i < N; ++i)
        fout << dis(mt) << ' ';
    fout << std::flush;
}

template <typename Func, typename... Args>
TimeUnit getRunTime(Func f, Args &&... args)
{
    auto beg = now();
    f(std::forward<Args>(args)...);
    return (now() - beg).count();
}

int main()
{
    std::cout << "generating pattern with length of " + std::to_string(PatternLen) << "\n\n";
    generate(PatternFile, PatternLen);
    std::cout << "generating array with length of " + std::to_string(ArrayLen) << "\n\n";
    generate(ArrayFile, ArrayLen);

    std::vector<int> res[2];
    TimeUnit matchTime[2] = {getRunTime([&]() { res[0] = KMPFinder(PatternFile, ArrayFile).getAllMatches(); }),
                             getRunTime([&]() { res[1] = HashFinder(PatternFile, ArrayFile).getAllMatches(); })};
    TimeUnit readTime = getRunTime([&]() 
    {
        std::vector<int> pattern, array;
        std::ifstream fin(PatternFile);
        int val;
        while (fin >> val)
            pattern.push_back(val);
        fin.open(ArrayFile);
        while (fin >> val)
            array.push_back(val);
    });

    matchTime[0] -= readTime, matchTime[1] -= readTime;

    std::cout << "KMP time use: " << matchTime[0] << '\n'
              << "Hash time use: " << matchTime[1] << '\n'
              << "KMP improvement: " << static_cast<double>(matchTime[1] - matchTime[0]) / matchTime[1] << '\n'
              << "correct? " << std::boolalpha << (res[0] == res[1]) << '\n';
}