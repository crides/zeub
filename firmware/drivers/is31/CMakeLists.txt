# Copyright (c) 2020 The ZMK Contributors
# SPDX-License-Identifier: MIT

zephyr_library()

zephyr_include_directories(.)

zephyr_library_sources_ifdef(CONFIG_IS31FL3729 is31fl3729.c)
zephyr_library_sources_ifdef(CONFIG_IS31FL3741 is31fl3741.c)
