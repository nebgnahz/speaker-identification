#include "wav-reader.h"

#include <assert.h>
#include <sndfile.h>

#include <iostream>

WavReader::WavReader(const std::string name) {
    SF_INFO sfinfo;
    SNDFILE* file = sf_open(name.c_str(), SFM_READ, &sfinfo) ;

    frames_ = sfinfo.frames;
    sample_rate_ = sfinfo.samplerate;
    num_channels_ = sfinfo.channels;

    double* ptr = new double[sfinfo.frames * sfinfo.channels];
    sf_count_t num = sf_readf_double(file, ptr, sfinfo.frames);
    assert(num == frames_);

    // TODO(benzh) For now we only support one-channel audio data.
    assert(num_channels_ == 1);

    data_.clear();
    data_.insert(data_.end(), &ptr[0], &ptr[num - 1]);

    delete[] ptr;
    sf_close(file);
}
