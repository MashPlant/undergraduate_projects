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
        PublicCompositionMatch PubCM("pat.txt", "array.txt");
        //fill in the code to compute all the matching indexes
        PubCM.get_match(matches);

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
        PrivateCompositionMatch PrvCM("pat.txt", "array.txt");
        //fill in the code to compute all the matching indexes
        PrvCM.get_match(matches);

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
        PublicInheritanceMatch PubIM("pat.txt", "array.txt");
        //fill in the code to compute all the matching indexes
        PubIM.get_match(matches);

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
        PrivateInheritanceMatch PrvIM("pat.txt", "array.txt");
        //fill in the code to compute all the matching indexes
        PrvIM.get_match(matches);

        cout << "Match indexes:";
        for (unsigned i = 0; i < matches.size(); ++ i)
        {
            cout << " " << matches[i];
        }
        cout << endl;
    }

    return 0;
}
