#include "esp_logging_buffer.h"

buffer_t log_buffer;

void buffer_init(uint32_t ui32_max_size) {

    log_buffer.ui32_tail = 0;
    log_buffer.ui32_head = 0;
    log_buffer.ui32_max_size = ui32_max_size;
    log_buffer.ui32_no_of_el = 0;

}

int32_t buffer_write(char data_buff[BUFFER_SIZE], int8_t ui8_data) {

    bool b_ret_value = 0;
    int32_t i32_next_item;

    i32_next_item = (log_buffer.ui32_head + 1) % (log_buffer.ui32_max_size);

    if (i32_next_item == log_buffer.ui32_tail) {
        b_ret_value = 1; //buffer is full
    }
    else if ( log_buffer.ui32_no_of_el < log_buffer.ui32_max_size ) {
        (log_buffer.ui32_no_of_el)++;
        data_buff[log_buffer.ui32_head] = ui8_data;
        log_buffer.ui32_head = i32_next_item;
    }
    else {
        b_ret_value = -1; //error
    }

    return b_ret_value;

}

int32_t buffer_write_log(char data_buff[BUFFER_SIZE], int8_t temp_buff[MAX_TEMP_BUFF], uint8_t log_arg_num) {

    bool b_ret_value = 0;

    if ( ( log_buffer.ui32_head + log_arg_num ) > log_buffer.ui32_max_size ) {
        b_ret_value = 1; //no enough space for this log in buffer
    }
    else {
        for (int32_t i = 0; i < log_arg_num; i++) {
            if ( buffer_write(data_buff, temp_buff[i]) != 0 ) {
                b_ret_value = -1; //error
                break;
            }
        }
    }

    return b_ret_value;

}


int32_t buffer_read(char data_buff[BUFFER_SIZE], int8_t* pui8_data) {

    bool b_ret_value = 0;
    int32_t i32_next_item;

    if ( log_buffer.ui32_no_of_el == 0 ) {
        b_ret_value = 1; //buffer is empty
    }
    else {
        i32_next_item = (log_buffer.ui32_tail + 1) % (log_buffer.ui32_max_size);

        *pui8_data = data_buff[log_buffer.ui32_tail];
        log_buffer.ui32_tail = i32_next_item;
        (log_buffer.ui32_no_of_el)--;
    }

    return b_ret_value;

}
