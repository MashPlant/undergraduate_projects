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
    cout << matcher.match(17) << endl;
    cout << "======= public composition match =======" << endl;
    {
        vector<int> matches;
        PublicCompositionMatch PubCM("pat.txt", "array.txt");
        //fill in the code to compute all the matching indexes
        //you can just use the public kmp like this
        /*
        int j = -1;
        while(PubCM.kmp.match(j + 1) != -1)
        {
            j = PubCm.kmp.match(j + 1);
            matches.push_back(j);
        }
        */
        PubCM.matchAll();
        matches = PubCM.GetMatches();

        cout << "pSize =" << PubCM.GetpSize() << " "
             << "aSize =" << PubCM.GetaSize() << endl;

        cout << "Match indexes:";
        if(matches.size() == 0)
            cout << "No resualt" << endl;
        else
        {
            for (unsigned i = 0; i < matches.size(); ++i)
            {
                cout << " " << matches[i];
            }
            cout << endl;
        }
        cout << endl;

        PubCM.ReloadInput("pattern.txt", "array.txt");
        PubCM.matchAll();
        matches = PubCM.GetMatches();

        cout << "pSize =" << PubCM.GetpSize() << " "
             << "aSize =" << PubCM.GetaSize() << endl;

        cout << "Match indexes:";
        if (matches.size() == 0)
            cout << "No resualt" << endl;
        else
        {
            for (unsigned i = 0; i < matches.size(); ++i)
            {
                cout << " " << matches[i];
            }
            cout << endl;
        }
    }
    cout << endl;
    cout << "======= private composition match =======" << endl;
    {
        vector<int> matches;
        PrivateCompositionMatch PrvCM("pat.txt", "array.txt");
        //fill in the code to compute all the matching indexes
        PrvCM.matchAll();
        matches = PrvCM.GetMatches();

        cout << "pSize =" << PrvCM.GetpSize() << " "
             << "aSize =" << PrvCM.GetaSize() << endl;

        cout << "Match indexes:";
        if (matches.size() == 0)
            cout << "No resualt" << endl;
        else
        {
            for (unsigned i = 0; i < matches.size(); ++i)
            {
                cout << " " << matches[i];
            }
            cout << endl;
        }
        cout << endl;

        PrvCM.ReloadInput("pattern.txt", "array.txt");
        PrvCM.matchAll();
        matches = PrvCM.GetMatches();

        cout << "pSize =" << PrvCM.GetpSize() << " "
             << "aSize =" << PrvCM.GetaSize() << endl;

        cout << "Match indexes:";
        if (matches.size() == 0)
            cout << "No resualt" << endl;
        else
        {
            for (unsigned i = 0; i < matches.size(); ++i)
            {
                cout << " " << matches[i];
            }
            cout << endl;
        }
    }
    cout << endl;
    cout << "======= public inheritance match ======= " << endl;
    {
        vector<int> matches;
        PublicInheritanceMatch PubIM("pat.txt", "array.txt");
        //fill in the code to compute all the matching indexes
        PubIM.matchAll();
        matches = PubIM.GetMatches();

        cout << "pSize =" << PubIM.GetpSize() << " "
             << "aSize =" << PubIM.GetaSize() << endl;

        cout << "Match indexes:";
        if (matches.size() == 0)
            cout << "No resualt" << endl;
        else
        {
            for (unsigned i = 0; i < matches.size(); ++i)
            {
                cout << " " << matches[i];
            }
            cout << endl;
        }
        cout << endl;
        PubIM.ReloadInput("pattern.txt", "array.txt");
        PubIM.matchAll();
        matches = PubIM.GetMatches();

        cout << "pSize =" << PubIM.GetpSize() << " "
             << "aSize =" << PubIM.GetaSize() << endl;

        cout << "Match indexes:";
        if (matches.size() == 0)
            cout << "No resualt" << endl;
        else
        {
            for (unsigned i = 0; i < matches.size(); ++i)
            {
                cout << " " << matches[i];
            }
            cout << endl;
        }
    }
    cout << endl;
    cout << "======= private inheritance match =======" << endl;
    {
        vector<int> matches;
        PrivateInheritanceMatch PrvIM("pat.txt", "array.txt");
        //fill in the code to compute all the matching indexes
        PrvIM.matchAll();
        matches = PrvIM.GetMatches();

        cout << "pSize =" << PrvIM.GetpSize() << " "
             << "aSize =" << PrvIM.GetaSize() << endl;

        cout << "Match indexes:";
        if (matches.size() == 0)
            cout << "No resualt" << endl;
        else
        {
            for (unsigned i = 0; i < matches.size(); ++i)
            {
                cout << " " << matches[i];
            }
            cout << endl;
        }
        cout << endl;

        PrvIM.ReloadInput("pattern.txt", "array.txt");
        PrvIM.matchAll();
        matches = PrvIM.GetMatches();

        cout << "pSize =" << PrvIM.GetpSize() << " "
             << "aSize =" << PrvIM.GetaSize() << endl;

        cout << "Match indexes:";
        if (matches.size() == 0)
            cout << "No resualt" << endl;
        else
        {
            for (unsigned i = 0; i < matches.size(); ++i)
            {
                cout << " " << matches[i];
            }
            cout << endl;
        }
    }

    return 0;
}
