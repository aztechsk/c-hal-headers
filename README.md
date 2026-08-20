# Hardware Abstraction Layer

This project provides small, platform-independent Hardware Abstraction Layer (HAL) interfaces for common microcontroller peripherals.

The HAL separates higher-level embedded software from MCU-specific peripheral implementations and provides a consistent API for GPIO, pin multiplexing, and SPI master operation.

## Interfaces

### `gpio_hal.h`

Platform-independent GPIO interface.

It provides:

- GPIO direction and logical level control,
- internal pull-up and pull-down configuration,
- push-pull and open-drain output modes,
- Schmitt trigger and input filter configuration,
- GPIO interrupt configuration and callback registration,
- debounce timing configuration,
- access to platform-specific GPIO controller handles.

GPIO operations use a controller handle and a single-pin bit mask. Interrupt callbacks are executed directly from the low-level interrupt context.

### `pinmux_hal.h`

Platform-independent pin multiplexing interface.

It allows a physical pin to be assigned to GPIO or to one of the alternate peripheral functions supported by the platform. The interface also provides configuration of drive strength and slew rate.

Like the GPIO HAL, the API operates on a controller handle and a single-pin bit mask.

### `spi_hal.h`

Platform-independent SPI master interface.

An SPI device is described by `struct spi_hal_dev` and its configuration, including:

- SPI bus instance,
- clock polarity and phase mode,
- transfer width from 8 to 16 bits,
- serial clock frequency,
- chip-select configuration,
- transfer timing parameters.

Transfers are synchronous, full-duplex, and in-place. Polling, interrupt-driven, and DMA transfer modes are supported. A transfer may contain one or two buffer segments.

## Platform Backends

The interfaces are intended to be implemented by platform-specific low-level drivers. Application code and peripheral drivers can therefore use the same HAL API without depending directly on a particular microcontroller peripheral implementation.

## License

The source files are distributed under a permissive ISC-style license. See the individual source files for the full license text.