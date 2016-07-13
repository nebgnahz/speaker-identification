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
    struct Options {
        uint32_t sample_rate;
        uint32_t fft_size;
        double start_freq;
        double end_freq;
        uint32_t num_tri_filter;
        uint32_t num_cepstral_coeff;
        uint32_t lifter_param;
        Options() : sample_rate(0), fft_size(0), start_freq(-1), end_freq(-1),
                    num_tri_filter(0), num_cepstral_coeff(0), lifter_param(0) {}

        bool operator==(const Options& rhs) {
            return this->sample_rate == rhs.sample_rate &&
                    this->fft_size == rhs.fft_size &&
                    this->start_freq == rhs.start_freq &&
                    this->end_freq == rhs.end_freq &&
                    this->num_tri_filter == rhs.num_tri_filter &&
                    this->num_cepstral_coeff == rhs.num_cepstral_coeff &&
                    this->lifter_param == rhs.lifter_param;
        }
    };

    MFCC(struct Options options = Options::Options());

    MFCC(const MFCC &rhs);
    MFCC& operator=(const MFCC &rhs);
    bool deepCopyFrom(const FeatureExtraction *featureExtraction) override;
    ~MFCC() {}

    void initialize();

    virtual bool computeFeatures(const VectorDouble &inputVector) override;
    virtual bool reset() override;

    using MLBase::train;
    using MLBase::train_;
    using MLBase::predict;
    using MLBase::predict_;

    struct Options getOptions() const {
        return options_;
    }
    TriFilterBanks getFilters() const {
        return filters_;
    }

  public:
    void computeLFBE(const vector<double>& fft, vector<double>& lfbe);
    vector<double> getCC(const vector<double>& lfbe);
    vector<double> lifterCC(const vector<double>& cc);

  protected:
    bool initialized_;

    Options options_;

    // The information below can be generated with options_. We fill them during
    // the initialize() function.
    double* cc_matrix_;

    TriFilterBanks filters_;

    static RegisterFeatureExtractionModule<MFCC> registerModule;
};

}  // namespace GRT

#endif  // ESP_MFCC_H_
