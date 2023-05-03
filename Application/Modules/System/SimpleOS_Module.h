#ifndef SimpleOS_Module_H_
#define SimpleOS_Module_H_

#include "../Modules.h"

#define SYSTICK_DIFF(currrent,before,calc)  if (currrent >= before)     \
                                            {                           \
                                              calc = currrent - before; \
                                            }                           \
                                            else                        \
                                            {                           \
                                              calc = 0xFFFF - before;   \
                                              calc += currrent + 1;     \
                                            }


void SimpleOS_DelayMS(uint16_t ms);
uint16_t SimpleOS_GetTick(void);
bool SimpleOS_CheckInterval(uint16_t *flushTick, uint16_t interval);

void SimpleOS_init(void);
void SimpleOS_open(void);
void SimpleOS_close(void);
void SimpleOS_flush(void);

#ifdef RTT_OUT
#define RTT_INIT(fmt) (segger_rtt_init(fmt))
// #define RTT_PRINT(fmt, ...) print_log("[DEBUG] %s, %s(), line:%d, msg:" fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
// #define RTT_PRINT(fmt, ...) print_log("%s(), msg: " fmt, __func__, ##__VA_ARGS__)
#define RTT_PRINT(fmt, ...) print_log(fmt, ##__VA_ARGS__)
#define RTT_PRINT_0(fmt, ...) print_log_0(fmt, ##__VA_ARGS__)
#define RTT_PRINT_1(fmt, ...) print_log_1(fmt, ##__VA_ARGS__)
#define RTT_PRINT_2(fmt, ...) print_log_2(fmt, ##__VA_ARGS__)
#define RTT_PRINT_3(fmt, ...) print_log_3(fmt, ##__VA_ARGS__)
/* Exported functions ------------------------------------------------------- */
void segger_rtt_init(char *str);
void print_log(const char *sFormat, ...);
void print_log_0(const char *sFormat, ...);
void print_log_1(const char *sFormat, ...);
void print_log_2(const char *sFormat, ...);
void print_log_3(const char *sFormat, ...);
#else
#define RTT_INIT(fmt) ((void)0U)
#define RTT_PRINT(fmt, ...) ((void)0U)
#define RTT_PRINT_0(fmt, ...) ((void)0U)
#define RTT_PRINT_1(fmt, ...) ((void)0U)
#define RTT_PRINT_2(fmt, ...) ((void)0U)
#define RTT_PRINT_3(fmt, ...) ((void)0U)
#endif

#endif //SimpleOS_Module_H_
