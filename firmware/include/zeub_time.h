#pragma once

#include <zephyr/sys/timeutil.h>

struct time_state {
    struct tm time;
    uint8_t frag;
};

extern volatile bool time_synced;

void time_set_cur(const struct time_state *time);
void log_time_state(const char *msg, const struct time_state *time);
int time_get_unix_time(struct time_state *time);
uint64_t time_state_timegm(const struct time_state *time);
uint32_t time_state_diff_ms(const struct time_state *a, const struct time_state *b);
