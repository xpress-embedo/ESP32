/*
 * thingspeak.h
 *
 *  Created on: Feb 5, 2024
 *      Author: xpress_embedo
 */

#ifndef MAIN_THINGSPEAK_H_
#define MAIN_THINGSPEAK_H_

typedef enum {
  THING_SPEAK_EV_NONE = 0,
  THING_SPEAK_EV_TEMP_HUMID = 1,    // Event for temperature and humidity update
  THING_SPEAK_EV_MAX,
} thingspeak_event_t;

typedef struct _thingspeak_q_msg_t {
  thingspeak_event_t  event_id;
  uint8_t             *data;
} thingspeak_q_msg_t;

// Public Function Prototypes
void thingspeak_start( void );
BaseType_t thingspeak_send_event( thingspeak_event_t event, uint8_t *pData );


#endif /* MAIN_THINGSPEAK_H_ */
