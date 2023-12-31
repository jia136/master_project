#ifndef ESP_LOGGING_BUFFER_H_INCLUDED
#define ESP_LOGGING_BUFFER_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024
#define MAX_TEMP_BUFF 100

struct buffer {

    uint32_t ui32_tail;
    uint32_t ui32_head;
    uint32_t ui32_max_size;
    uint32_t ui32_no_of_el;

};

typedef struct buffer buffer_t;

void buffer_init(uint32_t ui32_max_size);

int32_t buffer_write(char data_buff[BUFFER_SIZE], int8_t ui8_data);

int32_t buffer_write_log(char data_buff[BUFFER_SIZE], int8_t temp_buff[MAX_TEMP_BUFF], uint8_t log_arg_num);


#endif // ESP_LOGGING_BUFFER_H_INCLUDED
