# Copyright (c) 2021 Guillaume Paquet <guillaume.paquet@smile.fr>
# SPDX-License-Identifier: Apache-2.0

# board_runner_args(jlink "--device=nRF52840_xxAA" "--speed=4000" "--iface=swd" "--reset")
board_runner_args(jlink "--device=nRF52840_xxAA" "--speed=4000")
# board_runner_args(pyocd "--target=nrf52840" "--frequency=4000000")
# include(${ZEPHYR_BASE}/boards/common/nrfutil.board.cmake)
include(${ZEPHYR_BASE}/boards/common/nrfjprog.board.cmake)
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
# include(${ZEPHYR_BASE}/boards/common/pyocd.board.cmake)
# include(${ZEPHYR_BASE}/boards/common/openocd-nrf5.board.cmake)
