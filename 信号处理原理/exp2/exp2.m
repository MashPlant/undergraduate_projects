run(1, 1000:500:10000, 1000, {'conv\_naive', 'conv\_fft', 'conv\_overlap\_save', 'conv\_overlap\_add'}, ...
    {@conv_naive, @conv_fft, @conv_overlap_save, @conv_overlap_add});

run(2, 10000:5000:100000, 1000, {'conv\_fft', 'conv\_overlap\_save', 'conv\_overlap\_add'}, ...
    {@conv_fft, @conv_overlap_save, @conv_overlap_add});

% driver for testing
% `fig_id`: the id of figure that the result is drawn on
% `N_range`: a vec of a set of N (length(x)); `M`: length(y);
% `names`: names of solvers; `solvers`: a vec of fn(vec, vec) -> vec,
% elements can be `conv_fft`, `conv_overlap_save`, `conv_overlap_add` or `conv_overlap_add`
% length(names) should be equal to length(`solvers`)
function run(fig_id, N_range, M, names, solvers)
    times = zeros(length(solvers), length(N_range));

    for Ni = 1:length(N_range)
        N = N_range(Ni);
        results = zeros(length(solvers), N + M - 1);
        fprintf("testing N = %d, M = %d\n", N, M);
        x = rand([1, N]);
        y = rand([1, M]);

        for i = 1:length(solvers)
            solver = solvers{i};
            solver(x, y);
            beg = clock();
            % run 10 times and record average time
            for j = 1:10 results(i, :) = solver(x, y); end
            times(i, Ni) = etime(clock(), beg) / 10;
        end

        % calculate L1 error, verify correctness
        err = sum(abs(results(2:length(solvers), :) - results(1, :)), 2);
        fmt = ['L1 errors: [', repmat('%f, ', 1, length(err) - 1), '%f]\n'];
        fprintf(fmt, err);
    end

    figure(fig_id);
    hold off

    for i = 1:length(solvers)
        plot(N_range, times(i, :), '-');
        hold on
    end

    legend(names, 'Location', 'northwest');
    xlabel(sprintf('N(M = %d)', M));
    ylabel('time/s');
end

% compute conv by definition
function ret = conv_naive(x, y)
    N = length(x);
    M = length(y);
    L = N + M - 1;
    ret = zeros(1, L);

    for n = 1:L

        for m = max(1, n + 1 - N):min(M, n)
            ret(n) = ret(n) + x(n - m + 1) * y(m);
        end

    end

end

% compute conv using FFT
function ret = conv_fft(x, y)
    % do (N + M - 1)-point DFT and IDFT to avoid wrapping affecting the result
    L = length(x) + length(y) - 1;
    ret = ifft(fft(x, L) .* fft(y, L));
end

% compute conv using overlap-save
function ret = conv_overlap_save(x, y)
    N = length(x);
    M = length(y);
    bs = M * 4; % bs for block size, I choose `bs = M * 4` arbitrarily
    L = bs + M - 1;
    fft_y = fft(y, L);
    ret = zeros(1, N + M - 1);

    for i = 0:floor((N + M - 2) / bs)
        lo = i * bs - M + 2;
        hi = min(N, lo + L - 1);
        % overlap-save: x(-M + 2:bs), x(-M + 2 + bs:bs * 2), ... there is overlap
        % (suppose x is an online input, we have to save its old value for next computation, so "save")
        % for index < 1, must manually pad 0 in the front
        if (lo >= 1) subx = x(lo:min(N, lo + L - 1)); else subx = [zeros(1, 1 - lo), x(1:hi)]; end
        tmp = ifft(fft(subx, L) .* fft_y);
        lo = i * bs + 1;
        hi = min(lo + bs - 1, N + M - 1);
        % but theres is no overlap on result
        ret(lo:hi) = tmp(M:hi - lo + M);
    end

end

% compute conv using overlap-save
function ret = conv_overlap_add(x, y)
    N = length(x);
    M = length(y);
    bs = M * 4;
    bn = floor(N / bs); % bn for block number
    L = bs + M - 1;
    fft_y = fft(y, L);
    ret = zeros(1, bn * bs + L);

    for i = 0:bn
        lo = i * bs + 1;
        tmp = ifft(fft(x(lo:min(N, (i + 1) * bs)), L) .* fft_y);
        % overlap-add: ret(1:L), ret(bs + 1:bs + L), ... there is overlap
        ret(lo:lo + L - 1) = ret(lo:lo + L - 1) + tmp;
    end

    ret = ret(1:N + M - 1);
end
