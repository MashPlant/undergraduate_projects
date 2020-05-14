#include <iostream>
#include "KMP.h"
#include "PublicCompositionMatch.h"
#include "PrivateCompositionMatch.h"
#include "PublicInheritanceMatch.h"
#include "PrivateInheritanceMatch.h"

// NOTE: the missing header files and source files need to be implemented for compiling the program
// The KMP lib only finds one match, that code needs to be reused to find all the matches

using namespace std;

int main()
{
    KMP matcher("pattern.txt", "array.txt");
    cout << matcher.match(15) << endl;
    cout << "======= public composition match =======" << endl;
    {
        vector<int> matches;
        PublicCompositionMatch PubCM("pattern.txt", "array.txt");
        matches = PubCM.getAllMatches();

        cout << "Match indexes:";
        for (unsigned i = 0; i < matches.size(); ++ i)
        {
            cout << " " << matches[i];
        }
        cout << endl;
    }
    cout << "======= private composition match =======" << endl;
    {
        vector<int> matches;
        PrivateCompositionMatch PrvCM("pattern.txt", "array.txt");
        matches = PrvCM.getAllMatches();

        cout << "Match indexes:";
        for (unsigned i = 0; i < matches.size(); ++ i)
        {
            cout << " " << matches[i];
        }
        cout << endl;
    }

    cout << "======= public inheritance match ======= " << endl;
    {
        vector<int> matches;
        PublicInheritanceMatch PubIM("pattern.txt", "array.txt");
        matches = PubIM.getAllMatches();

        cout << "Match indexes:";
        for (unsigned i = 0; i < matches.size(); ++ i)
        {
            cout << " " << matches[i];
        }
        cout << endl;
    }

    cout << "======= private inheritance match =======" << endl;
    {
        vector<int> matches;
        PrivateInheritanceMatch PrvIM("pattern.txt", "array.txt");
        matches = PrvIM.getAllMatches();

        cout << "Match indexes:";
        for (unsigned i = 0; i < matches.size(); ++ i)
        {
            cout << " " << matches[i];
        }
        cout << endl;
    }

    return 0;
}
