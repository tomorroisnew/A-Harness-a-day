#include <vector>
#include <unistd.h>     // For read()
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include "fpng.h"
#include "fpng.cpp"

extern "C" {
    __AFL_FUZZ_INIT();
}

int main(int argc, char** argv) {
#ifdef __AFL_HAVE_MANUAL_CONTROL
    __AFL_INIT();
#endif

    fpng::fpng_init();

    uint8_t* buf = __AFL_FUZZ_TESTCASE_BUF;

    while (__AFL_LOOP(1000)) {
        int len = __AFL_FUZZ_TESTCASE_LEN;
        if (len < 1)
            continue;

        std::vector<uint8_t> out_buf;
        uint32_t width = 0, height = 0, channels = 0;

        // decode directly from memory
        fpng::fpng_decode_memory(buf, len, out_buf, width, height, channels, 4);

        // cleanup between iterations
        out_buf.clear();
    }

    return 0;
}
