/*
 * http_server.h
 *
 *  Created on: Aug 8, 2025
 *      Author: xpress_embedo
 */

#ifndef MAIN_HTTP_SERVER_H_
#define MAIN_HTTP_SERVER_H_


/*
 * Messages for the HTTP Monitor
 */
typedef enum http_server_msg
{
  HTTP_MSG_WIFI_CONNECT_INIT = 0,
  HTTP_MSG_WIFI_CONNECT_SUCCESS,
  HTTP_MSG_WIFI_CONNECT_FAIL,
  HTTP_MSG_WIFI_USER_DISCONNECT,
  HTTP_MSG_WIFI_OTA_UPDATE_SUCCESSFUL,  // to be removed
  HTTP_MSG_WIFI_OTA_UPDATE_FAILED,      // to be removed
} http_server_msg_e;

/*
 * Structure for Message Queue
 */
typedef struct http_server_q_msg
{
  http_server_msg_e msg_id;
} http_server_q_msg_t;

// Public Function Declaration
void http_server_start(void);
void http_server_stop(void);
BaseType_t http_server_monitor_send_msg(http_server_msg_e msg_id);

#endif /* MAIN_HTTP_SERVER_H_ */
