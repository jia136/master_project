#ifndef _WIFI_H_
#define _WIFI_H_

#include "nvs_flash.h"

enum wifi_msg {CONNECT_FAIL, RETRY_TO_CONNECT, INIT_DONE, CONNECT_TO_SSID_PASS, FAIL_SSID_PASS, UNEXPECTED};

void wifi_init(void);

#endif