#ifndef _TIME_LOG_H_
#define _TIME_LOG_H_

enum time_msg {TIME_SYNC_EVENT, INIT_SNTP, WAITING_TO_BE_SET, TIME_NOT_SET};

void time_init(void);

#endif
