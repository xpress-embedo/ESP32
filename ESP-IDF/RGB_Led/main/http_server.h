/*
 * wifi_app.h
 *
 *  Created on: 16-Jan-2022
 *      Author: xpress_embedo
 */

#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

/*
 * Messages for the HTTP Monitor
 */
typedef enum _HTTP_Server_Msg_e
{
  HTTP_MSG_WIFI_CONNECT_INIT = 0,
  HTTP_MSG_WIFI_CONNECT_SUCCESS,
  HTTP_MSG_WIFI_CONNECT_FAIL,
  HTTP_MSG_OTA_UPDATE_SUCCESSFUL,
  HTTP_MSG_OTA_UPDATE_FAILED,
  HTTP_MSG_OTA_UPDATE_INITIALIZED,
} HTTP_Server_Msg_e;

/*
 * Structure for the message queue
 */
typedef struct _HTTP_Server_Queue_Msg_s
{
  HTTP_Server_Msg_e msg_id;
} HTTP_Server_Queue_Msg_s;

// Public Function Prototypes
BaseType_t HTTP_ServerMonitor_SendMsg( HTTP_Server_Msg_e msg_id );
void HTTP_ServerStart( void );
void HTTP_ServerStop( void );


#endif /* HTTP_SERVER_H_ */
