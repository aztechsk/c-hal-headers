/*
 * gpio_hal.h
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

#ifndef GPIO_HAL_H
#define GPIO_HAL_H

#include <FreeRTOS.h>
#include <gentyp.h>
#include <stdint.h>

/**
 * @file gpio_hal.h
 * @brief Platform-independent GPIO Hardware Abstraction Layer.
 *
 * This HAL provides uniform GPIO configuration and control across
 * different microcontroller architectures.
 *
 * - Each controller manages up to 32 pins (bitmask representation).
 * - The @p ctrl parameter is a platform-specific pointer to the
 *   GPIO/PIO controller instance (opaque to upper layers).
 * - Parameter validation errors are fatal and terminate via
 *   @c crit_err_exit().
 */

/**
 * Note:
 * This API is designed for single-pin operations.
 * Each function expects exactly one bit set in @p pin_mask.
 * Passing multi-bit masks results in a fatal parameter error.
 */

/** @brief Logical level on a GPIO pin. */
enum gpio_hal_level {
	GPIO_HAL_LOW = 0,
	GPIO_HAL_HIGH = 1
};

/** @brief GPIO pin direction. */
enum gpio_hal_dir {
	GPIO_HAL_DIR_INPUT,
	GPIO_HAL_DIR_OUTPUT
};

/** @brief Internal pull resistor configuration. */
enum gpio_hal_pull {
	GPIO_HAL_PULL_NONE,   /**< No pull resistor. */
	GPIO_HAL_PULL_UP,     /**< Enable pull-up resistor. */
	GPIO_HAL_PULL_DOWN    /**< Enable pull-down resistor. */
};

/** @brief Output driver type. */
enum gpio_hal_drive {
	GPIO_HAL_DRIVE_PUSH_PULL, /**< Standard push-pull driver. */
	GPIO_HAL_DRIVE_OPEN_DRAIN /**< Open-drain / open-collector mode. */
};

/** @brief Input filtering mode. */
enum gpio_hal_filter {
	GPIO_HAL_FILTER_NONE,     /**< No input filter. */
	GPIO_HAL_FILTER_GLITCH,   /**< Fast digital glitch filter. */
	GPIO_HAL_FILTER_DEBOUNCE  /**< Slow clock-based debounce filter. */
};

/** @brief GPIO interrupt trigger type. */
enum gpio_hal_intr {
	GPIO_HAL_INTR_DISABLED,    /**< Interrupt disabled. */
	GPIO_HAL_INTR_RISING,      /**< Rising edge trigger. */
	GPIO_HAL_INTR_FALLING,     /**< Falling edge trigger. */
	GPIO_HAL_INTR_BOTH,        /**< Both edges trigger. */
	GPIO_HAL_INTR_LEVEL_HIGH,  /**< High level trigger. */
	GPIO_HAL_INTR_LEVEL_LOW    /**< Low level trigger. */
};

/**
 * @brief ISR callback function type.
 *
 * Called from the GPIO interrupt handler when one or more pins
 * of a controller generate an interrupt.
 *
 * @param status_mask  Raw interrupt status mask for that controller.
 *
 * @return pdTRUE if a higher-priority task should be woken.
 */
typedef BaseType_t (*gpio_hal_isr_clbk_t)(uint32_t status_mask);

/* -------------------------------------------------------------------------- */
/** @name Configuration                                                       */
/* -------------------------------------------------------------------------- */

/**
 * @brief Set GPIO pin direction.
 *
 * @param ctrl      Controller instance handle.
 * @param pin_mask  Bitmask of pin to configure (1U << pin_index).
 * @param dir       Desired direction (input/output).
 */
void gpio_hal_set_dir(void *ctrl, uint32_t pin_mask, enum gpio_hal_dir dir);

/**
 * @brief Configure internal pull resistor.
 *
 * @param ctrl      Controller instance handle.
 * @param pin_mask  Bitmask of pin to configure.
 * @param pull      Pull resistor mode.
 */
void gpio_hal_set_pull(void *ctrl, uint32_t pin_mask, enum gpio_hal_pull pull);

/**
 * @brief Configure output drive type.
 *
 * @param ctrl      Controller instance handle.
 * @param pin_mask  Bitmask of pin to configure.
 * @param drive     Output driver mode.
 */
void gpio_hal_set_drive(void *ctrl, uint32_t pin_mask, enum gpio_hal_drive drive);

/**
 * @brief Enable or disable Schmitt trigger on input.
 *
 * @param ctrl      Controller instance handle.
 * @param pin_mask  Bitmask of pin.
 * @param enable    @c true to enable, @c false to disable.
 */
void gpio_hal_set_schmitt(void *ctrl, uint32_t pin_mask, boolean_t enable);

/**
 * @brief Configure input filtering mode.
 *
 * @param ctrl      Controller instance handle.
 * @param pin_mask  Bitmask of pin.
 * @param filter    Desired filtering mode.
 */
void gpio_hal_set_filter(void *ctrl, uint32_t pin_mask, enum gpio_hal_filter filter);

/* -------------------------------------------------------------------------- */
/** @name Basic I/O Operations                                                */
/* -------------------------------------------------------------------------- */

/**
 * @brief Write logical level to output pin(s).
 *
 * @param ctrl      Controller instance handle.
 * @param pin_mask  Bitmask of pin.
 * @param level     Logical level to drive.
 */
void gpio_hal_set_level(void *ctrl, uint32_t pin_mask, enum gpio_hal_level level);

/**
 * @brief Read current input level of pin(s).
 *
 * @param ctrl      Controller instance handle.
 * @param pin_mask  Bitmask of pin.
 * @return GPIO_HAL_HIGH or GPIO_HAL_LOW.
 */
enum gpio_hal_level gpio_hal_get_input(void *ctrl, uint32_t pin_mask);

/**
 * @brief Read output latch state (if supported by hardware).
 *
 * @param ctrl      Controller instance handle.
 * @param pin_mask  Bitmask of pin.
 * @return GPIO_HAL_HIGH or GPIO_HAL_LOW.
 */
enum gpio_hal_level gpio_hal_get_output(void *ctrl, uint32_t pin_mask);

/* -------------------------------------------------------------------------- */
/** @name Interrupt Control                                                   */
/* -------------------------------------------------------------------------- */

/**
 * @brief Configure interrupt trigger type for pin(s).
 *
 * @param ctrl      Controller instance handle.
 * @param pin_mask  Bitmask of pin.
 * @param trig      Interrupt trigger type.
 */
void gpio_hal_intr_config(void *ctrl, uint32_t pin_mask, enum gpio_hal_intr trig);

/** @brief Enable interrupt for pin. */
void gpio_hal_intr_enable(void *ctrl, uint32_t pin_mask);

/** @brief Is interrupt enabled for pin. */
boolean_t gpio_hal_is_intr_enabled(void *ctrl, uint32_t pin_mask);

/** @brief Disable interrupt for pin. */
void gpio_hal_intr_disable(void *ctrl, uint32_t pin_mask);

/** @brief Clear pending interrupt. */
void gpio_hal_intr_clear(void *ctrl);

/**
 * @brief Register ISR callback for controller.
 *
 * @attention
 *   The registered callback is invoked **directly** from the
 *   low-level interrupt context. It must be short, non-blocking,
 *   and safe for ISR execution.
 *
 * @param ctrl  Controller instance handle.
 * @param cb    Pointer to callback function.
 */
void gpio_hal_isr_register(void *ctrl, gpio_hal_isr_clbk_t cb);

/**
 * @brief Test whether a callback is registered.
 *
 * @param ctrl  Controller instance handle.
 * @param cb    Callback pointer to test.
 * @return true if registered; false otherwise.
 */
boolean_t gpio_hal_isr_registered(void *ctrl, gpio_hal_isr_clbk_t cb);

/* -------------------------------------------------------------------------- */
/** @name Debounce Filter Timing                                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief Set debounce clock period in microseconds.
 *
 * @param ctrl Controller instance handle.
 * @param us   Debounce time.
 */
void gpio_hal_debounce_set_us(void *ctrl, int us);

/**
 * @brief Set debounce clock period in milliseconds.
 *
 * @param ctrl Controller instance handle.
 * @param ms   Debounce time.
 */
void gpio_hal_debounce_set_ms(void *ctrl, int ms);

/* -------------------------------------------------------------------------- */
/** @name Handle to GPIO controller                                           */
/* -------------------------------------------------------------------------- */

/**
 * @brief Obtain handle to GPIO controller by index.
 *
 * Returns an opaque pointer to the underlying GPIO/PIO controller
 * instance identified by @p ctrl_id. The returned handle is used
 * as the @p ctrl parameter in all gpio_hal_* functions.
 *
 * @param ctrl_id  Zero-based index of the GPIO controller (0..N-1).
 *
 * @return Pointer to controller instance.
 *
 * @note
 *   Invalid or out-of-range @p ctrl_id values result in a fatal
 *   parameter error and system termination via crit_err_exit().
 */
void *gpio_hal_get_ctrl(int ctrl_id);

#endif
