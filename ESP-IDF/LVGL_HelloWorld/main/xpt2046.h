/*
 * xpt2046.h
 *
 *  Created on: Jan 20, 2024
 *      Author: xpress_embedo
 */

#ifndef MAIN_XPT2046_H_
#define MAIN_XPT2046_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>

// Public Properties
void xpt2046_init(void);
uint8_t xpt2046_read(int16_t *det_x, int16_t *det_y);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MAIN_XPT2046_H_ */
