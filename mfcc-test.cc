#include <assert.h>

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#include "mfcc.h"
#include "GRT.h"

const uint32_t kSampleRate = 16000;
const uint32_t kFFTSize = 257;
const uint32_t kFrameCount = 264;
const uint32_t kNumFilterBank = 20;
const uint32_t kNumCC = 13;
const uint32_t kLifterParam = 22;
const uint32_t kStartFreq = 300;
const uint32_t kEndFreq = 3700;

#define EXPECT_CLOSE_VEC(a, b, eps)                         \
    for (auto ai = (a).begin(), bi = (b).begin();           \
         ai != (a).end() && bi != (b).end(); ++ai, ++bi) {  \
        if (abs(*ai - *bi) > eps) {                         \
            std::cerr << "diff(" << *ai << ", " << *bi      \
                      << ") > " << eps << std::endl;        \
            assert(false);                                  \
        }                                                   \
    }

#define EXPECT_EQ(a, b)                                 \
    if ((a) != (b)) {                                   \
        std::cerr << (a) << " != " << (b) << std::endl; \
        assert(false);                                  \
    }

using std::vector;
using std::string;

// Forward declaration of util functions
vector<string> split(string str, char delimiter);
GRT::MatrixDouble readCSVToMatrix(const string&, uint32_t, uint32_t);

int main(int argc, const char* argv[]) {
    if (argc < 2) {
        std::cerr << "USAGE: " << std::endl;
        std::cerr << "  " << argv[0] << " <data directory> <iteration>"
                  << std::endl;
        exit(1);
    }

    std::string dir = argv[1];

    uint32_t iters = 1;

    if (argc == 3) {
        if (sscanf(argv[2], "%ud", &iters) != 1 || iters == 0) {
            std::cerr<< "iteration is not a positive integer" << std::endl;
            exit(1);
        }
    }

    GRT::MatrixDouble fft = readCSVToMatrix(dir + "/2.fft.csv", kFFTSize, kFrameCount);
    GRT::MatrixDouble fbe = readCSVToMatrix(dir + "/3.lfbe.csv", kNumFilterBank, kFrameCount);
    GRT::MatrixDouble cc = readCSVToMatrix(dir + "/4.cc.csv", kNumCC, kFrameCount);
    GRT::MatrixDouble liftered = readCSVToMatrix(dir + "/5.liftered.csv", kNumCC, kFrameCount);

    for (uint32_t iter = 0; iter < iters; iter++) {

    GRT::MFCC::Options options;
    options.sample_rate = kSampleRate;
    options.fft_size = kFFTSize;
    options.start_freq = kStartFreq;
    options.end_freq = kEndFreq;
    options.num_tri_filter = kNumFilterBank;
    options.num_cepstral_coeff = kNumCC;
    options.lifter_param = kLifterParam;
    GRT::MFCC mfcc(options);

    // Iterate each frame and compare the result
    for (uint32_t col = 0; col < kFrameCount; col++) {
        // 1. FFT -> LFBE
        vector<double> fft_frame = fft.getColVector(col);
        vector<double> my_lfbe(kNumFilterBank);
        mfcc.computeLFBE(fft_frame, my_lfbe);
        vector<double> their_lfbe = fbe.getColVector(col);
        EXPECT_CLOSE_VEC(my_lfbe, their_lfbe, 0.01);

        // 2. LFBE -> CC
        vector<double> my_cc(kNumCC);
        mfcc.computeCC(their_lfbe, my_cc);
        vector<double> their_cc = cc.getColVector(col);
        EXPECT_CLOSE_VEC(my_cc, their_cc, 0.01);

        // 3. CC -> Lifter
        vector<double> my_liftered = mfcc.lifterCC(their_cc);
        vector<double> their_liftered = liftered.getColVector(col);
        EXPECT_CLOSE_VEC(my_liftered, their_liftered, 0.01);

        // Finally, do an end-to-end check.
        mfcc.computeFeatures(fft_frame);
        vector<double> features = mfcc.getFeatureVector();
        EXPECT_CLOSE_VEC(features, their_liftered, 0.01);
    }
    }

    // If we reach here, declare success
    std::cout << "Passed" << std::endl;
    return 0;
}

GRT::MatrixDouble readCSVToMatrix(const string& filename, uint32_t row, uint32_t col) {
    GRT::MatrixDouble mat(row, col);
    std::ifstream file(filename);
    uint32_t r = 0, c = 0;
    string line;
    while (std::getline(file, line)) {
        c = 0;
        // parse csv file
        vector<string> elems = split(line, ',');
        for (const string& elem : elems) {
            mat[r][c++] = std::stod(elem);
        }
        EXPECT_EQ(c, col);
        r++;
    }

    EXPECT_EQ(r, row);
    return mat;
}

vector<string> split(string str, char delimiter) {
    vector<string> internal;
    stringstream ss(str);
    string item;

    while(getline(ss, item, delimiter)) {
        internal.push_back(item);
    }

    return internal;
}
