#ifndef __SETTINGS_H
#define __SETTINGS_H

#include <stdint.h>
#include <assert.h>

#define SETTINGS_MAGIC_KEY 0x00112233

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
uint32_t settings_is_change(void);
void settings_write_to_flash(void);

static_assert((sizeof(struct settings) & 0b111) == 0,
              "Err: size setting from save to flash must be a multiple of 8");

#endif
