#include "esp_logging.h"
#include "esp_logging_buffer.h"
#include "socket.h"

#define MAX_TEMP_BUFF 100

static int16_t current_log_level = 5;       //debug level
static int16_t current_log_capacity = 128;  //in bytes
int16_t log_capacity = 128;                 //in bytes

char data_buff[BUFFER_SIZE];

static void log_vmi(int8_t temp_buff[MAX_TEMP_BUFF], uint16_t ui16_vmi_packed, int8_t * ui8_log_size);

static uint16_t packed_vmi_log(log_level_t log_level, uint16_t ui16_module, uint16_t ui16_id);

static void log_arg(int8_t temp_buff[MAX_TEMP_BUFF], const int8_t * const pui8_arg, int8_t * ui8_log_size);

static void log_msg_end(int8_t temp_buff[MAX_TEMP_BUFF], int8_t * ui8_log_size );
static void log_msg_next_arg(int8_t temp_buff[MAX_TEMP_BUFF], int8_t * ui8_log_size );

void log_init() {

   buffer_init(current_log_capacity);
   log_capacity = current_log_capacity;

}

void log_level_set(uint16_t ui16_log_level) {

    if ( ui16_log_level < 6 ) {
        current_log_level = ui16_log_level;
    }

}

void log_capacity_set(int16_t ui16_log_capacity) {
    
    if ( ui16_log_capacity < 6 ) {
        current_log_capacity = ui16_log_capacity;
    }   

}

static void log_vmi( int8_t temp_buff[MAX_TEMP_BUFF], uint16_t ui16_vmi_packed, int8_t * ui8_log_size ) {

   temp_buff[0] = ((int8_t)(ui16_vmi_packed >> 8));
   temp_buff[1] = ((int8_t)(ui16_vmi_packed & 0xFF));
   (*ui8_log_size) = 2;

}

static uint16_t packed_vmi_log( log_level_t log_level, uint16_t ui16_module, uint16_t ui16_id ) {

   uint16_t ui16_vmi_packed = 0;

   ui16_vmi_packed |= (0x0007 & log_level);
   ui16_vmi_packed |= (0x00F8 & (ui16_module << 3));
   ui16_vmi_packed |= (0xFF00 & (ui16_id << 8));

   return ui16_vmi_packed;
}

static void log_arg( int8_t temp_buff[MAX_TEMP_BUFF], const int8_t * const pui8_arg, int8_t * ui8_log_size ) {

   int32_t i = 0;
   while (*(pui8_arg + i) != '\0') {
       temp_buff[i + (*ui8_log_size)] = (*(pui8_arg + i));
       ++i;
       if (i >= 15) { //ogranicenje da svaki argument moze da ima najvise 16 byte
           break;
       }
   }
   (*ui8_log_size) += i;

}

static inline void log_msg_end( int8_t temp_buff[MAX_TEMP_BUFF], int8_t * ui8_log_size ) {

   temp_buff[(*ui8_log_size)] = (int8_t)MSG_END;
   (*ui8_log_size)++;

}

static inline void log_msg_next_arg( int8_t temp_buff[MAX_TEMP_BUFF], int8_t * ui8_log_size ) {

   temp_buff[(*ui8_log_size)] = (int8_t)NEXT_ARG;
   (*ui8_log_size)++;

}

void log_msg_0( log_level_t log_level, uint16_t ui16_module, uint16_t ui16_id ) {

   int8_t temp_buff[MAX_TEMP_BUFF];
   int8_t ui8_log_size = 0;

    if (current_log_level >= log_level) {

        uint16_t ui16_vmi_packed = packed_vmi_log(log_level, ui16_module, ui16_id);

        log_vmi(temp_buff, ui16_vmi_packed, &ui8_log_size);
        log_msg_end(temp_buff, &ui8_log_size);
        //buffer_write_log(data_buff, temp_buff, ui8_log_size);
        //websocket_app_start(data_buff, log_capacity);
        //void log_init();
        if ( buffer_write_log(data_buff, temp_buff, ui8_log_size) != 0 ) {
            //BAFER JE PUN, TREBA SLATI PODATKE
            websocket_app_start(data_buff, log_capacity);
            void log_init();
        }

    }

}

void log_msg_1( log_level_t log_level, uint16_t ui16_module, uint16_t ui16_id, const void * const pui8_arg_0) {

    int8_t temp_buff[MAX_TEMP_BUFF];
    int8_t ui8_log_size = 0;

    if (current_log_level >= log_level) {

        uint16_t ui16_vmi_packed = packed_vmi_log(log_level, ui16_module, ui16_id);

        log_vmi(temp_buff, ui16_vmi_packed, &ui8_log_size);
        log_msg_next_arg(temp_buff, &ui8_log_size);
        log_arg(temp_buff, pui8_arg_0, &ui8_log_size);
        log_msg_end(temp_buff, &ui8_log_size);

        if ( buffer_write_log(data_buff, temp_buff, ui8_log_size) != 0 ) {
            //BAFER JE PUN, TREBA SLATI PODATKE
            websocket_app_start(data_buff, log_capacity);
            void log_init();
        }

    }

}

void log_msg_2( log_level_t log_level, uint16_t ui16_module, uint16_t ui16_id, const void * const pui8_arg_0, const void * const pui8_arg_1 ) {

    int8_t temp_buff[MAX_TEMP_BUFF];
    int8_t ui8_log_size = 0;

    if (current_log_level >= log_level) {

        uint16_t ui16_vmi_packed = packed_vmi_log(log_level, ui16_module, ui16_id);

        log_vmi(temp_buff, ui16_vmi_packed, &ui8_log_size);
        log_msg_next_arg(temp_buff, &ui8_log_size);
        log_arg(temp_buff, pui8_arg_0, &ui8_log_size);
        log_msg_next_arg(temp_buff, &ui8_log_size);
        log_arg(temp_buff, pui8_arg_1, &ui8_log_size);
        log_msg_end(temp_buff, &ui8_log_size);

        if ( buffer_write_log(data_buff, temp_buff, ui8_log_size) != 0 ) {
            //BAFER JE PUN, TREBA SLATI PODATKE
            websocket_app_start(data_buff, log_capacity);
            void log_init();
        }

    }

}

void log_msg_3( log_level_t log_level, uint16_t ui16_module, uint16_t ui16_id, const void * const pui8_arg_0, const void * const pui8_arg_1, const void * const pui8_arg_2 ) {

    int8_t temp_buff[MAX_TEMP_BUFF];
    int8_t ui8_log_size = 0;

    if (current_log_level >= log_level) {

        uint16_t ui16_vmi_packed = packed_vmi_log(log_level, ui16_module, ui16_id);

        log_vmi(temp_buff, ui16_vmi_packed, &ui8_log_size);
        log_msg_next_arg(temp_buff, &ui8_log_size);
        log_arg(temp_buff, pui8_arg_0, &ui8_log_size);
        log_msg_next_arg(temp_buff, &ui8_log_size);
        log_arg(temp_buff, pui8_arg_1, &ui8_log_size);
        log_msg_next_arg(temp_buff, &ui8_log_size);
        log_arg(temp_buff, pui8_arg_2, &ui8_log_size);
        log_msg_end(temp_buff, &ui8_log_size);

        if ( buffer_write_log(data_buff, temp_buff, ui8_log_size) != 0 ) {
            //BAFER JE PUN, TREBA SLATI PODATKE
            websocket_app_start(data_buff, log_capacity);
            void log_init();
        }

    }
}

