#include <omp.h>
#include <signal.h>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using i8 = char;
using f32 = float;
using i32 = int;
using u32 = unsigned;

const u32 THREAD = 8;
const u32 LAYER_IN = 60000;
const u32 LAYER1 = 100;
const u32 LAYER2 = 100;
const u32 LAYER_OUT = 8;

struct NetParam {
  f32 w1[LAYER1][LAYER_IN];
  f32 b1[LAYER1];
  f32 w2[LAYER2][LAYER1];
  f32 b2[LAYER2];
  f32 w3[LAYER_OUT][LAYER2];
  f32 b3[LAYER_OUT];
} net, upd[THREAD];

struct Neuron {
  f32 x1[LAYER1];
  f32 x2[LAYER2];
  f32 x3[LAYER_OUT];
} fwd[THREAD];

inline f32 relu(f32 x) { return x > 0 ? x : 0; }

inline f32 diff_relu(f32 y) { return y > 0; }

inline u32 argmax(const f32 *x, u32 n) {
  u32 ret = 0;
  f32 ans = -1e9f;
  for (u32 i = 0; i < n; ++i) {
    if (ans < x[i]) {
      ans = x[i];
      ret = i;
    }
  }
  return ret;
}

void forward(Neuron &f, const f32 *in) {
  for (u32 i = 0; i < LAYER1; ++i) {
    f32 tmp = 0;
    const f32 *a = net.w1[i], *b = in;
    for (u32 j = 0; j < LAYER_IN; ++j) {
      tmp += a[j] * b[j];
    }
    f.x1[i] = relu(tmp + net.b1[i]);
  }
  for (u32 i = 0; i < LAYER2; ++i) {
    f32 tmp = 0;
    const f32 *a = net.w2[i], *b = f.x1;
    for (u32 j = 0; j < LAYER1; ++j) {
      tmp += a[j] * b[j];
    }
    f.x2[i] = relu(tmp + net.b2[i]);
  }
  f32 sum = 0;  // for softmax
  for (u32 i = 0; i < LAYER_OUT; ++i) {
    f32 tmp = 0;
    const f32 *a = net.w3[i], *b = f.x2;
    for (u32 j = 0; j < LAYER2; ++j) {
      tmp += a[j] * b[j];
    }
    sum += f.x3[i] = expf(tmp + net.b3[i]);
  }
  for (f32 &x : f.x3) {
    x /= sum;
  }
}

void backward(const Neuron &f, const f32 *in, const f32 *out, NetParam &grad) {
  // the size of tmp_b & tmp_x should be max{hidden layers size}
  f32 tmp_b[LAYER1];  // grad of b, also grad of activation(x)
  f32 tmp_x[LAYER1];  // grad of x
  for (u32 i = 0; i < LAYER_OUT; ++i) {
    grad.b3[i] += tmp_b[i] = out[i] - f.x3[i];
  }
  memset(tmp_x, 0, LAYER2 * sizeof(f32));
  for (u32 i = 0; i < LAYER_OUT; ++i) {
    for (u32 j = 0; j < LAYER2; ++j) {
      grad.w3[i][j] += tmp_b[i] * f.x2[j];
      tmp_x[j] += tmp_b[i] * net.w3[i][j];
    }
  }
  for (u32 i = 0; i < LAYER2; ++i) {
    grad.b2[i] += tmp_b[i] = diff_relu(f.x2[i]) * tmp_x[i];
  }
  memset(tmp_x, 0, LAYER1 * sizeof(f32));
  for (u32 i = 0; i < LAYER2; ++i) {
    for (u32 j = 0; j < LAYER1; ++j) {
      grad.w2[i][j] += tmp_b[i] * f.x1[j];
      tmp_x[j] += tmp_b[i] * net.w2[i][j];
    }
  }
  for (u32 i = 0; i < LAYER1; ++i) {
    grad.b1[i] += tmp_b[i] = diff_relu(f.x1[i]) * tmp_x[i];
  }
  // no need to set tmp_x
  for (u32 i = 0; i < LAYER1; ++i) {
    for (u32 j = 0; j < LAYER_IN; ++j) {
      grad.w1[i][j] += tmp_b[i] * in[j];
      // no need to set tmp_x
    }
  }
}

struct XorShiftRNG {
  u32 seed;

  u32 gen_u32() {
    seed ^= seed << 13u;
    seed ^= seed >> 17u;
    seed ^= seed << 5u;
    return seed;
  }

  f32 gen_f32(f32 lo, f32 hi) { return lo + f32(gen_u32()) / -1u * (hi - lo); }
};

const u32 TOT = 2342;
const u32 TRAIN = 2048;
const u32 VAL = TOT - TRAIN;
const u32 TEST = 2228;
f32 input_x[TRAIN][LAYER_IN];
f32 input_y[TRAIN][LAYER_OUT];
f32 val_x[VAL][LAYER_IN];
f32 val_y[VAL][LAYER_OUT];
f32 test_x[TEST][LAYER_IN];  // this is not cause a waste of space, os can optimize it out
f32 test_y[TEST][LAYER_OUT];
Neuron test_output[TEST];

const u32 BATCH_SIZE = 512;
const u32 ITER = TRAIN / BATCH_SIZE + 1;
const u32 EPOCH = 50;
const f32 LEARN_RATE = 0.05;

i32 main(i32 argc, i8 **argv) {
  if (argc == 2 && argv[1][0] == 'i') {  // i for inference
    {
      FILE *f = fopen("data/test_x_small.bin", "r");
      fread(test_x, sizeof(test_x), 1, f);
      f = freopen("data/test_y.bin", "r", f);
      fread(test_y, sizeof(test_y), 1, f);
      f = freopen("output/dnn.bin", "r", f);
      fread(&net, sizeof(net), 1, f);
      fclose(f);
    }
    f32 errs[THREAD]{};
    u32 accs[THREAD]{};
#pragma omp parallel for num_threads(THREAD)
    for (u32 i = 0; i < TEST; ++i) {
      u32 tid = omp_get_thread_num();
      forward(test_output[i], test_x[i]);
      f32 err = 0;
      for (u32 j = 0; j < LAYER_OUT; ++j) {
        err += -test_y[i][j] * logf(test_output[i].x3[j]);
      }
      errs[tid] += err;
      accs[tid] += argmax(test_y[i], LAYER_OUT) == argmax(test_output[i].x3, LAYER_OUT);
    }
    f32 tot_err = 0;
    for (f32 err : errs) {
      tot_err += err;
    }
    u32 tot_acc = 0;
    for (u32 grad : accs) {
      tot_acc += grad;
    }
    printf("test: err per run: %f, acc rate = %f\n", tot_err / TEST, f32(tot_acc) / TEST);
    // just to aggregate result, avoid too many fwrite
    for (u32 i = 0; i < TEST; ++i) {
      memcpy(test_y[i], test_output[i].x3, sizeof(fwd[i].x3));
    }
    {
      FILE *f = fopen("output/dnn_val.bin", "w");
      fwrite(test_y, sizeof(test_y), 1, f);
      fclose(f);
    }
    exit(0);
  }
  signal(SIGINT, [](i32) {
    puts("received SIGINT, save current model?[y/n]");
    if (getchar() == 'y') {
      FILE *f = fopen("output/dnn.bin", "w");
      fwrite(&net, sizeof(net), 1, f);
      fclose(f);
    }
    exit(0);
  });
  {
    FILE *f = fopen("data/train_x_small.bin", "r");
    fread(input_x, sizeof(input_x), 1, f);
    f = freopen("data/train_y.bin", "r", f);
    fread(input_y, sizeof(input_y), 1, f);
    f = freopen("data/validate_x_small.bin", "r", f);
    fread(val_x, sizeof(val_x), 1, f);
    f = freopen("data/validate_y.bin", "r", f);
    fread(val_y, sizeof(val_y), 1, f);
    fclose(f);
  }
  XorShiftRNG rngs[THREAD];
  f32 errs[THREAD];
  u32 accs[THREAD];
  for (u32 i = 0; i < THREAD; ++i) {
    rngs[i].seed = i * 192608167 + 1;
  }
  // Xaiver init
  {
    XorShiftRNG init{19660813};
    f32 hi = sqrtf(6.0f / (LAYER_IN + LAYER1)), lo = -hi;
    for (u32 i = 0; i < LAYER1; ++i) {
      for (u32 j = 0; j < LAYER_IN; ++j) {
        net.w1[i][j] = init.gen_f32(lo, hi);
      }
    }
    hi = sqrtf(6.0f / (LAYER1 + LAYER2)), lo = -hi;
    for (u32 i = 0; i < LAYER2; ++i) {
      for (u32 j = 0; j < LAYER1; ++j) {
        net.w2[i][j] = init.gen_f32(lo, hi);
      }
    }
    hi = sqrtf(6.0f / (LAYER2 + LAYER_OUT)), lo = -hi;
    for (u32 i = 0; i < LAYER_OUT; ++i) {
      for (u32 j = 0; j < LAYER2; ++j) {
        net.w3[i][j] = init.gen_f32(lo, hi);
      }
    }
  }
  auto beg = std::chrono::high_resolution_clock::now();
  f32 max_val_acc = 0.0f;
  for (u32 epoch = 0; epoch < EPOCH; ++epoch) {
    printf("epoch %d, elapsed %.3fs\n", epoch, std::chrono::duration<f32>(std::chrono::high_resolution_clock::now() - beg).count());
    memset(errs, 0, sizeof(errs));
    memset(accs, 0, sizeof(accs));
    for (u32 iter = 0; iter < ITER; ++iter) {
#pragma omp parallel for num_threads(THREAD)
      for (u32 batch = 0; batch < BATCH_SIZE; ++batch) {
        u32 tid = omp_get_thread_num();
        u32 idx = rngs[tid].gen_u32() % TRAIN;

        forward(fwd[tid], input_x[idx]);
        f32 err = 0;
        for (u32 i = 0; i < LAYER_OUT; ++i) {
          err += -input_y[idx][i] * logf(fwd[tid].x3[i]);
        }
        errs[tid] += err;
        accs[tid] += argmax(input_y[idx], LAYER_OUT) == argmax(fwd[tid].x3, LAYER_OUT);
        backward(fwd[tid], input_x[idx], input_y[idx], upd[tid]);
      }
      for (NetParam &u : upd) {
        f32 *np = (f32 *)&net, *up = (f32 *)&u;
#pragma omp parallel for num_threads(THREAD)
        for (u32 i = 0; i < sizeof(NetParam) / sizeof(f32); ++i) {
          np[i] += up[i] * (LEARN_RATE / BATCH_SIZE);
          up[i] = 0;
        }
      }
    }
    {
      f32 tot_err = 0;
      for (f32 err : errs) {
        tot_err += err;
      }
      u32 tot_acc = 0;
      for (u32 grad : accs) {
        tot_acc += grad;
      }
      printf("train: err per run: %f, acc rate = %f\n", tot_err / (BATCH_SIZE * ITER), f32(tot_acc) / (BATCH_SIZE * ITER));
    }
    {
      memset(errs, 0, sizeof(errs));
      memset(accs, 0, sizeof(accs));
#pragma omp parallel for num_threads(THREAD)
      for (u32 i = 0; i < VAL; ++i) {
        u32 tid = omp_get_thread_num();
        forward(fwd[tid], val_x[i]);
        f32 err = 0;
        for (u32 j = 0; j < LAYER_OUT; ++j) {
          err += -val_y[i][j] * logf(fwd[tid].x3[j]);
        }
        errs[tid] += err;
        accs[tid] += argmax(val_y[i], LAYER_OUT) == argmax(fwd[tid].x3, LAYER_OUT);
      }
      f32 tot_err = 0;
      for (f32 err : errs) {
        tot_err += err;
      }
      u32 tot_acc = 0;
      for (u32 grad : accs) {
        tot_acc += grad;
      }
      f32 acc = f32(tot_acc) / VAL;
      printf("validate: err per run: %f, acc rate = %f\n", tot_err / VAL, acc);
      if (max_val_acc < acc) {
        max_val_acc = acc;
        printf("update max_val_acc = %.3f\n", max_val_acc);
        {
          FILE *f = fopen("output/dnn.bin", "w");
          fwrite(&net, sizeof(net), 1, f);
          fclose(f);
        }
      }
    }
  }
}