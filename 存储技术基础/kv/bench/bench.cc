#include <thread>

#include "bench_util.h"
#include "zipf.h"
#include "include/engine.h"

#define MAX_THREAD 64
#define OP_PER_THREAD 200000ull
#define KEY_SPACE (OP_PER_THREAD * 4)
#define KEY_SIZE 8
#define VALUE_SIZE 4096

using namespace polar_race;

int threadNR = 1;
int readNR = 100;
bool isSkew = 0;

Engine *engine = NULL;

void usage() {
    fprintf(stderr,
            "Usage: ./bench thread_num[1-64] read_ratio[0-100] isSkew[0|1] \n");
    exit(-1);
}

void parseArgs(int argc, char **argv) {
    if (argc != 4) {
        usage();
    }
    threadNR = std::atoi(argv[1]);
    readNR = std::atoi(argv[2]);
    int k = std::atoi(argv[3]);
    isSkew = k;

    if (threadNR <= 0 || threadNR > 64) usage();
    if (readNR < 0 || readNR > 100) usage();
    if (k != 0 && k != 1) usage();

    fprintf(stdout, "thread_num: %d, read ratio: %d%%, isSkew: %s\n", threadNR,
            readNR, isSkew ? "true" : "false");
}

void bench_thread(int id) {

    struct zipf_gen_state state;
    unsigned int seed = asm_rdtsc() + id;

    char v[5000];
    std::string value;
    gen_random(v, 4096);
    mehcached_zipf_init(&state, KEY_SPACE, isSkew ? 0.99 : 0,
                        asm_rdtsc() >> 17);
    for (int i = 0; i < OP_PER_THREAD; ++i) {
        bool isRead = (rand_r(&seed) % 100) < readNR;
        uint64_t key = mehcached_zipf_next(&state);
        PolarString k((char *)&key, sizeof(uint64_t));
        if (isRead) {
            engine->Read(k, &value);
        } else {
            engine->Write(k, v);
        }
    }
}

int main(int argc, char **argv) {
    parseArgs(argc, argv);

    system("mkdir -p data");
    std::string engine_path =
        std::string("./data/test-") + std::to_string(asm_rdtsc());
    printf("open engine_path: %s\n", engine_path.c_str());

    RetCode ret = Engine::Open(engine_path, &engine);
    assert(ret == kSucc);

    char v[5000];
    gen_random(v, 4096);
    for (int i = 0; i < KEY_SPACE; i += 100) {
        uint64_t key = i;
        PolarString k((char *)&key, sizeof(uint64_t));
        engine->Write(k, v);
    }
    delete engine;

    std::thread ths[MAX_THREAD];

    timespec s, e;

    clock_gettime(CLOCK_REALTIME, &s);
    ret = Engine::Open(engine_path, &engine);
    assert(ret == kSucc);
    for (int i = 0; i < threadNR; ++i) {
        ths[i] = std::thread(bench_thread, i);
    }

    for (int i = 0; i < threadNR; ++i) {
        ths[i].join();
    }
    clock_gettime(CLOCK_REALTIME, &e);

    double us = (e.tv_sec - s.tv_sec) * 1000000 +
                (double)(e.tv_nsec - s.tv_nsec) / 1000;
    printf("%d thread, %d operations per thread, time: %lfus\n", threadNR, OP_PER_THREAD, us);
    printf("throughput %lf operations/s\n", 1ull * (threadNR * OP_PER_THREAD) * 1000000 / us);

    delete engine;

    system((std::string("rm -rf ") + engine_path).c_str());

    return 0;
}
