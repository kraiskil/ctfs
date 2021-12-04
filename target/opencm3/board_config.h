// Target configs that don't fit into the <board>.cc
#pragma once

#if defined STM32F4_BLUEPILL
#define STM32F4
#include "stm32f4_bluepill.h"
#elif defined STM32F4_DISCO
#define STM32F4
#include "stm32f4_discovery.h"
#else
#error unknown target board
#endif

