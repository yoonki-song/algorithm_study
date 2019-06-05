#include <memory>
#include <vector>
#include <algorithm>

using namespace std;

#include "sortHeap.h"
#include "sortMerge.h"
#include "sortQuick.h"

/////////// For Testing ///////////////////////////////////////////////////////

#include <cassert>
#include <string>
#include <iostream>
#include "../common/iostreamhelper.h"
#include "../common/profile.h"
#include "../common/rand.h"

void testSort() {
    //return; //TODO: if you want to test, make this line a comment.

    cout << "--- sort ---------------------------------" << endl;
    {
        int N = 100000;
        int T = 100;

#ifdef _DEBUG
        N = 1000;
        T = 10;
#endif

        vector<int> vin(N);
        for (int i = 0; i < N; i++)
            vin[i] = RandInt32::get();

        for (int i = 0; i < T; i++) {
            auto vout1 = vin;
            MaxHeap<int>::sort(vout1);

            auto vout2 = vin;
            MergeSort<int>::sort(vout2);

            auto vout3 = vin;
            QuickSort<int>::sort(vout3);

            if (vout1 != vout2 || vout1 != vout3)
                cout << "ERROR : mismatched!" << endl;

            assert(vout1 == vout2);
            assert(vout1 == vout3);
        }
    }
    cout << "OK!" << endl;
    cout << "--- speed test" << endl;
    {
        int N = 100000;
        int T = 100;

#ifdef _DEBUG
        N = 1000;
        T = 10;
#endif

        vector<int> vin(N);
        for (int i = 0; i < N; i++)
            vin[i] = RandInt32::get();

        cout << "Heap sort..." << endl;
        PROFILE_START(0);
        auto vout1 = vin;
        for (int i = 0; i < T; i++) {
            vout1 = vin;
            MaxHeap<int>::sort(vout1);
        }
        PROFILE_STOP(0);

        cout << "Merge sort..." << endl;
        PROFILE_START(1);
        auto vout2 = vin;
        for (int i = 0; i < T; i++) {
            vout2 = vin;
            MergeSort<int>::sort(vout2);
        }
        PROFILE_STOP(1);

        cout << "Quick sort..." << endl;
        PROFILE_START(2);
        auto vout3 = vin;
        for (int i = 0; i < T; i++) {
            vout3 = vin;
            QuickSort<int>::sort(vout3);
        }
        PROFILE_STOP(2);
    }
    cout << "OK!" << endl;
}
