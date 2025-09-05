/**
 ******************************************************************************
 * @file           : button_handler.c
 * @author         : KeyhanSalehi
 * @brief          : Implementation of the Button Handler library.
 ******************************************************************************
 * @attention
 * 
 * Provides functions to manage button inputs with debounCing and
 * timeout using a software timer.
 * 
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/

/* 1. System Header Files */

/* 2. Project Header Files */

/* 3. Module Header File */
#include "button_handler.h"

/* Defines & Macros ----------------------------------------------------------*/
/**
 * @brief Constants and macros specific to this file.
 */

/*! @def @brief Define for deBounce delay (50ms).*/
#define DEBOUNCE_DELAY_MS 50

/*! @def @brief Define for timeout threshold (1 seconds).*/
#define BUTTON_TIMEOUT_MS SEC_TO_MS(1)

/* Typedefs ------------------------------------------------------------------*/
/**
 * @brief Typedefs for local use in this file.
 */

/* Variables -----------------------------------------------------------------*/

/* 1. Global Variables */

/* 2. Static Variables */

/* Function Declarations -----------------------------------------------------*/

/* 1. Local Prototype Functions */

/*! @fn @private */
static return_t Button_CountPushes(Button_t *button);

/* 2. Global Function Declarations */

/*!
 * @fn     void Button_Init(Button_t *button, GPIO_TypeDef *port, uint16_t pin).
 * @brief  Initializes a button with specified port and pin.
 * @param  button Pointer to the Button_t structure.
 * @param  port GPIO port.
 * @param  pin GPIO pin.
 * @return void
 */
void Button_Init(Button_t *button, GPIO_TypeDef *port, uint16_t pin,
		uint32_t pull) {
	button->port = port;
	button->pin = pin;
	button->pushCount = 0;
	button->isReadFinish = false;
	softTimer_reset(&button->debounceTimer); /* Initialize deBounce timer */
	softTimer_reset(&button->timeoutTimer); /* Initialize timeout timer */
	button->lastState = GPIO_PIN_RESET;
	button->pull = pull; /* Store the pull configuration */

	/*! @note massage for developer : Configure GPIO as input with pull-up in CubeMx */
}

/*!
 * @fn     return_t Button_GetFinalCount(Button_t *button, uint8_t *finalCount).
 * @brief  Gets the final button push count after validation.
 * @param  button Pointer to the Button_t structure.
 * @param  finalCount Pointer to store the final push count.
 * @return return_t Returns return_success, return_failed, or return_busy.
 */
return_t Button_GetFinalCount(Button_t *button, uint8_t *finalCount) {

	/* local variable */
	return_t finalResult = return_busy;
	return_t countResult = Button_CountPushes(button); /* save result */

	/* check result */
	if (countResult == return_success) {
		if (button->pushCount > 0 && button->pushCount <= PUSH_COUNT_MAX) {
			*finalCount = button->pushCount;
			finalResult = return_success;
		} else {
			finalResult = return_failed;
		}
		button->pushCount = 0;
		button->isReadFinish = false;
	} else {
		/* return_failed or return_busy directly */
		finalResult = countResult;
	}

	/* return final result */
	return finalResult;
}

/* 3. Local Function Declarations */

/*!
 * @fn     static return_t Button_CountPushes(Button_t *button).
 * @brief  Counts the number of button pushes.
 * @param  button Pointer to the Button_t structure.
 * @return return_t Returns return_success, return_failed, or return_busy.
 */
static return_t Button_CountPushes(Button_t *button) {

	/* Local variable & initial */
	GPIO_PinState currentState = HAL_GPIO_ReadPin(button->port, button->pin);
	/* Determine the active and inactive states based on pull configuration */
	GPIO_PinState activeState =
			(button->pull == GPIO_PULLUP) ? GPIO_PIN_RESET : GPIO_PIN_SET;
	GPIO_PinState inactiveState =
			(button->pull == GPIO_PULLUP) ? GPIO_PIN_SET : GPIO_PIN_RESET;

	/* Check for state transition with debounCing */
	if (currentState == activeState && button->lastState == inactiveState) {
		if (softTimer_isElapsed(&button->debounceTimer, DEBOUNCE_DELAY_MS)) {
			softTimer_reset(&button->debounceTimer); /* Reset deBounce timer on valid press */
			softTimer_reset(&button->timeoutTimer);
			button->pushCount++;
		}
	}
	button->lastState = currentState;

	/* Check timeout (runs independently) */
	if (softTimer_isElapsed(&button->timeoutTimer, BUTTON_TIMEOUT_MS)) {
		button->isReadFinish = true;
		softTimer_reset(&button->timeoutTimer); /* Reset timeout timer */
	}

	/* check pushCount not overflow before send result */
	if (button->pushCount > PUSH_COUNT_MAX) {
		button->pushCount = 0;
		return return_failed;
	}

	if (button->isReadFinish == true) {
		return return_success;
	}

	return return_busy;
}

/************************ (C) COPYRIGHT [Your Company Name] *****END OF FILE****/
