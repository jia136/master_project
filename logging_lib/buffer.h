#ifndef __BUFFER_H__
#define __BUFFER_H__

 #include <stdint.h>
 #include <stdbool.h>
 #include <stdlib.h>

 #define BUFFER_SIZE 1024

 struct buffer {

    uint32_t ui32_tail;
    uint32_t ui32_head;
    uint32_t ui32_max_size;
    uint32_t ui32_no_of_el;
    uint8_t pui8_buff_data[BUFFER_SIZE];

};

 typedef struct buffer buffer_t;

 extern buffer_t log_buffer;

 void buffer_init(uint32_t ui32_max_size);
 int32_t buffer_read(uint8_t* pui8_data);
 int32_t buffer_write(uint8_t ui8_data);

 #endif
