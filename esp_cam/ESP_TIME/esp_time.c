//SNTP
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "esp_attr.h"
#include "esp_sleep.h"
#include "esp_sntp.h"
#include "esp_logging.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_time.h"

#define MODULE_TAG 3 //cam_time module
void time_sync_notification_cb(struct timeval *tv) {
    LOGI_0(MODULE_TAG, TIME_SYNC_EVENT);
}

void Get_current_date_time(char *date_time) {
	char strftime_buf[64];
	time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);
    // Set timezone to Indian Standard Time
	setenv("TZ", "UTC-1", 1);
	tzset();
	localtime_r(&now, &timeinfo);

	strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    strcpy(date_time,strftime_buf);
}


static void initialize_sntp(void) {
    LOGI_0(MODULE_TAG, INIT_SNTP);
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
#endif
    sntp_init();
}

static void obtain_time(void) {
    initialize_sntp();
    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;
    char buf1[16];
    char buf2[16];

    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        snprintf(buf1, 16, "%d", retry);  
        snprintf(buf2, 16, "%d", retry_count);
        LOGI_2(MODULE_TAG, WAITING_TO_BE_SET, &buf1, &buf2);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    time(&now);
    localtime_r(&now, &timeinfo);
}

 void time_init()  {

	time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);
	// Is time set? If not, tm_year will be (1970 - 1900).
	if (timeinfo.tm_year < (2016 - 1900)) {
	    LOGI_0(MODULE_TAG, TIME_NOT_SET);
	    obtain_time();
	    // update 'now' variable with current time
	    time(&now);
	}

}