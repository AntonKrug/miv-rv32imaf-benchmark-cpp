/*******************************************************************
 * (c) Copyright 2016-2017 Microsemi SoC Products Group. All rights reserved.
 * C++ centric benchmark
 * version 1.0 - 16/12/2017 by anton.krug@microsemi.com
 * Using UART at 115200 baudrate with MiV Core running 40MHz
 *******************************************************************/

#include <stdio.h>
#include "riscv_hal.h"
#include "hw_platform.h"

#include "raytracer.h"

volatile uint32_t tick_current;

void SysTick_Handler(void) {
	tick_current++;
}

uint32_t test_raytracer() {
  uint32_t expected = 0;
  uint32_t actual;
  uint32_t start;
  uint32_t end;

  start = tick_current;
  actual = raytracer();
  end = tick_current;
  printf("Raytracer checksum= %s, time=%dms\n", (expected != actual) ? "PASS" : "FAIL");
  return end - start;
}


int main() {
  uint32_t tick_total = 0;

  printf("\nFloating C++ benchmark started\n");

  SysTick_Config(SYS_CLK_FREQ/1000);
  tick_total += test_raytracer();
  printf("Finished in final time %d\n", tick_total);


  return 0;
}
