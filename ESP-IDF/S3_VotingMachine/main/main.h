/*
 * main.h
 *
 *  Created on: Jun 17, 2024
 *      Author: xpress_embedo
 */

#ifndef MAIN_MAIN_H_
#define MAIN_MAIN_H_

#include <unistd.h>

// macros
#define MAX_PARTY_NAME_LEN              (10)        // Assuming Congress party has longest name + Null Character and \n
#define MAX_NUM_OF_PARTY                (7)         // Let's assume 7 number of parties are maximum (as of now we can only display 7 without scrolling)

// Public Function Prototypes
uint8_t get_number_of_parties( void );
char *  get_name_of_party( uint8_t party_idx );

#endif /* MAIN_MAIN_H_ */
