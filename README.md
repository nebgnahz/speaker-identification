# Speaker Recognition

Speaker recognition is the task of recognizing a person automatically from his
or her voice; we assume there is a set of known voices recorded prior to the
recognition task. In addtion, we don't address the verification problem here.

For detailed discussion about the subject, please refer to the paper
[Automatic Speaker Recognition Using Gaussian Mixture Speaker Models](http://dl.acm.org/citation.cfm?id=211317).

The main technique we are using are MFCC and GMM.

## MFCC

Mel-frequency cepstrum coefficients are common features that are used for
voice/speech related tasks. It's a representation of the power spectrum, but
based on a nonlinear mel scale.

There is an educational MFCC implementation (not focusing on , please check out
the
[mfcc branch](https://github.com/nebgnahz/speaker-identification/tree/mfcc). The
master branch uses BLAS to speed up feature extraction.

## GMM

For text-independent speaker recognition, the sound source (human voice) can be
modelled as a mixture models, each following the Gaussian
distribution. Typically, 32-component GMM should be sufficent to distinguish a
few number of speakers.

# Code Structure

This repository mainly contains the C++ implementation. However, during the
research, it was easier to use Matlab to do data and algorithm exploration. You
can check out the `matlab` folder and specifically the script
`speaker_recognition.m`. It currently uses data that's exported from GRT. It wouldn't
be too hard to simply use audio files (you may check out the MOCHA-TIMIT dataset).

# Dataset

1. [TIMIT](https://catalog.ldc.upenn.edu/LDC93S1)

Doesn't seem to be free.

2. [MOCHA-TIMIT](http://www.cstr.ed.ac.uk/research/projects/artic/mocha.html)

Although small, two speakers but can be used for initial testing.
