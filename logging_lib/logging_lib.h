#ifndef __LOGGING_LIB_H__
#define __LOGGING_LIB_H__

#include <stdint.h>

#define NEXT_ARG 58
#define MSG_END  59

void log_init();

void log_level_set(uint16_t ui16_log_level);
void log_capacity_set(uint32_t ui32_log_capacity);

void log_msg_0(uint16_t ui16_severity, uint16_t ui16_module, uint16_t ui16_id);
void log_msg_1(uint16_t ui16_severity, uint16_t ui16_module, uint16_t ui16_id, const uint8_t * const pui8_arg_0);
void log_msg_2(uint16_t ui16_severity, uint16_t ui16_module, uint16_t ui16_id, const uint8_t * const pui8_arg_0, const uint8_t * const pui8_arg_1);
void log_msg_3(uint16_t ui16_severity, uint16_t ui16_module, uint16_t ui16_id, const uint8_t * const pui8_arg_0, const uint8_t * const pui8_arg_1, const uint8_t * const pui8_arg_2);

#endif
