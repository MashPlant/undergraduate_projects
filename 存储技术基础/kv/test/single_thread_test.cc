#include <assert.h>
#include <stdio.h>
#include <string>

#include "include/engine.h"
#include "test_util.h"

using namespace polar_race;

#define KV_CNT 10000

char k[1024];
char v[9024];
std::string ks[KV_CNT];
std::string vs_1[KV_CNT];
std::string vs_2[KV_CNT];
int main() {

    Engine *engine = NULL;
    printf_(
        "======================= single thread test "
        "============================");
    std::string engine_path =
        std::string("./data/test-") + std::to_string(asm_rdtsc());
    RetCode ret = Engine::Open(engine_path, &engine);
    assert(ret == kSucc);
    printf("open engine_path: %s\n", engine_path.c_str());

    /////////////////////////////////
    gen_random(k, 23);
    gen_random(v, 1);

    std::string value;
    ret = engine->Read(k, &value);
    assert(ret == kNotFound);

    ret = engine->Write(k, v);
    assert(ret == kSucc);

    ret = engine->Read(k, &value);
    assert(ret == kSucc);
    assert(value == v);

    gen_random(v, 111);
    ret = engine->Write(k, v);
    assert(ret == kSucc);

    ret = engine->Read(k, &value);
    assert(ret == kSucc);
    assert(value == v);

    ////////////////////////////////////
    gen_random(k, 13);
    gen_random(v, 4097);

    ret = engine->Write(k, v);
    assert(ret == kSucc);

    ret = engine->Read(k, &value);
    assert(ret == kSucc);
    assert(value == v);

    ///////////////////////////////////
    for (int i = 0; i < KV_CNT; ++i) {
        gen_random(k, 6);
        ks[i] = std::string(k) + std::to_string(i);
        gen_random(v, 1027);
        vs_1[i] = v;
        gen_random(v, 1027);
        vs_2[i] = v;
    }

    for (int i = 0; i < KV_CNT; ++i) {
        ret = engine->Write(ks[i], vs_1[i]);
        assert(ret == kSucc);
    }

    for (int i = 0; i < KV_CNT; ++i) {
        ret = engine->Read(ks[i], &value);
        assert(ret == kSucc);
        assert(value == vs_1[i]);
    }

    for (int i = 0; i < KV_CNT; ++i) {
        if (i % 2 == 0) {
            ret = engine->Write(ks[i], vs_2[i]);
            assert(ret == kSucc);
        }
    }

    for (int i = 0; i < KV_CNT; ++i) {
        ret = engine->Read(ks[i], &value);
        assert(ret == kSucc);

        if (i % 2 == 0) {
            assert(value == vs_2[i]);
        } else {
            assert(value == vs_1[i]);
        }
    }

    delete engine;

    // re-open
    ret = Engine::Open(engine_path, &engine);
    assert(ret == kSucc);
    for (int i = 0; i < KV_CNT; ++i) {
        ret = engine->Read(ks[i], &value);
        assert(ret == kSucc);

        if (i % 2 == 0) {
            assert(value == vs_2[i]);
        } else {
            assert(value == vs_1[i]);
        }
    }

    printf_(
        "======================= single thread test pass :) "
        "======================");

    return 0;
}
