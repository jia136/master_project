#ifndef ESP_LOGGING_H_INCLUDED
#define ESP_LOGGING_H_INCLUDED

#include <stdint.h>

#define NEXT_ARG 58
#define MSG_END  59

#define BUFFER_SIZE 2048


typedef enum {
    LOG_NONE,       /*!< No log output */
    LOG_ERROR,      /*!< Critical errors, software module can not recover on its own */
    LOG_WARN,       /*!< Error conditions from which recovery measures have been taken */
    LOG_INFO,       /*!< Information messages which describe normal flow of events */
    LOG_DEBUG,      /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    LOG_VERBOSE     /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
} log_level_t;

void log_init();

void log_level_set(uint16_t ui16_log_level);
void log_capacity_set(int16_t ui16_log_capacity);

void log_msg_0( log_level_t log_level, uint16_t ui16_module, uint16_t ui16_id );
void log_msg_1( log_level_t log_level, uint16_t ui16_module, uint16_t ui16_id, const void * const pui8_arg_0 );
void log_msg_2( log_level_t log_level, uint16_t ui16_module, uint16_t ui16_id, const void * const pui8_arg_0, const void * const pui8_arg_1 );
void log_msg_3( log_level_t log_level, uint16_t ui16_module, uint16_t ui16_id, const void * const pui8_arg_0, const void * const pui8_arg_1, const void * const pui8_arg_2 );

#define LOGE_0( ui16_module, ui16_id ) log_msg_0( LOG_ERROR, ui16_module, ui16_id )
#define LOGW_0( ui16_module, ui16_id ) log_msg_0( LOG_WARN, ui16_module, ui16_id )
#define LOGI_0( ui16_module, ui16_id ) log_msg_0( LOG_INFO, ui16_module, ui16_id )
#define LOGD_0( ui16_module, ui16_id ) log_msg_0( LOG_DEBUG, ui16_module, ui16_id )
#define LOGV_0( ui16_module, ui16_id ) log_msg_0( LOG_VERBOSE, ui16_module, ui16_id )

#define LOGE_1( ui16_module, ui16_id, pui8_arg_0 ) log_msg_1( LOG_ERROR, ui16_module, ui16_id, pui8_arg_0 )
#define LOGW_1( ui16_module, ui16_id, pui8_arg_0 ) log_msg_1( LOG_WARN, ui16_module, ui16_id, pui8_arg_0 )
#define LOGI_1( ui16_module, ui16_id, pui8_arg_0 ) log_msg_1( LOG_INFO, ui16_module, ui16_id, pui8_arg_0 )
#define LOGD_1( ui16_module, ui16_id, pui8_arg_0 ) log_msg_1( LOG_DEBUG, ui16_module, ui16_id, pui8_arg_0 )
#define LOGV_1( ui16_module, ui16_id, pui8_arg_0 ) log_msg_1( LOG_VERBOSE, ui16_module, ui16_id, pui8_arg_0 )

#define LOGE_2( ui16_module, ui16_id, pui8_arg_0, pui8_arg_1 ) log_msg_2( LOG_ERROR, ui16_module, ui16_id, pui8_arg_0, pui8_arg_1 )
#define LOGW_2( ui16_module, ui16_id, pui8_arg_0, pui8_arg_1 ) log_msg_2( LOG_WARN, ui16_module, ui16_id, pui8_arg_0, pui8_arg_1 )
#define LOGI_2( ui16_module, ui16_id, pui8_arg_0, pui8_arg_1 ) log_msg_2( LOG_INFO, ui16_module, ui16_id, pui8_arg_0, pui8_arg_1 )
#define LOGD_2( ui16_module, ui16_id, pui8_arg_0, pui8_arg_1 ) log_msg_2( LOG_DEBUG, ui16_module, ui16_id, pui8_arg_0, pui8_arg_1 )
#define LOGV_2( ui16_module, ui16_id, pui8_arg_0, pui8_arg_1 ) log_msg_2( LOG_VERBOSE, ui16_module, ui16_id, pui8_arg_0, pui8_arg_1 )

#define LOGE_3( ui16_module, ui16_id, pui8_arg_0, pui8_arg_1, pui8_arg_2 ) log_msg_3( LOG_ERROR, ui16_module, ui16_id, pui8_arg_0, pui8_arg_1, pui8_arg_2 )
#define LOGW_3( ui16_module, ui16_id, pui8_arg_0, pui8_arg_1, pui8_arg_2 ) log_msg_3( LOG_WARN, ui16_module, ui16_id, pui8_arg_0, pui8_arg_1, pui8_arg_2 )
#define LOGI_3( ui16_module, ui16_id, pui8_arg_0, pui8_arg_1, pui8_arg_2 ) log_msg_3( LOG_INFO, ui16_module, ui16_id, pui8_arg_0, pui8_arg_1, pui8_arg_2 )
#define LOGD_3( ui16_module, ui16_id, pui8_arg_0, pui8_arg_1, pui8_arg_2 ) log_msg_3( LOG_DEBUG, ui16_module, ui16_id, pui8_arg_0, pui8_arg_1, pui8_arg_2 )
#define LOGV_3( ui16_module, ui16_id, pui8_arg_0, pui8_arg_1, pui8_arg_2 ) log_msg_3( LOG_VERBOSE, ui16_module, ui16_id, pui8_arg_0, pui8_arg_1, pui8_arg_2 )

#endif // ESP_LOGGING_H_INCLUDED
