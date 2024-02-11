/*
 * sntp_time_sync.h
 *
 *  Created on: Feb 11, 2024
 *      Author: xpress_embedo
 */

#ifndef MAIN_SNTP_TIME_SYNC_H_
#define MAIN_SNTP_TIME_SYNC_H_

// Public Function Declaration
void sntp_time_sync_start( void );
char * sntp_time_sync_get_time( void );
void sntp_time_sync_get_time_tm( struct tm *time_info );

#endif /* MAIN_SNTP_TIME_SYNC_H_ */
