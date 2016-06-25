#ifndef MFCC_H_
#define MFCC_H_

#include <stdint.h>
#include <math.h>

#include <vector>

using std::vector;
#define PI 3.1415926535

class TriFilterBank {
  public:
    TriFilterBank(double left, double middle, double right, uint32_t fs, uint32_t size);

    static inline double toMelScale(double freq) {
        return 1127.0f * log(1.0f + freq / 700.0f);
    }

    static inline double fromMelScale(double mel_freq) {
        return 700.0f * (exp(mel_freq / 1127.0f) - 1.0f);
    }

    inline vector<double>& getFilter() { return filter_;  }

    double filter(vector<double> input);

  private:
    vector<double> filter_;
};

class MFCC {
  public:
    MFCC(uint32_t sampleRate, uint32_t FFTSize,
         double startFreq, double endFreq,
         uint32_t numFilterbankChannel,
         uint32_t numCepstralCoeff,
         uint32_t lifterParam);

    inline vector<TriFilterBank> getFilters() const { return filters_; }

    vector<double> getLFBE(const vector<double>& fft);
    vector<double> getCC(const vector<double>& lfbe);
    vector<double> lifterCC(const vector<double>& cc);

  protected:
    bool initialized_;
    uint32_t num_cc_;
    uint32_t lifter_param_;

    vector<TriFilterBank> filters_;
};

#endif  // MFCC_H_
