using namespace std;

#include "fft.h"

#define SCALE   32768

vector<int> multPolyMod(const vector<int>& left, const vector<int>& right, int MOD) {
    vector<int> res(left.size() + right.size() - 1);

    for (int i = 0; i < (int)right.size(); i++) {
        for (int j = 0; j < (int)left.size(); j++) {
            res[i + j] = (long long(res[i + j]) + long long(left[j]) * right[i]) % MOD;
        }
    }

    return res;
}

// It's better performance than multPoly() when N >= 128
vector<int> multPolyFFTMod(const vector<int>& left, const vector<int>& right, int MOD) {
    int sizeL = (int)left.size();
    int sizeR = (int)right.size();
    int sizeDst = sizeL + sizeR - 1;

    int size = 1;
    while (size < sizeDst)
        size <<= 1;

    vector<Complex<double>> A1(size), A2(size);
    vector<Complex<double>> B1(size), B2(size);
    for (int i = 0; i < sizeL; i++) {
        A1[i].first = left[i] / SCALE;
        A2[i].first = left[i] % SCALE;
    }
    for (int i = 0; i < sizeR; i++) {
        B1[i].first = right[i] / SCALE;
        B2[i].first = right[i] % SCALE;
    }

    fft(A1); fft(A2);
    fft(B1); fft(B2);

    vector<int> res(sizeDst);
    vector<Complex<double>> C(size);

    for (int i = 0; i < size; i++)
        C[i] = A1[i] * B1[i];
    fft(C, true);
    for (int i = 0; i < sizeDst; i++)
        res[i] = ((long long)fmod(C[i].first + 0.5, MOD) * SCALE * SCALE) % MOD;

    for (int i = 0; i < size; i++)
        C[i] = A1[i] * B2[i] + A2[i] * B1[i];
    fft(C, true);
    for (int i = 0; i < sizeDst; i++)
        res[i] = (res[i] + (long long)fmod(C[i].first + 0.5, MOD) * SCALE) % MOD;

    for (int i = 0; i < size; i++)
        C[i] = A2[i] * B2[i];
    fft(C, true);
    for (int i = 0; i < sizeDst; i++)
        res[i] = (res[i] + (long long)fmod(C[i].first + 0.5, MOD)) % MOD;

    return res;
}


// https://www.hackerrank.com/contests/w23/challenges/sasha-and-swaps-ii
// (x + a)(x + a + 1)(x + a + 2)...(x + k)
// left = a, right = k
vector<int> multPolyPolyMod(int left, int right, int MOD) {
    int n = right - left + 1;
    if (n < 128) {
        vector<int> res = vector<int>{ 1, left };
        for (int i = left + 1; i <= right; i++)
            res = multPolyMod(res, vector<int>{ 1, i }, MOD);
        return res;
    }

    vector<vector<int>> poly;

    poly.push_back(vector<int>{ 1, left });
    for (int i = left + 1; i <= right; i++) {
        int j = 0;
        while (j < (int)poly.size() && !(poly[j].size() & (poly[j].size() - 1)))
            j++;

        // make the number of polynomial terms 2^n
        if (j >= (int)poly.size())
            poly.push_back(vector<int>{ 1, i });
        else
            poly[j] = multPolyMod(poly[j], vector<int> { 1, i }, MOD);

        // apply FFT
        while (j > 0 && poly[j].size() == poly[j - 1].size()
            && (poly[j].size() & (poly[j].size() - 1)) == 0) {
            if (poly[j].size() < 128)
                poly[j - 1] = multPolyMod(poly[j - 1], poly[j], MOD);
            else
                poly[j - 1] = multPolyFFTMod(poly[j - 1], poly[j], MOD);
            poly.erase(poly.begin() + j);
            j--;
        }
    }

    vector<int> res = poly.back();
    for (int i = (int)poly.size() - 2; i >= 0; i--)
        res = multPolyFFTMod(res, poly[i], MOD);

    return res;
}


/////////// For Testing ///////////////////////////////////////////////////////

#include <time.h>
#include <cassert>
#include <string>
#include <iostream>
#include "../common/iostreamhelper.h"
#include "../common/profile.h"

#define MOD     1000000007

void testMultPolyMod() {
    return; //TODO: if you want to test functions of this file, make this line a comment.

    cout << "--- Modular Polynomial Multiplication ----------------" << endl;

    for (int i = 0; i < 10; i++) {
        vector<int> A(1000);
        vector<int> B(1000);

        for (int i = 0; i < (int)A.size(); i++)
            A[i] = (long long)rand() * rand() % MOD;
        for (int i = 0; i < (int)B.size(); i++)
            B[i] = (long long)rand() * rand() % MOD;

        vector<int> out1 = multPolyMod(A, B, MOD);
        vector<int> out2 = multPolyFFTMod(A, B, MOD);
        assert(out1 == out2);
    }

    cout << "*** Speed test ***" << endl;
    for (int n = 32; n <= 2048; n <<= 1) {
        vector<int> in1(n);
        vector<int> in2(n);
        vector<int> out;
        for (int i = 0; i < n; i++) {
            in1[i] = rand() % 1024;
            in2[i] = rand() % 1024;
        }

        cout << "N = " << n << endl;
        cout << "  multPolyMod() : ";
        PROFILE_START(0);
        for (int i = 0; i < 1000; i++) {
            out = multPolyMod(in1, in2, MOD);
            if (out.empty())
                cerr << "It'll never be shwon!" << endl;
        }
        PROFILE_STOP(0);

        cout << "  multPolyFFTMod() : ";
        PROFILE_START(1);
        for (int i = 0; i < 1000; i++) {
            out = multPolyFFTMod(in1, in2, MOD);
            if (out.empty())
                cerr << "It'll never be shwon!" << endl;
        }
        PROFILE_STOP(1);
    }

    cout << "OK!" << endl;
}