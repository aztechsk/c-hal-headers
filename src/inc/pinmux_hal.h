/*
 * pinmux_hal.h
 *
 * Copyright (c) 2025 Jan Rusnak <jan@rusnak.sk>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef PINMUX_HAL_H
#define PINMUX_HAL_H

#include <gentyp.h>
#include <stdint.h>

/**
 * @file pinmux_hal.h
 * @brief Platform-independent Pin Multiplexing HAL.
 *
 * The pinmux HAL allows selecting which peripheral or GPIO function
 * controls each physical pin. It mirrors Linux-style pinctrl concepts
 * but with a simplified procedural API.
 */

/**
 * Note:
 * This API is designed for single-pin operations.
 * Each function expects exactly one bit set in @p pin_mask.
 * Passing multi-bit masks results in a fatal parameter error.
 */

/** @brief Alternate function selector (AF0–AF15). */
enum pinmux_hal_func {
	PINMUX_HAL_FUNC_GPIO_IN,   /**< Pin controlled by GPIO/PIO (INPUT). */
	PINMUX_HAL_FUNC_GPIO_OUT,  /**< Pin controlled by GPIO/PIO (OUTPUT). */
	PINMUX_HAL_AF0,
	PINMUX_HAL_AF1,
	PINMUX_HAL_AF2,
	PINMUX_HAL_AF3,
	PINMUX_HAL_AF4,
	PINMUX_HAL_AF5,
	PINMUX_HAL_AF6,
	PINMUX_HAL_AF7,
	PINMUX_HAL_AF8,
	PINMUX_HAL_AF9,
	PINMUX_HAL_AF10,
	PINMUX_HAL_AF11,
	PINMUX_HAL_AF12,
	PINMUX_HAL_AF13,
	PINMUX_HAL_AF14,
	PINMUX_HAL_AF15
};

/** @brief Drive strength configuration. */
enum pinmux_hal_drive {
	PINMUX_HAL_DRIVE_DEFAULT = 0, /**< Use platform default drive. */
	PINMUX_HAL_DRIVE_LOW,         /**< Reduced output current. */
	PINMUX_HAL_DRIVE_MEDIUM,      /**< Nominal output current. */
	PINMUX_HAL_DRIVE_HIGH         /**< Maximum output current. */
};

/** @brief Slew rate configuration. */
enum pinmux_hal_slew {
	PINMUX_HAL_SLEW_DEFAULT = 0, /**< Use platform default slew rate. */
	PINMUX_HAL_SLEW_SLOW,        /**< Reduced transition speed. */
	PINMUX_HAL_SLEW_FAST         /**< Fastest possible transition. */
};

/**
 * @brief Assign an alternate function to pin.
 *
 * @param ctrl      Pin controller instance handle.
 * @param pin_mask  Bitmask of pin to configure.
 * @param func      Desired alternate function (AF0-AF15 or GPIO).
 */
void pinmux_hal_set_func(void *ctrl, uint32_t pin_mask, enum pinmux_hal_func func);

/**
 * @brief Read back current alternate function.
 *
 * @param ctrl      Pin controller instance handle.
 * @param pin_mask  Bitmask of pin to configure.
 * @return Currently active function (AFx or GPIO).
 */
enum pinmux_hal_func pinmux_hal_get_func(void *ctrl, uint32_t pin_mask);

/**
 * @brief Configure output drive strength.
 *
 * @param ctrl      Pin controller instance handle.
 * @param pin_mask  Bitmask of pin to configure.
 * @param drive     Drive strength setting.
 */
void pinmux_hal_set_drive(void *ctrl, uint32_t pin_mask, enum pinmux_hal_drive drive);

/**
 * @brief Configure slew rate.
 *
 * @param ctrl      Pin controller instance handle.
 * @param pin_mask  Bitmask of pin to configure.
 * @param slew      Slew rate setting.
 */
void pinmux_hal_set_slew(void *ctrl, uint32_t pin_mask, enum pinmux_hal_slew slew);

#endif
