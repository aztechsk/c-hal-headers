/*
 * spi_hal.h
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

#ifndef SPI_HAL_H
#define SPI_HAL_H

#include <stdbool.h>

/**
 * @file spi_hal.h
 * @brief SPI Hardware (Master Mode) Abstraction Layer (HAL).
 *
 * This HAL provides a minimal, hardware-independent SPI API
 * on top of a platform-specific low-level driver.
 *
 * - Transfers are full-duplex and in-place: the contents of TX buffers
 *   are overwritten with RX data after completion.
 * - Up to two buffer segments (buf0, buf1) can be passed in one transfer.
 * - Chip select behavior between segments is defined by @ref spi_hal_dev_cfg::cs_rise.
 * - Calls are synchronous/blocking: the function returns once the transfer has finished
 *   (polling, interrupt, or DMA based on @ref spi_hal_xfer_type).
 *
 * @note Functions may return:
 *  - `-EHW`  - hardware error.
 *  - `-EDMA` - DMA transfer error.
 */

/** @brief Transfer handling method. */
enum spi_hal_xfer_type {
	/** Polling mode (busy-wait in software). */
	SPI_HAL_POLL,
	/** Interrupt mode (non-DMA, ISR-driven, blocking until complete). */
	SPI_HAL_INTR,
	/** DMA mode (blocking until DMA transfer completion). */
	SPI_HAL_DMA
};

/** @brief SPI clock polarity/phase mode. */
enum spi_hal_mode {
	SPI_HAL_MODE0, /**< CPOL=0, CPHA=0 */
	SPI_HAL_MODE1, /**< CPOL=0, CPHA=1 */
	SPI_HAL_MODE2, /**< CPOL=1, CPHA=0 */
	SPI_HAL_MODE3  /**< CPOL=1, CPHA=1 */
};

/** @brief SPI chip-select line identifier. */
enum spi_hal_csel_num {
	SPI_HAL_CSEL0,
        SPI_HAL_CSEL1,
        SPI_HAL_CSEL2,
        SPI_HAL_CSEL3
};

/**
 * @brief Bits per transfer unit.
 *
 * - 8 bits: each unit is 1 byte.
 * - 9-16 bits: each unit is 1 uint16_t word.
 *
 * @note The @c size0 and @c size1 parameters in @ref spi_hal_xfer
 *       specify the number of transfer units, not raw bytes.
 */
enum spi_hal_bits_trans {
	SPI_HAL_8_BIT_TRANS,
	SPI_HAL_9_BIT_TRANS,
        SPI_HAL_10_BIT_TRANS,
	SPI_HAL_11_BIT_TRANS,
	SPI_HAL_12_BIT_TRANS,
	SPI_HAL_13_BIT_TRANS,
	SPI_HAL_14_BIT_TRANS,
	SPI_HAL_15_BIT_TRANS,
	SPI_HAL_16_BIT_TRANS
};

/**
 * @brief SPI device configuration descriptor.
 *
 * This structure defines all parameters required to configure
 * a device on a given SPI bus.
 */
struct spi_hal_dev_cfg {
	int spi_bus_id;		/**< SPI bus instance ID. */
	enum spi_hal_mode mode;	/**< SPI mode (clock polarity/phase). */
	enum spi_hal_bits_trans bits_trans; /**< Bits per transfer unit. */
	int dly_bct_ns;	/**< Delay between consecutive transfers (ns). */
	int dly_bcs_ns;	/**< Delay before serial clock after CS assert (ns). */
	bool cs_rise;	/**< If true, CS is deasserted after each transfer unit. */
        int sck_hz;	/**< Target serial clock frequency (Hz). */
	enum spi_hal_csel_num csel_num;	/**< Chip-select line identifier. */
	void *csel_cont; 		/**< CSEL pin - Pointer to PIO controller. */
	unsigned int csel_pin;		/**< CSEL pin - Bitmask of pin. */
};

/**
 * @brief SPI device handle.
 *
 * The handle stores configuration and an opaque pointer to the
 * backend implementation.
 */
struct spi_hal_dev {
	struct spi_hal_dev_cfg cfg;	/**< Active configuration. */
	void *opaque;			/**< Backend private data. */
};

/**
 * @brief Initialize an SPI HAL device handle.
 *
 * Prepares the device descriptor for use. Backend-specific fields
 * are allocated/initialized as needed.
 *
 * @param dev Pointer to device handle.
 */
void spi_hal_dev_init(struct spi_hal_dev *dev);

/**
 * @brief Reconfigure an SPI HAL device.
 *
 * Applies changes in configuration from @ref spi_hal_dev::cfg
 * to the underlying hardware instance.
 *
 * @param dev Pointer to device handle.
 */
void spi_hal_dev_cfg(struct spi_hal_dev *dev);

/**
 * @brief Perform a blocking SPI transfer.
 *
 * Full-duplex, in-place transfer. Each buffer contains TX data before
 * the call and is overwritten with RX data after completion.
 *
 * Rules:
 * - @p size0 must always be non-zero. The first buffer is mandatory.
 * - The second buffer is optional:
 *   - If @p size1 is zero, @p buf1 is ignored and only the first segment is transferred.
 * - Unit counts (@p size0/@p size1) are specified in transfer units
 *   (byte for 8-bit mode; uint16_t word for 9-16 bit modes).
 *
 * @param dev        Pointer to device handle.
 * @param xfer_type  Transfer method (polling, interrupt, DMA).
 * @param buf0       First buffer (in/out), mandatory.
 * @param size0      Number of transfer units in @p buf0 (must be > 0).
 * @param buf1       Second buffer (in/out), optional.
 * @param size1      Number of transfer units in @p buf1, or 0 if unused.
 *
 * @return 0 on success,
 *         negative error code on failure (-EHW - hardware error; -EDMA - DMA error).
 */
int spi_hal_xfer(struct spi_hal_dev *dev, enum spi_hal_xfer_type xfer_type, void *buf0, int size0, void *buf1, int size1);

#endif
