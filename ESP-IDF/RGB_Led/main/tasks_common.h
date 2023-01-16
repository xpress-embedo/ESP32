/*
 * tasks_common.h
 *
 *  Created on: 28-Dec-2022
 *      Author: xpress_embedo
 */

#ifndef MAIN_TASKS_COMMON_H_
#define MAIN_TASKS_COMMON_H_

// Wi-Fi Application Task
#define WIFI_APP_TASK_STACK_SIZE            (4096u)
#define WIFI_APP_TASK_PRIORIIRY             (5)
#define WIFI_APP_TASK_CORE_ID               (0)
#define WIFI_APP_QUEUE_SIZE                 (3)

// HTTP Server Task
#define HTTP_SERVER_TASK_STACK_SIZE         (8192u)
#define HTTP_SERVER_TASK_PRIORITY           (4)
#define HTTP_SERVER_TASK_CORE_ID            (0)
#define HTTP_SERVER_QUEUE_SIZE              (3)     // TODO

// HTTP Server Monitor Task
#define HTTP_SERVER_MONITOR_TASK_STACK_SIZE (4096u)
#define HTTP_SERVER_MONITOR_TASK_PRIORITY   (3)
#define HTTP_SERVER_MONITOR_TASK_CORE_ID    (0)
#define HTTP_SERVER_MONITOR_QUEUE_SIZE      (3)     // TODO

#endif /* MAIN_TASKS_COMMON_H_ */
