#include <stdint.h>

#include "settings.h"
#include "flash.h"

#define SETTINGS_PAGE_NUM      511
#define SETTINGS_PAGE_SIZE     FLASH_PAGE_SIZE
#define SETTINGS_PAGE_ADDR     (flash_get_page_addr(SETTINGS_PAGE_NUM))
#define SETTINGS_PAGE_ADDR_END (SETTINGS_PAGE_ADDR + SETTINGS_PAGE_SIZE - 1)

struct settings settings = {
    .delay_us = 50000,
    .pulse_width_ns = 200,
    .train_period_us = 402,
    .cont_period_ns = 1000,
    .pol = 1,
    .train_count = 2,
};

static struct settings *pflash_settings = (struct settings *)SETTINGS_PAGE_ADDR;

void settings_init(void)
{
    if (pflash_settings->magic_key != SETTINGS_MAGIC_KEY) {
        return;
    }

    do {
        pflash_settings++;
        uint32_t settings_addr_end = (uint32_t)pflash_settings
                                   + sizeof(struct settings);
        if ((settings_addr_end - 1) > SETTINGS_PAGE_ADDR_END) {
            break;
        }
    } while (pflash_settings->magic_key == SETTINGS_MAGIC_KEY);

    settings = *--pflash_settings;
}

void settings_write_to_flash(void)
{
    pflash_settings++;
    uint32_t settings_addr_end = (uint32_t)pflash_settings
                               + sizeof(struct settings);

    if ((settings_addr_end - 1) > SETTINGS_PAGE_ADDR_END) {
        flash_erase_page(SETTINGS_PAGE_NUM);
        pflash_settings = (struct settings *)SETTINGS_PAGE_ADDR;
    }
    flash_memcpy_u32(&settings, pflash_settings, sizeof(struct settings));
}
