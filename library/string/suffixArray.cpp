#include <tuple>
#include <queue>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

#include "suffixArray.h"
#include "suffixArrayAlgo.h"

#include "suffixArray_LarssonSadakane.h"

/////////// For Testing ///////////////////////////////////////////////////////

#include <cassert>
#include <iostream>
#include "../common/iostreamhelper.h"
#include "../common/profile.h"
#include "../common/rand.h"

static string makeRandomString(int n) {
    string s;
    s.reserve(n);
    for (int i = 0; i < n; i++)
        s.push_back(RandInt32::get() % 26 + 'a');
    return s;
}

static string makeRandomString(int n, int charCnt) {
    string s;
    s.reserve(n);
    for (int i = 0; i < n; i++)
        s.push_back(RandInt32::get() % charCnt + 'a');
    return s;
}

static int lowerBoundForwardSlow(const vector<int>& sa, int left, int length, const string& s) {
    for (int i = left + 1; i < int(sa.size()); i++) {
        if (SuffixArrayAlgo::commonPrefixNaive(s, sa[left], sa[i]) <= length)
            return i;
    }

    return int(sa.size());
}

static int lowerBoundBackwardSlow(const vector<int>& sa, int right, int length, const string& s) {
    for (int i = right - 1; i >= 0; i--) {
        if (SuffixArrayAlgo::commonPrefixNaive(s, sa[i], sa[right]) <= length)
            return i;
    }

    return -1;
}

void testSuffixArray() {
    //return; //TODO: if you want to test, make this line a comment.

    cout << "-- Suffix Array --------------------" << endl;
    {
        string s("abdadafaaabdfaeef");

        vector<int> ans = SuffixArray::buildSuffixArray(s);
        vector<int> gt = makeSuffixArrayNaive(s, int(s.length()));
        assert(ans == gt);

        long long cntAns = SuffixArrayAlgo::countSubstrings(ans, s);
        long long cntGT = SuffixArrayAlgo::countSubstringsNaive(gt, s);
        assert(cntAns == cntGT);
    }
    {
        //        01234567890123456
        string s("abdadafaaabdfaeef");

#if 0
        vector<int> ans = SuffixArrayLarssonSadakane::build(s);
#else
        SuffixArrayLarssonSadakane larsson;
        vector<int> ans = larsson.build(s);
#endif
        vector<int> gt = makeSuffixArrayNaive(s, int(s.length()));
        if (ans != gt) {
            cout << "Mismatched : " << endl
                 << "    ans = " << ans << endl
                 << "     gt = " << gt << endl;
        }
        assert(ans == gt);

        long long cntAns = SuffixArrayAlgo::countSubstrings(ans, s);
        long long cntGT = SuffixArrayAlgo::countSubstringsNaive(gt, s);
        assert(cntAns == cntGT);
    }
    {
        int T = 10;
        while (T-- > 0) {
            int N = 1000;
            auto s = makeRandomString(N);

            vector<pair<string, int>> v;
            v.reserve(N);
            for (int i = 0; i < N; i++)
                v.emplace_back(s.substr(i, N - i), i);

            sort(v.begin(), v.end());

            auto SA = SuffixArray::buildSuffixArray(s);
#if 0
            auto SA2 = SuffixArrayLarssonSadakane::build(s);
#else
            SuffixArrayLarssonSadakane larsson;
            vector<int> SA2 = larsson.build(s);
#endif
            for (int i = 0; i < N; i++) {
                if (v[i].second != SA[i] || v[i].second != SA2[i])
                    cout << "Mismatched  at " << __LINE__ << endl;
                assert(v[i].second == SA[i] && v[i].second == SA2[i]);
            }
        }
    }
    {
        int N = 100;
        int T = 10;
        int L = 5;
        string s = makeRandomString(N, 3);

        SuffixArray SA(s);

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < T; j++) {
                int len = RandInt32::get() % L;
                assert(SA.lowerBoundLcpForward(i, len) == lowerBoundForwardSlow(SA.suffixArray, i, len, s));
                assert(SA.upperBoundLcpForward(i, len) == lowerBoundForwardSlow(SA.suffixArray, i, len - 1, s));
                assert(SA.lowerBoundLcpBackward(i, len) == lowerBoundBackwardSlow(SA.suffixArray, i, len, s));
                assert(SA.upperBoundLcpBackward(i, len) == lowerBoundBackwardSlow(SA.suffixArray, i, len - 1, s));
            }
        }
    }
    // speed test
    {
        int T = 10000;
        int N = 10000;
#ifdef _DEBUG
        T = 100;
        N = 100;
#endif

        vector<string> in;
        for (int i = 0; i < T; i++)
            in.push_back(makeRandomString(N));

        int sum1 = 0;
        int sum2 = 0;

        PROFILE_START(0);
        for (int i = 0; i < T; i++) {
            auto SA = SuffixArray::buildSuffixArray(in[i]);
            sum1 += SA[0];
        }
        PROFILE_STOP(0);

        PROFILE_START(1);
        SuffixArrayLarssonSadakane larsson;
        for (int i = 0; i < T; i++) {
#if 0
            auto SA = SuffixArrayLarssonSadakane::build(in[i]);
#else
            vector<int> SA = larsson.build(in[i]);
#endif
            sum2 += SA[0];
        }
        PROFILE_STOP(1);

        if (sum1 != sum2)
            cout << "Mismatched  at " << __LINE__ << endl;
    }
    {
        int T = 1000;
        int N = 100000;
#ifdef _DEBUG
        T = 100;
        N = 100;
#endif

        vector<string> in;
        for (int i = 0; i < T; i++)
            in.push_back(makeRandomString(N));

        int sum1 = 0;
        int sum2 = 0;

        PROFILE_START(0);
        for (int i = 0; i < T; i++) {
            auto SA = SuffixArray::buildSuffixArray(in[i]);
            sum1 += SA[0];
        }
        PROFILE_STOP(0);

        PROFILE_START(1);
        SuffixArrayLarssonSadakane larsson;
        for (int i = 0; i < T; i++) {
#if 0
            auto SA = SuffixArrayLarssonSadakane::build(in[i]);
#else
            vector<int> SA = larsson.build(in[i]);
#endif
            sum2 += SA[0];
        }
        PROFILE_STOP(1);

        if (sum1 != sum2)
            cout << "Mismatched  at " << __LINE__ << endl;
    }
    {
        int T = 100;
        int N = 1000000;
#ifdef _DEBUG
        T = 100;
        N = 100;
#endif

        vector<string> in;
        for (int i = 0; i < T; i++)
            in.push_back(makeRandomString(N));

        int sum1 = 0;
        int sum2 = 0;

        PROFILE_START(0);
        for (int i = 0; i < T; i++) {
            auto SA = SuffixArray::buildSuffixArray(in[i]);
            sum1 += SA[0];
        }
        PROFILE_STOP(0);

        PROFILE_START(1);
        SuffixArrayLarssonSadakane larsson;
        for (int i = 0; i < T; i++) {
#if 0
            auto SA = SuffixArrayLarssonSadakane::build(in[i]);
#else
            vector<int> SA = larsson.build(in[i]);
#endif
            sum2 += SA[0];
        }
        PROFILE_STOP(1);

        if (sum1 != sum2)
            cout << "Mismatched  at " << __LINE__ << endl;
    }
    cout << "OK!" << endl;
}
