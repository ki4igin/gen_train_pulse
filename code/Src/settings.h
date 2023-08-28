#ifndef __SETTINGS_H
#define __SETTINGS_H

#include <stdint.h>
#include <assert.h>

#define SETTINGS_MAGIC_KEY 0x00112233
#define SETTINGS_SIZE      32

struct settings {
    uint32_t magic_key;
    uint32_t delay_us;
    uint32_t pulse_width_ns;
    uint32_t train_period_us;
    uint32_t cont_period_ns;
    uint32_t pol;
    uint32_t train_count;
    uint32_t resv;
};

extern struct settings settings;

void settings_init(void);
void settings_write_to_flash(void);

static_assert(sizeof(struct settings) == SETTINGS_SIZE, "Err");

#endif
