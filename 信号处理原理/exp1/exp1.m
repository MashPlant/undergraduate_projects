% dtmf parameters
global KEYS
global ROWS
global COLS
KEYS = ['1', '2', '3', 'A';
    '4', '5', '6', 'B';
    '7', '8', '9', 'C';
    '*', '0', '#', 'D'];
ROWS = [697, 770, 852, 941];
COLS = [1209, 1336, 1477, 1633];

% parameters for generated audio signal
fs = 48000;
step = 5000;
keys = '0123456789ABCD*#';
t = 0:1 / fs:1; % one second with `fs` sampling point
x = zeros(1, length(t) * length(keys)); % preallocation

for i = 1:length(keys)
    [r, c] = find(keys(i) == KEYS);
    % add one second's sound to the generated audio signal
    x((i - 1) * length(t) + 1:i * length(t)) = sin(2 * pi * ROWS(r) * t) + sin(2 * pi * COLS(c) * t);
end

disp('dtmf_goertzel on generated input');
disp(solve(x, fs, step, @dtmf_goertzel));
disp('dtmf_fft on generated input');
disp(solve(x, fs, step, @dtmf_fft));

[x, fs] = audioread('exp1.wav');
disp('dtmf_goertzel on real input');
disp(solve(x, fs, step, @dtmf_goertzel));
disp('dtmf_fft on real input');
disp(solve(x, fs, step, @dtmf_fft));

% driver for testing, returns the recognition result and print time usage
% `x`: input audio signal; `fs`: sampling freq; 
% `step`: how many samping points does a single round of recognition takes
% `solver`: fn(x, fs) -> key, it can be `dtmf_goertzel` or `dtmf_fft`
function ret = solve(x, fs, step, solver)
    tic
    ret = [];

    for i = 1:step:length(x) - step
        ret = [ret, solver(x(i:i + step), fs)];
    end

    toc
end

% recognize dtmf keys by goertzel
% `x`: input audio signal; `fs`: sampling freq
function ret = dtmf_goertzel(x, fs)
    global KEYS
    global ROWS
    global COLS

    % `all` is the factor of y(n - 1) in IIR
    all = [ROWS, COLS] * length(x) / fs;
    all = 2 * cos(2 * pi * all / length(x));
    y1 = zeros(1, length(all));
    y2 = zeros(1, length(all));

    % vectorization, run 8 IIR together
    for xi = x
        y = xi + all .* y1 - y2;
        y2 = y1;
        y1 = y;
    end

    p = y1.^2 + y2.^2 - all .* y1 .* y2; % p for power
    [rm, r] = max(p(1:4));
    [cm, c] = max(p(5:8));
    % this is an arbitrary threshold, and tuning it cannot improve accuracy much
    if (rm > 100 || cm > 100) ret = KEYS(r, c); else ret = ''; end
end

% recognize dtmf keys by fft
% `x`: input audio signal; `fs`: sampling freq
function ret = dtmf_fft(x, fs)
    global KEYS
    global ROWS
    global COLS

    X = abs(fft(x, fs));
    [rm, r] = max(X(ROWS));
    [cm, c] = max(X(COLS));
    if (rm > 100 || cm > 100) ret = KEYS(r, c); else ret = ''; end
end