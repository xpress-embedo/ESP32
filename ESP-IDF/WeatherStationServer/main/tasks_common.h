/*
 * tasks_common.h
 *
 *  Created on: 15-Jul-2023
 *      Author: xpress_embedo
 */

#ifndef MAIN_TASKS_COMMON_H_
#define MAIN_TASKS_COMMON_H_

// WiFi Application Task
#define WIFI_APP_TASK_STACK_SIZE                (4*1024u)
#define WIFI_APP_TASK_PRIORITY                  (5u)

// HTTP Server Task
#define HTTP_SERVER_TASK_STACK_SIZE             (8*1024u)
#define HTTP_SERVER_TASK_PRIORITY               (4u)

// HTTP Server Monitor
#define HTTP_SERVER_MONITOR_STACK_SIZE          (4*1024u)
#define HTTP_SERVER_MONITOR_PRIORITY            (3u)


#endif /* MAIN_TASKS_COMMON_H_ */
