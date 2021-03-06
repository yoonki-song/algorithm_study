#include <memory.h>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cctype>
#include <cstring>
#include <climits>
#include <cmath>
#include <vector>
#include <string>
#include <memory>
#include <numeric>
#include <limits>
#include <functional>
#include <tuple>
#include <set>
#include <map>
#include <bitset>
#include <stack>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iostream>

using namespace std;

#define PROFILE_START(i)    clock_t start##i = clock()
#define PROFILE_STOP(i)     fprintf(stderr, "elapsed time (" #i ") = %f\n", double(clock() - start##i) / CLOCKS_PER_SEC)

typedef long long           ll;
typedef unsigned long long  ull;

#define MAXN    100000
#define MAXM    14

int gN, gM;
string gS;
vector<string> gP;

struct PatternSubsequence {
    static int popcount(unsigned x) {
#ifndef __GNUC__
        return int(__popcnt(x));
#else
        return __builtin_popcount(x);
#endif
    }

    int N;                      // string length
    int M;                      // pattern count
    vector<int> patternLength;  // pattern lengths
    vector<vector<int>> next;   // next[pattern id][string position]

    void build(const string& S, const vector<string>& patterns) {
        N = int(S.length());
        M = int(patterns.size());

        patternLength = vector<int>(M);
        next = vector<vector<int>>(M, vector<int>(N + 1, N));

        for (int i = 0; i < M; i++) {
            auto pos = kmp(S, patterns[i]);
            for (auto j : pos)
                next[i][j] = j;
        }

        for (int i = 0; i < M; i++) {
            patternLength[i] = int(patterns[i].length());
            for (int j = N - 1; j >= 0; j--)
                next[i][j] = min(next[i][j], next[i][j + 1]);
        }
    }

    // if it has a pattern subsequence, then return next position
    // else return -1
    int hasPatternSubsequence(const vector<int>& patternIndexes) {
        int pos = 0;
        for (auto id : patternIndexes) {
            pos = next[id][pos];
            if (pos >= N)
                return -1;
            pos += int(patternLength[id]);
        }
        return pos;
    }

    static vector<int> getBitIndexes(int x) {
        vector<int> res;
        for (int i = 0; x; i++, x >>= 1) {
            if (x & 1)
                res.push_back(i);
        }
        return res;
    }

    //---

    static vector<int> prefixFunction(const string& s) {
        vector<int> p(s.size());
        int j = 0;
        for (int i = 1; i < int(s.size()); i++) {
            while (j > 0 && s[j] != s[i])
                j = p[j - 1];

            if (s[j] == s[i])
                j++;
            p[i] = j;
        }
        return p;
    }

    static vector<int> kmp(const string& s, const string& p) {
        vector<int> res;
        vector<int> pref = prefixFunction(p);

        int n = int(s.size()), m = int(p.size()), j = 0;
        for (int i = 0; i < n; i++) {
            while (j > 0 && s[i] != p[j])
                j = pref[j - 1];

            if (s[i] == p[j]) {
                if (j == m - 1) {
                    res.push_back(i - m + 1);
                    j = pref[j];
                } else {
                    j++;
                }
            }
        }

        return res;
    }
};

// O(N*M + SUM(P) C(M, M/2) * (N + (M / 2)! * M))
inline long long countPatternSubsequenceInString(string& S, vector<string>& patterns) {
    PatternSubsequence forward, backward;

    forward.build(S, patterns);

    for (auto& p : patterns)
        reverse(p.begin(), p.end());
    reverse(S.begin(), S.end());
    backward.build(S, patterns);

    int N = int(S.length());
    int M = int(patterns.size());
    int halfM = M / 2;

    ll ans = 0;
    for (int i = 0; i < (1 << M); i++) {
        if (PatternSubsequence::popcount(static_cast<unsigned int>(i)) != halfM)
            continue;

        vector<int> count(N + 1);

        // forward
        auto idx = PatternSubsequence::getBitIndexes(i);
        do {
            int i = forward.hasPatternSubsequence(idx);
            if (i >= 0)
                ++count[i];
        } while (next_permutation(idx.begin(), idx.end()));

        for (int i = 1; i <= N; i++)
            count[i] += count[i - 1];

        // backward
        idx = PatternSubsequence::getBitIndexes(i ^ ((1 << M) - 1));
        do {
            int i = backward.hasPatternSubsequence(idx);
            if (i >= 0)
                ans += count[N - i];
        } while (next_permutation(idx.begin(), idx.end()));
    }

    return ans;
}

int main(void) {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    cin >> gN >> gM;
    cin >> gS;

    gP.resize(gM);
    for (int i = 0; i < gM; i++)
        cin >> gP[i];

    cout << countPatternSubsequenceInString(gS, gP) << endl;

    return 0;
}
