// Harness for https://github.com/laurikari/tre/ focusing on tre_regncomp.
// TODO: Harness and fuzz tre_regexec too

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../local_includes/tre.h"

__AFL_FUZZ_INIT();

int main() {
    #ifdef __AFL_HAVE_MANUAL_CONTROL
  __AFL_INIT();
    #endif

    unsigned char *buf = __AFL_FUZZ_TESTCASE_BUF;  // must be after __AFL_INIT
                                                 // and before __AFL_LOOP!

    // AFL persistent loop: run the body up to 1000 times per fork
    while (__AFL_LOOP(10000)) {
        // Read up to 4096 bytes from stdin
        int len = __AFL_FUZZ_TESTCASE_LEN;  // don't use the macro directly in a
                                        // call!

        if (len < 8) continue;  // check for a required/useful minimum input length

        regex_t preg;
        int flags = REG_EXTENDED | REG_NEWLINE;

        // Try compiling fuzz input as regex pattern
        if (tre_regncomp(&preg, (const char *)buf, len, flags) == 0) {
            const char *test_str = "persistent test string 123";
            regmatch_t pmatch[10];

            tre_have_backrefs(&preg);
            tre_have_approx(&preg);

            // Try matching the test string
            tre_regexec(&preg, test_str, 10, pmatch, 0);
        }
        tre_regfree(&preg);
    }

    return 0;
}
