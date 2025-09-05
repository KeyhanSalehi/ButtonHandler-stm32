# Button Handler Library

![Button Handler](https://img.shields.io/badge/License-MIT-blue.svg)  
![Platform](https://img.shields.io/badge/Platform-STM32-green.svg)  
![Language](https://img.shields.io/badge/Language-C-orange.svg)

## Overview

The Button Handler library is a lightweight C library designed for handling button inputs on embedded systems, specifically STM32 microcontrollers. It provides debouncing, press counting, and timeout detection using software timers, enabling reliable detection of single, double, or multiple button presses within a specified time window. The library supports configurable pull-up/down modes and limits the maximum press count to prevent overflows.

### Key Features
- **Debouncing**: Filters out noise from button bounces with a configurable delay (default: 50ms).
- **Press Counting**: Counts consecutive button presses within a timeout period (default: 1 second).
- **Timeout Detection**: Finalizes the count after a timeout if no more presses occur.
- **Pull Configuration**: Supports `GPIO_PULLUP`, `GPIO_PULLDOWN`, or `GPIO_NOPULL`.
- **State Management**: Tracks button state to detect transitions accurately.
- **Error Handling**: Returns status codes (`return_success`, `return_failed`, `return_busy`).
- **Lightweight**: Minimal dependencies, ideal for resource-constrained systems.

## Dependencies
- **STM32 HAL Library**: Required for GPIO operations (e.g., `HAL_GPIO_ReadPin`).
- **`softTimer.h`**: Software timer library for debouncing and timeout (must be implemented or included in your project).
- **`Common.h`**: Assumed to define standard types like `return_t`, `bool`, etc. Replace with your project's common headers if needed.

**Note**: This library does not handle GPIO initialization. Configure GPIO pins as inputs with appropriate pull settings using your MCU configuration tool (e.g., STM32CubeMX).

## Installation
1. Clone or download the repository.
2. Place the following files in your project:
   - Copy `button_handler.h` to your `Inc` (include) directory.
   - Copy `button_handler.c` to your `Src` (source) directory.
3. Include `button_handler.h` in your source files where needed.
4. Ensure `softTimer.h` and other dependencies are in your include path.
5. Add `button_handler.c` and `button_handler.h` to your build system (e.g., Makefile, STM32CubeIDE project).

## Usage
### Basic Workflow
1. **Initialize the Button**: Call `Button_Init` to set up a button with GPIO port, pin, and pull configuration.
2. **Poll the Button**: In your main loop or timer interrupt, call `Button_GetFinalCount` to check for completed reads.
3. **Handle Results**: Process the final press count after the timeout period.

The library uses a polling approach in `Button_GetFinalCount`, which internally calls `Button_CountPushes` to handle state transitions and timers.

### Configuration Options
- **Debounce Delay** (`DEBOUNCE_DELAY_MS`): Default 50ms. Adjust in `button_handler.c` to change the minimum time between valid presses.
- **Timeout Delay** (`BUTTON_TIMEOUT_MS`): Default 1000ms (1 second). Adjust in `button_handler.c` to change the idle time before finalizing the count.
- **Max Push Count** (`PUSH_COUNT_MAX`): Default 5. Adjust in `button_handler.h` to allow more presses (see below for details).
- **Pull Mode**: Set in `Button_Init` (`GPIO_PULLUP`, `GPIO_PULLDOWN`, or `GPIO_NOPULL`):
  - `GPIO_PULLUP`: Button press is LOW (active low).
  - `GPIO_PULLDOWN`: Button press is HIGH (active high).
- **Return Types** (from `return_t` in `Common.h`):
  - `return_success`: Valid count ready.
  - `return_failed`: Invalid count (e.g., 0 or > `PUSH_COUNT_MAX`).
  - `return_busy`: Still counting presses.

**Note**: Call `softTimer_update()` regularly (e.g., every 1ms in `SysTick_Handler`) to update timers.

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
  - `port`: GPIO port (e.g., `GPIOA`).
  - `pin`: GPIO pin (e.g., `GPIO_PIN_0`).
  - `pull`: Pull mode (`GPIO_PULLUP`, `GPIO_PULLDOWN`, or `GPIO_NOPULL`).
- **Returns**: Void.
- **Notes**: Resets counters and timers. GPIO must be pre-configured as input.

#### `return_t Button_GetFinalCount(Button_t *button, uint8_t *finalCount)`
- **Description**: Retrieves the final press count after timeout.
- **Parameters**:
  - `button`: Pointer to a `Button_t` instance.
  - `finalCount`: Pointer to store the final count (1 to `PUSH_COUNT_MAX`).
- **Returns**:
  - `return_success`: Valid count stored in `finalCount`.
  - `return_failed`: Invalid count (e.g., 0 or > max).
  - `return_busy`: Still counting.
- **Notes**: Call in a loop. Resets state on success/failure.

## Example Usage: Counting Button Presses
This example shows how to count button presses and perform actions based on the count (e.g., single press, double press). It handles two buttons with different pull configurations.

### Example Code
```c
// Src/main.c
#include "button_handler.h"
#include "softTimer.h"  // Your software timer library
#include "Common.h"     // For return_t, bool, etc.
#include <stdio.h>      // For printf (debug)

// Global button instances
Button_t key1;  // Button with pull-up (active low)
Button_t key2;  // Button with pull-down (active high)

// In your SysTick_Handler or timer update function
void SysTick_Handler(void) {
    HAL_IncTick();       // STM32 HAL tick
    softTimer_update();  // Update all software timers
}

// Handle actions based on press count
void HandleButtonPress(uint8_t keyVal, const char* buttonName) {
    switch (keyVal) {
        case 1:
            printf("%s: Single press detected - Toggle LED\n", buttonName);
            // Example: HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
            break;
        case 2:
            printf("%s: Double press detected - Change mode\n", buttonName);
            // Example: Change system mode
            break;
        case 3:
            printf("%s: Triple press detected - Reset system\n", buttonName);
            // Example: NVIC_SystemReset();
            break;
        case 4:
            printf("%s: Quad press detected - Enter config mode\n", buttonName);
            // Example: Enter configuration mode
            break;
        default:
            printf("%s: Unsupported press count (%d)\n", buttonName, keyVal);
            break;
    }
}

int main(void) {
    // HAL and system init (from STM32CubeMX)
    HAL_Init();
    // ... GPIO init: Configure button pins as input
    // Example: key1 on GPIOA Pin 0 (pull-up), key2 on GPIOB Pin 1 (pull-down)

    // Initialize buttons
    Button_Init(&key1, GPIOA, GPIO_PIN_0, GPIO_PULLUP);   // Active low
    Button_Init(&key2, GPIOB, GPIO_PIN_1, GPIO_PULLDOWN); // Active high

    uint8_t key1Val = 0;
    uint8_t key2Val = 0;
    return_t status;

    while (1) {
        // Poll for key1
        status = Button_GetFinalCount(&key1, &key1Val);
        if (status == return_success) {
            HandleButtonPress(key1Val, "Key1");
            key1Val = 0;  // Reset count for next read
        } else if (status == return_failed) {
            printf("Key1: Invalid button count\n");
        }

        // Poll for key2
        status = Button_GetFinalCount(&key2, &key2Val);
        if (status == return_success) {
            HandleButtonPress(key2Val, "Key2");
            key2Val = 0;  // Reset count for next read
        } else if (status == return_failed) {
            printf("Key2: Invalid button count\n");
        }

        // Other main loop code...
        HAL_Delay(10);  // Small delay to avoid tight loop (optional)
    }
}
```

### Explanation of Example
- **Multiple Buttons**: Initializes two buttons (`key1`: pull-up, `key2`: pull-down) to demonstrate handling different pull configurations.
- **Action Handler**: The `HandleButtonPress` function maps press counts to actions (e.g., 1 press: toggle LED, 2 presses: change mode).
- **Reset Count**: Resets `keyVal` to 0 after a successful read, preparing for the next sequence.
- **Pull Modes**: `key1` detects LOW on press (pull-up), `key2` detects HIGH (pull-down).
- **Polling**: Both buttons are polled in the main loop, with debouncing and timeout handled internally.

## Adjusting `PUSH_COUNT_MAX`
To support more presses (e.g., up to 10), modify `PUSH_COUNT_MAX` in `Inc/button_handler.h`:

1. **Edit `button_handler.h`**:
   Change:
   ```c
   #define PUSH_COUNT_MAX 5
   ```
   to:
   ```c
   #define PUSH_COUNT_MAX 10
   ```

2. **Update Application Code**:
   Extend `HandleButtonPress` to handle additional counts:
   ```c
   void HandleButtonPress(uint8_t keyVal, const char* buttonName) {
       switch (keyVal) {
           case 1:
               printf("%s: Single press - Toggle LED\n", buttonName);
               break;
           case 2:
               printf("%s: Double press - Change mode\n", buttonName);
               break;
           case 3:
               printf("%s: Triple press - Reset system\n", buttonName);
               break;
           case 4:
               printf("%s: Quad press - Enter config mode\n", buttonName);
               break;
           case 5:
               printf("%s: Five presses - Custom action\n", buttonName);
               break;
           // Add more cases up to 10
           case 10:
               printf("%s: Ten presses - Special action\n", buttonName);
               break;
           default:
               printf("%s: Unsupported press count (%d)\n", buttonName, keyVal);
               break;
       }
   }
   ```

3. **Consider Type Limits**:
   - `pushCount` is a `uint8_t` (max 255). For `PUSH_COUNT_MAX` > 255, change to `uint16_t` in `button_handler.h`:
     ```c
     volatile uint16_t pushCount;
     ```
   - Update overflow checks in `button_handler.c` accordingly.

4. **Adjust Timeout**:
   - For higher counts, consider increasing `BUTTON_TIMEOUT_MS` in `button_handler.c` (e.g., to 2000ms) to give users enough time to press multiple times.

5. **Test**:
   - Verify debouncing (`DEBOUNCE_DELAY_MS`) suits the new count range.
   - Test with your hardware to ensure reliable counting.

### Implications
- **Pros**: Supports complex input patterns (e.g., 10 presses for a special command).
- **Cons**: Longer timeouts may increase latency; high counts may be hard for users to perform.
- **Recommendation**: Keep `PUSH_COUNT_MAX` ≤ 10 for usability. For very high counts, consider alternative inputs (e.g., long press).

## Troubleshooting
- **No Counts Detected**: Check GPIO configuration and wiring (pull-up: button to GND; pull-down: button to VCC).
- **Timers Not Working**: Ensure `softTimer_update()` is called regularly (e.g., every 1ms).
- **Overflow**: Counts exceeding `PUSH_COUNT_MAX` return `return_failed` and reset.
- **Debounce Issues**: Increase `DEBOUNCE_DELAY_MS` if buttons are noisy.

## Contributing
Contributions are welcome! Fork the repo, make changes, and submit a pull request. Please fix typos (e.g., "debounCing" to "debouncing") in code/comments.

## License
MIT License. See [LICENSE](LICENSE) for details.

Copyright (c) [KeyhanSalehi]
