#ifndef WAV_READER_
#define WAV_READER_

#include <string>
#include <vector>
#include <stdint.h>

using std::vector;

class WavReader {
  public:
    WavReader(const std::string name);

    const vector<double>& getData() const {
        return data_;
    }

  private:
    uint32_t frames_;
    uint32_t sample_rate_;
    uint32_t num_channels_;

    vector<double> data_;
};

#endif  // WAV_READER_
