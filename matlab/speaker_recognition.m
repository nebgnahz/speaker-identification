% First standard Matlab script setup.
% Since we will be using mfcc functions, we add it to the path.
clearvars; close all; clc;
addpath('mfcc');

% First we load all the data. The data is stored as csv file, and the 
% format is `sample_index, class_label, data`. This is from the GRT's
% `saveDatasetToCSVFile` function:
%    GRT/DataStructures/TimeSeriesClassificationData.cpp#L556
data = csvread('TrainingData.csv');

% then set all paramaters, see the comments inline
Tw = 25;        % use 25 ms frame
Ts = 10;        % hop size being 10 ms
alpha = 0.97;   % alpha is pre-emphasize factor
M = 20;         % number of filterbank channels
C = 12;         % number of Cepstral Coefficients
L = 22;         % lifter parameter
LF = 300;       % lower frequency of MFCC
HF = 3700;      % higher frequency of MFCC

N_GMM = 16;     % num of GMM classes
N_sample = 20;  % number of samples (total)
N_train = 18;   % number of training samples to use, should be less than 20

fs = 44100 / 5; % sampling rate

% Load each data.
MFCCs1 = [];
for sample_idx = 1:N_train
    % select the data where sample == sample_idx and class_label == 1
    speech = data(data(:, 1) == sample_idx & data(:, 2) == 1, 3);
    [ MFCC, ~, ~ ] = ...
        mfcc( speech, fs, Tw, Ts, alpha, @hamming, [LF HF], M, C+1, L );
    MFCCs1 = [ MFCCs1; MFCC'];
end
MFCCs2 = [];
for sample_idx = (1 + N_sample):(N_train + N_sample)
    speech = data(data(:, 1) == sample_idx & data(:, 2) == 2, 3);
    [ MFCC, ~, ~ ] = ...
        mfcc( speech, fs, Tw, Ts, alpha, @hamming, [LF HF], M, C+1, L );
    MFCCs2 = [ MFCCs2; MFCC'];
end

% After getting the feature, we train a GMM model, N_GMM

% GMM Training
tic
options = statset('MaxIter', 500);
GMModel1 = fitgmdist(MFCCs1, N_GMM, 'Options', options);
GMModel2 = fitgmdist(MFCCs2, N_GMM, 'Options', options);
toc

% Next we load data in and perform prediction.
test_class = 1;
test_index = (test_class - 1) * 20 + N_train + 1;
speech = data(data(:, 1) == test_index & data(:, 2) == test_class, 3);

% Perform the same feature extraction
[ MFCC, ~, ~ ] = ...
    mfcc( speech, fs, Tw, Ts, alpha, @hamming, [LF HF], M, C+1, L );
MFCCt= MFCC';

% get posterior
p = [pdf(GMModel1, MFCCt), pdf(GMModel2, MFCCt)];  % P(x|model_i)
[~, cIdx] = max(p,[],2);                           % argmax_i P(x|model_i)

% plot the prediction
figure
step = fs * Ts / 1000;
subplot(2, 1, 1)
plot(1:step:step*size(cIdx, 1), p)
legend('class 1', 'class 2')
subplot(2, 1, 2)
plot(speech + 1.5)

% In fact the accuracy is not very satisfactory
