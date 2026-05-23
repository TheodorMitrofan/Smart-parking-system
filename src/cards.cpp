#include "cards.h"
#include <string.h>

static const uint8_t known_uids[][4] = {
    { 0x85, 0x64, 0xF3, 0x06 },
    { 0xE6, 0xD7, 0x10, 0x06 },
};
static const uint8_t num_known = sizeof(known_uids) / sizeof(known_uids[0]);

uint8_t card_authorized(const uint8_t* uid) {
    for (uint8_t i = 0; i < num_known; i++) {
        if (memcmp(uid, known_uids[i], 4) == 0) return 1;
    }
    return 0;
}
