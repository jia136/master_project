#include "buffer.h"

 buffer_t log_buffer;

 void buffer_init(uint32_t ui32_max_size) {

    log_buffer.ui32_tail = 0;
    log_buffer.ui32_head = 0;
    log_buffer.ui32_max_size = ui32_max_size;
    log_buffer.ui32_no_of_el = 0;

 }

 int32_t buffer_read(uint8_t* pui8_data) {

    bool b_ret_value = 0;
    int32_t i32_next_item;

    if ( log_buffer.ui32_no_of_el == 0 ) {
        b_ret_value = 1;
    }
    else {
        i32_next_item = (log_buffer.ui32_tail + 1) % (log_buffer.ui32_max_size);

        *pui8_data = log_buffer.pui8_buff_data[log_buffer.ui32_tail];
        log_buffer.ui32_tail = i32_next_item;
        (log_buffer.ui32_no_of_el)--;
    }

    return b_ret_value;

 }

 int32_t buffer_write(uint8_t ui8_data) {

    bool b_ret_value = 0;
    int32_t i32_next_item;

    i32_next_item = (log_buffer.ui32_head + 1) % (log_buffer.ui32_max_size);

    if ( log_buffer.ui32_no_of_el < log_buffer.ui32_max_size ) {
        (log_buffer.ui32_no_of_el)++;
    }

    log_buffer.pui8_buff_data[log_buffer.ui32_head] = ui8_data;
    log_buffer.ui32_head = i32_next_item;

    return b_ret_value;

 }
