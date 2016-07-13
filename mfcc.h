#ifndef ESP_MFCC_H_
#define ESP_MFCC_H_

#include "CoreModules/FeatureExtraction.h"

#include <stdint.h>
#include <math.h>
#include <vector>

namespace GRT {

// TriFilterBanks contains the matrix that would perform the filter operation.
// Specifically, the multiplication will take the following form:
//
//   [  filter bank 1  ]     |----|
//   [  filter bank 2  ]
//   [   ...........   ]      fft
//   [   ...........   ]
//   [  filter bank N  ]     |____|
class TriFilterBanks {
  public:
    TriFilterBanks();
    ~TriFilterBanks();

    void initialize(uint32_t num_filter, uint32_t filter_size);
    void setFilter(uint32_t i, double left, double middle, double right, uint32_t fs);

    static inline double toMelScale(double freq) {
        return 1127.0f * log(1.0f + freq / 700.0f);
    }

    static inline double fromMelScale(double mel_freq) {
        return 700.0f * (exp(mel_freq / 1127.0f) - 1.0f);
    }

    inline uint32_t getNumFilters() const {
        return num_filter_;
    }

    void filter(const vector<double>& input, vector<double>& output);

  private:
    bool initialized_;
    double* filter_;
    uint32_t num_filter_;
    uint32_t filter_size_;
};

class MFCC : public FeatureExtraction {
  public:
    MFCC(uint32_t sampleRate = -1, uint32_t FFTSize = -1,
         double startFreq = -1, double endFreq = -1,
         uint32_t numFilterbankChannel = -1,
         uint32_t numCepstralCoeff = -1,
         uint32_t lifterParam = -1);

    MFCC(const MFCC &rhs);
    MFCC& operator=(const MFCC &rhs);
    bool deepCopyFrom(const FeatureExtraction *featureExtraction) override;
    ~MFCC() {}

    virtual bool computeFeatures(const VectorDouble &inputVector) override;
    virtual bool reset() override;

    using MLBase::train;
    using MLBase::train_;
    using MLBase::predict;
    using MLBase::predict_;

    TriFilterBanks getFilters() const {
        return filters_;
    }

  public:
    void computeLFBE(const vector<double>& fft, vector<double>& lfbe);
    vector<double> getCC(const vector<double>& lfbe);
    vector<double> lifterCC(const vector<double>& cc);

  protected:
    bool initialized_;
    uint32_t num_tri_filter_;
    uint32_t num_cc_;
    uint32_t lifter_param_;

    TriFilterBanks filters_;

    static RegisterFeatureExtractionModule<MFCC> registerModule;
};

}  // namespace GRT

#endif  // ESP_MFCC_H_
