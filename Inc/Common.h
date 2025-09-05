/*!
 *******************************************************************************
 * @file           : Common.h
 * @author         : KeyhanSalehi
 * @brief          : all things usually common with all class's and module's
 *******************************************************************************
 * @attention
 *
 * [License or attention note, e.g., copyright or legal information]
 *
 *******************************************************************************
 */

#ifndef INC_COMMON_H_
#define INC_COMMON_H_

/* Includes ------------------------------------------------------------------*/

/* 1. System Header Files */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* 2. Project Header Files */
#include "main.h"

/* Defines & Macros ----------------------------------------------------------*/

/*!
 * @brief uint8_t flag's state defined here.
 */
#define FLAG_ENABLE 1
#define FLAG_DISABLE 0

/* Typedefs ------------------------------------------------------------------*/

/*!
 * @brief all state a function can have.
 */
typedef enum {
	return_success, /* finish successfully */
	return_failed, /* finish with error */
	return_busy, /* function in the process */
} return_t;

/* Exported Variables --------------------------------------------------------*/

/* 1. Global Variables */

/* Exported Functions --------------------------------------------------------*/

/* 1. Global Function Declarations */

#endif /* INC_COMMON_H_ */

/* ******************** (C) COPYRIGHT [KeyhanSalehi] ******************* */

