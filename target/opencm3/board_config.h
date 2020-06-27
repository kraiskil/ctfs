// Target configs that don't fit into the <board>.cc
// Consider creating and moving these to a <board>.h

#if defined STM32F4_BLUEPILL
#include "stm32f4_bluepill.h"
#elif defined STM32F4_DISCO
#include "stm32f4_discovery.h"
#else
#error unknown target board
#endif

