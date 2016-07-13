#include "mfcc.h"

#include <cmath>
#include <new>

#include <Accelerate/Accelerate.h>

namespace GRT {

RegisterFeatureExtractionModule<MFCC> MFCC::registerModule("MFCC");

TriFilterBanks::TriFilterBanks() :
        initialized_(false) {
}

void TriFilterBanks::initialize(uint32_t num_filter, uint32_t filter_size) {
    num_filter_ = num_filter;
    filter_size_ = filter_size;
    filter_ = new double[num_filter_ * filter_size_];
    initialized_ = true;
}

void TriFilterBanks::setFilter(
    uint32_t idx, double left, double middle, double right, uint32_t fs) {
    uint32_t size = filter_size_;
    double unit = 1.0f * fs / 2 / (size - 1);
    for (uint32_t i = 0; i < size; i++) {
        double f = unit * i;
        uint32_t ni = i + idx * filter_size_;
        if (f <= left) {
            filter_[ni] = 0;
        } else if (left < f && f <= middle) {
            filter_[ni] = 1.0f * (f - left) / (middle - left);
        } else if (middle < f && f <= right) {
            filter_[ni] = 1.0f * (right - f) / (right - middle);
        } else if (right < f) {
            filter_[ni] = 0;
        } else {
            assert(false && "TriFilterBanks argument wrong or implementation bug");
        }
    }
}

TriFilterBanks::~TriFilterBanks() {
    if (initialized_) { delete filter_; }
}

void TriFilterBanks::filter(const vector<double>& input, vector<double>& output) {
    assert(input.size() == filter_size_
           && "Dimension mismatch in TriFilterBanks filter");

    // Perform matrix multiplication
    cblas_dgemv(CblasRowMajor, CblasNoTrans,
                num_filter_, filter_size_,
                1.0, filter_, filter_size_, input.data(), 1,
                1.0, output.data(), 1);
}


MFCC::MFCC(uint32_t sampleRate, uint32_t FFTSize,
           double startFreq, double endFreq,
           uint32_t numFilterbankChannel,
           uint32_t numCepstralCoeff,
           uint32_t lifterParam)
        : initialized_(false),
          num_tri_filter_(numFilterbankChannel),
          num_cc_(numCepstralCoeff),
          lifter_param_(lifterParam) {
    classType = "MFCC";
    featureExtractionType = classType;
    debugLog.setProceedingText("[INFO MFCC]");
    debugLog.setProceedingText("[DEBUG MFCC]");
    errorLog.setProceedingText("[ERROR MFCC]");
    warningLog.setProceedingText("[WARNING MFCC]");

    if (sampleRate <= 0 || FFTSize <= 0 || startFreq <= 0 || endFreq <= 0 ||
        numFilterbankChannel <= 0 || numCepstralCoeff <= 0 || lifterParam <= 0) {
        return;
    }

    numInputDimensions = FFTSize;
    numOutputDimensions = numCepstralCoeff;

    filters_.initialize(numFilterbankChannel, FFTSize);

    vector<double> freqs(numFilterbankChannel + 2);
    double mel_start = TriFilterBanks::toMelScale(startFreq);
    double mel_end = TriFilterBanks::toMelScale(endFreq);
    double mel_step = (mel_end - mel_start) / (numFilterbankChannel + 1);

    for (uint32_t i = 0; i < numFilterbankChannel + 2; i++) {
        freqs[i] = TriFilterBanks::fromMelScale(mel_start + i * mel_step);
    }

    for (uint32_t i = 0; i < numFilterbankChannel; i++) {
        filters_.setFilter(i, freqs[i], freqs[i + 1], freqs[i + 2], sampleRate);
    }

    initialized_ = true;
}

MFCC::MFCC(const MFCC &rhs) {
    classType = rhs.getClassType();
    featureExtractionType = classType;
    debugLog.setProceedingText("[DEBUG MFCC]");
    errorLog.setProceedingText("[ERROR MFCC]");
    warningLog.setProceedingText("[WARNING MFCC]");

    this->num_cc_ = rhs.num_cc_;
    this->lifter_param_ = rhs.lifter_param_;
    *this = rhs;
}

MFCC& MFCC::operator=(const MFCC &rhs) {
    if (this != &rhs) {
        this->classType = rhs.getClassType();
        this->filters_ = rhs.getFilters();
        this->num_cc_ = rhs.num_cc_;
        this->lifter_param_ = rhs.lifter_param_;
        copyBaseVariables( (FeatureExtraction*)&rhs );
    }
    return *this;
}

bool MFCC::deepCopyFrom(const FeatureExtraction *featureExtraction) {
    if (featureExtraction == NULL) return false;
    if (this->getFeatureExtractionType() ==
        featureExtraction->getFeatureExtractionType() ){
        // Invoke the equals operator to copy the data from the rhs instance to
        // this instance
        *this = *(MFCC*)featureExtraction;
        return true;
    }

    errorLog << "clone(MFCC *featureExtraction)"
             << "-  FeatureExtraction Types Do Not Match!"
             << endl;
    return false;
}

void MFCC::computeLFBE(const vector<double>& fft, vector<double>& lfbe) {
    assert(lfbe.size() == num_tri_filter_
           && "Dimension mismatch for LFBE computation");

    uint32_t M = num_tri_filter_;
    filters_.filter(fft, lfbe);

    for (uint32_t i = 0; i < M; i++) {
        if (lfbe[i] != 0) {
            lfbe[i] = log(lfbe[i]);
        }
    }
}

vector<double> MFCC::getCC(const vector<double>& lfbe) {
    uint32_t M = num_tri_filter_;

    vector<double> cc(num_cc_);
    for (uint32_t i = 0; i < num_cc_; i++) {
        for (uint32_t j = 0; j < M; j++) {
            // [1] j is 1:M not 0:(M-1), so we change (j - 0.5) to (j + 0.5)
            cc[i] += sqrt(2.0 / M) * lfbe[j] * cos(PI * i / M * (j + 0.5));
        }
    }
    return cc;
}

vector<double> MFCC::lifterCC(const vector<double>& cc) {
    vector<double> liftered(num_cc_);
    uint32_t L = lifter_param_;
    for (uint32_t i = 0; i < num_cc_; i++) {
        liftered[i] = (1 + 1.0f * L / 2 * sin(PI * i / L)) * cc[i];
    }
    return liftered;
}

bool MFCC::computeFeatures(const VectorDouble &inputVector) {
    // We assume the input is from a DFT (FFT) transformation.
    vector<double> intermediate;
    intermediate.reserve(num_tri_filter_);
    computeLFBE(inputVector, intermediate);

    VectorDouble cc = getCC(intermediate);
    VectorDouble liftered = lifterCC(cc);
    featureVector = liftered;

    featureDataReady = true;
    return true;
}

bool MFCC::reset() {
    return true;
}

}  // namespace GRT
