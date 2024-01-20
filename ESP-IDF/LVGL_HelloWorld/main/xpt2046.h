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
void xpt2046_read(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MAIN_XPT2046_H_ */
