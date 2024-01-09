#ifndef _ESP_WEB_H_
#define _ESP_WEB_H_

void post_rest_function(float temp, float pres, int alarm_activation);
void send_log_function(const char * log_data, int16_t log_capacity);

#endif