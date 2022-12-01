#include <cstdint>


#ifndef NDEBUG
extern uint32_t total_execution_time;
extern uint32_t fft_execution_time;
extern uint32_t peak_detect_execution_time;
#define DB_PRINT(...) printf(__VA_ARGS__)

#else
#define DB_PRINT(...)
#endif

