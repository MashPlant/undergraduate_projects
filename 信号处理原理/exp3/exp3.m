[y1, ~] = audioread('exp3_1.mp3');
[y2, ~] = audioread('exp3_2.mp3');
[y3, fs] = audioread('exp3_3.mp3');
t = fs * 6; % fixed 6 seconds for each audio
% the original audio seems to be two-channel, discard one
y1 = reshape(y1(1:t, 1), [1, t]);
y2 = reshape(y2(1:t, 1), [1, t]);
y3 = reshape(y3(1:t, 1), [1, t]);
ys = {y1, y2, y3};
fprintf("playing original audio...\n");
soundsc([y1, y2, y3], fs);

for i = 1:length(ys)
    y = ys{i};
    figure(1); subplot(3, 1, i);
    stem(1:t, y, '.');
    figure(2); subplot(3, 1, i);
    stem(1:t, abs(fft(y)), '.');
end

upsampled = zeros(1, t * 3); % * 3 up sampling
fdm_f = zeros(1, t * 3);

upsampled(1:3:t * 3) = y1;
tmp = fft(upsampled); % matlab disallow me to write `fft(upsampled, t)(...)`...
figure(3); subplot(3, 1, 1); stem(1:t * 3, abs(tmp), '.');
fdm_f(t + 1:2 * t) = tmp(t + 1:2 * t); % mid
upsampled(1:3:t * 3) = y2;
tmp = fft(upsampled);
figure(3); subplot(3, 1, 2); stem(1:t * 3, abs(tmp), '.');
fdm_f(t / 2 + 1:t) = tmp(t / 2 + 1:t); % mid-l
fdm_f(2 * t + 1:2 * t + t / 2) = tmp(2 * t + 1:2 * t + t / 2); % mid-r
upsampled(1:3:t * 3) = y3;
tmp = fft(upsampled);
figure(3); subplot(3, 1, 3); stem(1:t * 3, abs(tmp), '.');
fdm_f(1:t / 2) = tmp(1:t / 2); % l
fdm_f(t * 2 + t / 2 + 1:3 * t) = tmp(t * 2 + t / 2 + 1:3 * t); % r

fdm_t = real(ifft(fdm_f)); % only real signal can be transmitted
figure(4); subplot(2, 1, 1);
stem(1:t * 3, abs(fdm_f), '.');
figure(4); subplot(2, 1, 2);
stem(1:t * 3, fdm_t, '.');

rec_f = fft(fdm_t); % rec for recover
rec_y1_f = rec_f(t + 1:2 * t);
rec_y2_f = [rec_f(2 * t + 1:2 * t + t / 2), rec_f(t / 2 + 1:t)];
rec_y3_f = [rec_f(1:t / 2), rec_f(t * 2 + t / 2 + 1:3 * t)];
rec_ys_f = {rec_y1_f, rec_y2_f, rec_y3_f};
rec_ys = {real(ifft(rec_y1_f)), real(ifft(rec_y2_f)), real(ifft(rec_y3_f))};
fprintf("playing recovered audio...\n");
soundsc([rec_ys{1}, rec_ys{2}, rec_ys{3}], fs);

for i = 1:length(rec_ys_f)
    figure(5); subplot(3, 1, i);
    stem(1:t, abs(rec_ys_f{i}), '.');
    figure(6); subplot(3, 1, i);
    stem(1:t, rec_ys{i}, '.');
end