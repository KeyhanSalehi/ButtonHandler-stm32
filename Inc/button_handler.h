/*!
 *******************************************************************************
 * @file           : button_handler.h
 * @author         : KeyhanSalehi
 * @brief          : Header file for the Button Handler library.
 *******************************************************************************
 * @attention
 *
 * Provides functions to handle button inputs with debounCing,
 * counting, and release detection using a software timer.
 *
 *******************************************************************************
 */

#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

/* Includes ------------------------------------------------------------------*/

/* 1. System Header Files */
#include <Common.h>
/* 2. Project Header Files */
#include "softTimer.h"

/* Defines & Macros ----------------------------------------------------------*/

/*! @def @brief maximum push count. */
#define PUSH_COUNT_MAX 5

/* Typedefs ------------------------------------------------------------------*/

/*!
 * @struct
 * @brief Structure to hold button configuration and state.
 */
typedef struct {
	GPIO_TypeDef *port; /**< GPIO port (e.g., GPIOA). */
	uint16_t pin; /**< GPIO pin number (e.g., GPIO_PIN_0). */
	volatile uint8_t pushCount; /**< Number of button pushes. */
	volatile bool isReadFinish; /**< Flag indicating read completion. */
	softTimer_t debounceTimer; /**< Software timer for debounCing. */
	softTimer_t timeoutTimer; /**< Software timer for timeout. */
	GPIO_PinState lastState; /**< Last recorded button state. */
	uint32_t pull; /**< Pull configuration (GPIO_PULLUP, GPIO_PULLDOWN, or GPIO_NOPULL). */
} Button_t;

/* Exported Variables --------------------------------------------------------*/

/* 1. Global Variables */

/* Exported Functions --------------------------------------------------------*/

/* 1. Global Function Declarations */

/*!
 * @fn     void Button_Init(Button_t *button, GPIO_TypeDef *port, uint16_t pin).
 * @brief  Initializes a button with specified port and pin.
 * @param  button Pointer to the Button_t structure.
 * @param  port GPIO port.
 * @param  pin GPIO pin.
 * @param pull Pull configuration (GPIO_PULLUP, GPIO_PULLDOWN, or GPIO_NOPULL).
 * @return void
 */
void Button_Init(Button_t *button, GPIO_TypeDef *port, uint16_t pin,
		uint32_t pull);

/*!
 * @fn     return_t Button_GetFinalCount(Button_t *button, uint8_t *finalCount).
 * @brief  Gets the final button push count after validation.
 * @param  button Pointer to the Button_t structure.
 * @param  finalCount Pointer to store the final push count.
 * @return return_t Returns return_success, return_failed, or return_busy.
 */
return_t Button_GetFinalCount(Button_t *button, uint8_t *finalCount);

#endif /* BUTTON_HANDLER_H */

/* ******************** (C) COPYRIGHT [KeyhanSalehi] ******************* */

