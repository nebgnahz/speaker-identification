clear; close all; clc;
addpath('mfcc');

Tw = 25;  % ms
Ts = 10;  % ms
alpha = 0.97;
M = 20;
C = 12; 
L = 22;
LF = 300;
HF = 3700;

NC = 16; % num classes
N = 8;

MFCCs1 = [];
for i = 1:N
    wav_file = ['../data/fsew0_v1.1/fsew0_' ...
                num2str(i, '%.3d'), '.wav'];  

    [ speech, fs ] = audioread( wav_file );

    % Feature extraction (feature vectors as columns)
    [ MFCC, ~, ~ ] = ...
        mfcc( speech, fs, Tw, Ts, alpha, @hamming, [LF HF], M, C+1, L );
    
    MFCCs1 = [ MFCCs1; MFCC'];
end

MFCCs2 = [];
for i = 30:(30+N)
    wav_file = ['../data/msak0_v1.1/msak0_' ...
                num2str(i, '%.3d'), '.wav'];  

    [ speech, fs ] = audioread( wav_file );

    % Feature extraction (feature vectors as columns)
    [ MFCC, ~, ~ ] = ...
        mfcc( speech, fs, Tw, Ts, alpha, @hamming, [LF HF], M, C+1, L );
    
    MFCCs2 = [ MFCCs2; MFCC'];
end

tic
options = statset('MaxIter', 500);
GMModel1 = fitgmdist(MFCCs1, NC, 'Options', options);
GMModel2 = fitgmdist(MFCCs2, NC, 'RegularizationValue', 0.1, ...
                     'Options', options);

toc
wav_file = ['../data/msak0_v1.1/msak0_112.wav'];
[ speech, fs ] = audioread( wav_file );
% Feature extraction (feature vectors as columns)
[ MFCC, ~, ~ ] = ...
    mfcc( speech, fs, Tw, Ts, alpha, @hamming, [LF HF], M, C+1, L );
MFCCt= MFCC';

p = [pdf(GMModel1, MFCCt), pdf(GMModel2, MFCCt)];  % P(x|model_i)
[~, cIdx] = max(p,[],2);                           % argmax_i P(x|model_i)

figure
plot(cIdx)
axis([0, size(cIdx, 1), 0, 3])
figure
plot(speech)