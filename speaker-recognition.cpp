#include <GRT/GRT.h>
#include "mfcc.h"
#include "wav-reader.h"

int main(int argc, char* argv[]) {
    WavReader reader(argv[1]);

    // Sample rate is 16k, FFT window size 512 => 32 ms, hopsize 128 => 8 ms
    GRT::FFT fft(512, 128, 1,  GRT::FFT::HAMMING_WINDOW, true, false);
    MFCC mfcc(16000, 512 / 2, 300, 8000, 26, 12, 22);
    GRT::GMM gmm(32);

    for (const auto& d : reader.getData()) {
        fft.update(d);
    }
    fft.getFeatureVector();
}
