#pragma once

#include "zeub_time.h"

void strip_handle_time(const struct time_state *time);

struct strip_cmd {
    uint8_t hour, min;
    uint8_t white, yellow;
};

/* extern struct strip_cmd[CONFIG_STRIP_CMD_MAX_LEN]; */
void change_bright(const uint8_t strip, const int8_t change);
