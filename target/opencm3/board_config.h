// Target configs that don't fit into the <board>.cc
// This file is #included from the source files in
// src/lib/*.cc
#pragma once


#if defined STM32F4_BLUEPILL
#define STM32F4
#include "stm32f4_bluepill.h"
#elif defined STM32F4_DISCO
#define STM32F4
#include "stm32f4_discovery.h"
#elif defined STM32F4_NUCLEO
#include "stm32f4_nucleo.h"
#else
#error unknown target board
#endif

