/*
* @Author: Mockingjay
* @Date:   2018-05-21 22:17:59
* @Last Modified by:   Mockingjay
* @Last Modified time: 2018-05-22 10:33:56
*/
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cstdio>
#include "PublicInheritanceMatch.h"
#include "naive_algo.h"
#include <ctime>

using namespace std;

#define MAX_TAR 10
#define MAX_SRC 20000000
/**
 * [data_gen description]
 * for randomly generate the data
 * @param seed [description]
 * the random seed
 */
void data_gen(const long long &seed)
{
    srand(seed);
    FILE *fp = freopen("pattern.txt", "w", stdout);
    int t = 0;
    for (int i = 0; i < MAX_TAR; ++i)
    {
        t = rand() & 1;
        printf("%d ", t);
    }
    fclose(fp);

    fp = freopen("array.txt", "w", stdout);
    for (int i = 0; i < MAX_SRC; ++i)
    {
        t = rand() & 1;
        printf("%d ", t);
    }
    fclose(fp);
    //freopen("CON","w",stdout);
}

int main(int argc, char const *argv[])
{
    data_gen(time(0));

    double time1 = 0, time2 = 0;

    //QueryPerformanceFrequency(&nFreq);          //initializing time counter.

    cerr << "Now is the naive algorithm\n";
    {
        vector<int> matches;
        naive_algo NaiAl("pattern.txt", "array.txt");

        auto beg = clock();
        NaiAl.get_match(matches);
        time1 = clock() - beg;
        
        cerr << endl;
    }
    cerr << "The naive algorithm takes " << time1 << "s to finish\n";
    cerr << "**************************************************\n";

    cerr << "Now is the KMP algorithm\n";
    {
        vector<int> matches;
        PublicInheritanceMatch PubIM("pattern.txt", "array.txt");

        auto beg = clock();
        PubIM.get_match(matches);
        time2 = clock() - beg;
        //QueryPerformanceCounter(&nEndTime);
        //culculate the runtime
        //time2 = (double)(nEndTime.QuadPart-nBeginTime.QuadPart) / (double)nFreq.QuadPart;

        cerr << endl;
    }
    cerr << "The KMP algorithm takes " << time2 << "s to finish\n";
    cerr << "**************************************************\n";

    //cerr << "The improvement is about " << (time1 - time2) / time1 * 100 << "%.\n";
    return 0;
}
