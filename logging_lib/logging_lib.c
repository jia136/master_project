#include "logging_lib.h"
#include "buffer.h"

static uint16_t current_log_level = 8;
static uint32_t current_log_capacity = 1024;

void log_arg(const uint8_t * const pui8_arg);
void log_vmi(uint16_t ui16_vmi_packed);

uint16_t packed_vmi_log(uint16_t ui16_severity, uint16_t ui16_module, uint16_t ui16_id);


void log_init() {

    buffer_init(current_log_capacity);

}

void log_level_set(uint16_t ui16_log_level) {

    current_log_level = ui16_log_level;

}

void log_capacity_set(uint32_t ui16_log_capacity) {

    current_log_capacity = ui16_log_capacity;

}

void log_arg(const uint8_t * const pui8_arg) {

    int32_t i = 0;
    while (*(pui8_arg + i) != '\0') {
       buffer_write(*(pui8_arg + i));
       ++i;
       if (i >= 31) { //ogranicenje da svaki argument moze da ima najvise 32 znaka
        break;
       }
    }

}

void log_vmi(uint16_t ui16_vmi_packed) {

    buffer_write((uint8_t)(ui16_vmi_packed >> 8));
    buffer_write((uint8_t)(ui16_vmi_packed & 0xFF));

}

uint16_t packed_vmi_log(uint16_t ui16_severity, uint16_t ui16_module, uint16_t ui16_id) {

    uint16_t ui16_vmi_packed = 0;

    ui16_vmi_packed |= (0x0007 & ui16_severity);
    ui16_vmi_packed |= (0x00F8 & (ui16_module << 3));
    ui16_vmi_packed |= (0xFF00 & (ui16_id << 8));

    return ui16_vmi_packed;
}

void log_msg_0(uint16_t ui16_severity, uint16_t ui16_module, uint16_t ui16_id) {

    if (current_log_level >= ui16_severity) {

        uint16_t ui16_vmi_packed = packed_vmi_log(ui16_severity, ui16_module, ui16_id);

        log_vmi(ui16_vmi_packed);
        buffer_write((uint8_t)MSG_END);

    }

}

void log_msg_1(uint16_t ui16_severity, uint16_t ui16_module, uint16_t ui16_id, const uint8_t * const pui8_arg_0) {

    if (current_log_level >= ui16_severity) {

        uint16_t ui16_vmi_packed = packed_vmi_log(ui16_severity, ui16_module, ui16_id);

        log_vmi(ui16_vmi_packed);
        buffer_write((uint8_t)NEXT_ARG);
        log_arg(pui8_arg_0);
        buffer_write((uint8_t)MSG_END);

    }

}

void log_msg_2(uint16_t ui16_severity, uint16_t ui16_module, uint16_t ui16_id, const uint8_t * const pui8_arg_0, const uint8_t * const pui8_arg_1) {

    if (current_log_level >= ui16_severity) {

        uint16_t ui16_vmi_packed = packed_vmi_log(ui16_severity, ui16_module, ui16_id);

        log_vmi(ui16_vmi_packed);
        buffer_write((uint8_t)NEXT_ARG);
        log_arg(pui8_arg_0);
        buffer_write((uint8_t)NEXT_ARG);
        log_arg(pui8_arg_1);
        buffer_write((uint8_t)MSG_END);

    }

}

void log_msg_3(uint16_t ui16_severity, uint16_t ui16_module, uint16_t ui16_id, const uint8_t * const pui8_arg_0, const uint8_t * const pui8_arg_1, const uint8_t * const pui8_arg_2) {

    if (current_log_level >= ui16_severity) {

        uint16_t ui16_vmi_packed = packed_vmi_log(ui16_severity, ui16_module, ui16_id);

        log_vmi(ui16_vmi_packed);
        buffer_write((uint8_t)NEXT_ARG);
        log_arg(pui8_arg_0);
        buffer_write((uint8_t)NEXT_ARG);
        log_arg(pui8_arg_1);
        buffer_write((uint8_t)NEXT_ARG);
        log_arg(pui8_arg_2);
        buffer_write((uint8_t)MSG_END);

    }

}
