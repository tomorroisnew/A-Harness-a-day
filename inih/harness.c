#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ini.h"
#include <unistd.h>

__AFL_FUZZ_INIT();

typedef struct {
    const char* city;
    const char* country;
} address;

typedef struct {
    int version;
    const char* name;
    const char* email;
    char booltest;
    address addr;
} configuration;

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    configuration* pconfig = (configuration*)user;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (MATCH("protocol", "version")) {
        pconfig->version = atoi(value);
    } else if (MATCH("user", "name")) {
        pconfig->name = strdup(value);
    } else if (MATCH("user", "email")) {
        pconfig->email = strdup(value);
    } else if (MATCH("user", "city")) {
        pconfig->addr.city = strdup(value);
    } else if (MATCH("user", "country")) {
        pconfig->addr.country = strdup(value);
    } else {
        return 0;  // unknown field
    }

    return 1;
}

int main(void)
{
    
    #ifdef __AFL_HAVE_MANUAL_CONTROL
        __AFL_INIT();
    #endif

    unsigned char *buf = __AFL_FUZZ_TESTCASE_BUF;  // must be after __AFL_INIT
                                                 // and before __AFL_LOOP!

    while (__AFL_LOOP(10000)) {

        int len = __AFL_FUZZ_TESTCASE_LEN;  // don't use the macro directly in a
                                        // call!

        if (len < 3) continue;  // check for a required/useful minimum input length

        configuration config = {0};

        if (ini_parse_string(buf, handler, &config) < 0) {
            printf("Failed to parse INI data.\n");
            return 1;
        }

        printf("Parsed INI data:\n");
        printf("  version: %d\n", config.version);
        printf("  name:    %s\n", config.name ? config.name : "(null)");
        printf("  email:   %s\n", config.email ? config.email : "(null)");
        printf("  city:    %s\n", config.addr.city ? config.addr.city : "(null)");
        printf("  country: %s\n", config.addr.country ? config.addr.country : "(null)");

        // Cleanup
        free((void*)config.name);
        free((void*)config.email);
        free((void*)config.addr.city);
        free((void*)config.addr.country);
    }

    return 0;
}
