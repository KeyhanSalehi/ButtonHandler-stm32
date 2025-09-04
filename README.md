# Button Handler Library

![Button Handler](https://img.shields.io/badge/License-MIT-blue.svg)  
![Platform](https://img.shields.io/badge/Platform-STM32-green.svg)  
![Language](https://img.shields.io/badge/Language-C-orange.svg)

## Overview

The Button Handler library is a lightweight C library designed for handling button inputs on embedded systems (e.g., STM32 microcontrollers). It provides debouncing, press counting, and release detection with timeout functionality using a software timer. This helps in reliably reading button presses, avoiding noise from mechanical switches, and counting multiple presses within a time window.

The library uses HAL_GPIO functions (from STM32 HAL) for GPIO operations and assumes a `softTimer` library for timing. It supports configurable pull-up/down modes and limits the maximum press count to prevent overflows.

### Key Features
- **Debouncing**: Filters out noise from button bounces with a configurable delay (default: 50ms).
- **Press Counting**: Counts consecutive button presses within a timeout period (default: 1 second).
- **Timeout Detection**: Automatically finalizes the count after a timeout if no more presses occur.
- **Pull Configuration**: Supports GPIO_PULLUP, GPIO_PULLDOWN, or GPIO_NOPULL.
- **State Management**: Tracks button state to detect transitions accurately.
- **Error Handling**: Returns status codes for success, failure, or busy states.
- **Lightweight**: Minimal dependencies, suitable for resource-constrained embedded systems.

## Dependencies
- STM32 HAL Library (for GPIO operations like `HAL_GPIO_ReadPin`).
- `softTimer.h` library (for software timers; must be implemented separately or included from your project).
- `Common.h` (assumed to include standard types like `return_t`, `bool`, etc.; replace with your project's common headers if needed).

**Note**: This library does not handle GPIO initialization. You must configure the GPIO pin as input (with appropriate pull) in your MCU's configuration tool (e.g., STM32CubeMX).

## Installation
1. Clone or download the repository.
2. Copy `button_handler.h` and `button_handler.c` into your project directory.
3. Include `button_handler.h` in your source files where needed.
4. Ensure `softTimer.h` and any other dependencies are available in your include path.
5. Add the files to your build system (e.g., Makefile, IDE project).

## Usage
### Basic Workflow
1. **Initialize the Button**: Call `Button_Init` to set up the button structure with GPIO port, pin, and pull configuration.
2. **Poll the Button**: In your main loop or timer interrupt, call `Button_GetFinalCount` to check for completed reads.
3. **Handle Results**: When a count is ready (after timeout), process the final press count.

The library uses a polling approach in `Button_GetFinalCount`, which internally calls `Button_CountPushes` to handle state transitions and timers.

### Configuration Options
- **Debounce Delay**: Defined as `DEBOUNCE_DELAY_MS` (default: 50ms). This is the minimum time between valid presses to ignore bounces. Adjust in `button_handler.c` if needed.
- **Timeout Delay**: Defined as `BUTTON_TIMEOUT_MS` (default: 1000ms or 1 second). This is the idle time after the last press before finalizing the count. Adjust in `button_handler.c`.
- **Max Push Count**: Defined as `PUSH_COUNT_MAX` (default: 5). Prevents counting beyond this limit; resets to 0 on overflow.
- **Pull Mode**: Specified in `Button_Init` (GPIO_PULLUP, GPIO_PULLDOWN, or GPIO_NOPULL). Determines active/low logic:
  - GPIO_PULLUP: Button press pulls to LOW (active low).
  - GPIO_PULLDOWN: Button press pulls to HIGH (active high).
- **Return Types** (from `return_t`, assumed in `Common.h`):
  - `return_success`: Count is valid and ready.
  - `return_failed`: Invalid count (e.g., overflow or zero presses).
  - `return_busy`: Still waiting for timeout or more presses.

**Note**: Timers must be updated externally. Call `softTimer_update()` (or equivalent) in your system's tick handler (e.g., SysTick) to keep timers running.

## API Reference

### Structures
```c
typedef struct {
    GPIO_TypeDef *port;      // GPIO port (e.g., GPIOA)
    uint16_t pin;            // GPIO pin number (e.g., GPIO_PIN_0)
    volatile uint8_t pushCount; // Number of button presses
    volatile bool isReadFinish; // Flag indicating read completion
    softTimer_t debounceTimer;  // Software timer for debouncing
    softTimer_t timeoutTimer;   // Software timer for timeout
    GPIO_PinState lastState;    // Last recorded button state
    uint32_t pull;              // Pull configuration (GPIO_PULLUP, GPIO_PULLDOWN, or GPIO_NOPULL)
} Button_t;
```

### Functions

#### `void Button_Init(Button_t *button, GPIO_TypeDef *port, uint16_t pin, uint32_t pull)`
- **Description**: Initializes the button structure.
- **Parameters**:
  - `button`: Pointer to a `Button_t` instance.
  - `port`: GPIO port (e.g., GPIOA).
  - `pin`: GPIO pin (e.g., GPIO_PIN_0).
  - `pull`: Pull mode (GPIO_PULLUP, GPIO_PULLDOWN, or GPIO_NOPULL).
- **Returns**: Void.
- **Notes**: Resets counters and timers. GPIO must be pre-configured as input.

#### `return_t Button_GetFinalCount(Button_t *button, uint8_t *finalCount)`
- **Description**: Retrieves the final press count after timeout. Internally handles counting and validation.
- **Parameters**:
  - `button`: Pointer to a `Button_t` instance.
  - `finalCount`: Pointer to store the final count (1 to PUSH_COUNT_MAX).
- **Returns**:
  - `return_success`: Valid count stored in `finalCount`.
  - `return_failed`: Invalid (e.g., 0 or > max).
  - `return_busy`: Still counting (call again later).
- **Notes**: Call this in a loop. Resets internal state on success/failure.

## Example Usage
Below is a complete example assuming an STM32 project with SysTick for timer updates. Replace placeholders with your actual setup.

### Example Code
```c
// main.c
#include "button_handler.h"
#include "softTimer.h"  // Your software timer library
#include "Common.h"     // For return_t, bool, etc.
#include <stdio.h>      // For printf (debug)

// Global button instance
Button_t myButton;

// In your SysTick_Handler or timer update function
void SysTick_Handler(void) {
    HAL_IncTick();  // STM32 HAL tick
    softTimer_update();  // Update all software timers
}

int main(void) {
    // HAL and system init (from STM32CubeMX)
    HAL_Init();
    // ... GPIO init: Configure button pin as input with pull-up

    // Initialize button on GPIOA Pin 0 with pull-up
    Button_Init(&myButton, GPIOA, GPIO_PIN_0, GPIO_PULLUP);

    uint8_t pressCount = 0;
    return_t status;

    while (1) {
        // Poll for final count
        status = Button_GetFinalCount(&myButton, &pressCount);

        if (status == return_success) {
            printf("Button pressed %d times!\n", pressCount);
            // Handle logic based on count (e.g., 1: single click, 2: double click)
        } else if (status == return_failed) {
            printf("Invalid button count.\n");
        }
        // Busy: continue looping

        // Other main loop code...
        HAL_Delay(10);  // Small delay to avoid tight loop (optional)
    }
}
```

### Explanation of Example
- **Initialization**: Sets up the button on a specific pin with pull-up. Active state is LOW (button grounds the pin).
- **Polling**: `Button_GetFinalCount` is called repeatedly. It debounces presses, counts them, and waits for 1 second of inactivity.
- **Handling Results**:
  - On success: `pressCount` holds the number (e.g., 3 for triple press).
  - Use cases: Single press (1) for toggle, double press (2) for mode change, etc.
- **Options in Action**:
  - If using GPIO_PULLDOWN, change to `GPIO_PULLDOWN` in init; active state becomes HIGH.
  - Adjust delays in defines for faster/slower response (e.g., shorter debounce for responsive UI).

## Troubleshooting
- **No Counts Detected**: Ensure GPIO is configured correctly and button wiring matches pull mode.
- **Timers Not Working**: Verify `softTimer_update()` is called regularly (e.g., every 1ms).
- **Overflow**: If presses exceed `PUSH_COUNT_MAX`, it returns failed and resets.
- **Debounce Issues**: If buttons are very noisy, increase `DEBOUNCE_DELAY_MS`.

## Contributing
Contributions are welcome! Fork the repo, make changes, and submit a pull request. Please fix any typos (e.g., "debounCing" to "debouncing") in code/comments.

## License
MIT License. See [LICENSE](LICENSE) for details.

Copyright (c) [KeyhanSalehi]
