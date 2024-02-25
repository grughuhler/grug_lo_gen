/* SPDX-License-Identifier: BSD-3-Clause  */

/* This program generates square waves in quadrature.  The wave that is
 * shifted 90 deg in phase is on pin F1_SHIFTED_PIN and the non-shifted
 * wave is on F1_SHIFTED_PIN + 1.
 *
 * The pico is assumed powered by USB which also creates a USB-based
 * serial port on the host running the SDR software.  This program
 * listens for commands on that serial port.
 *
 * By default, it is not verbose.  Just enter a frequency in Hz followed
 * by return.  It then prints the actual frequency it managed.  There
 * will be less difference between the target and actual frequencies for
 * lower target frequencies, also less jitter.
 *
 * The Pico PIO hardware generates the output waves so also see
 * grug_lo_gen.pio for the very simple PIO program used.
 *
 * For building, I install the Pico C SDK per instructions, including
 * pico-examples.  Be sure you can build one of the pio examples such as
 * pio/hello_pio.
 *
 * Then clone this repo in pico-examples/pio and edit
 * pico-examples/pio/CMakeLists.txt to add a subdirectory for it.
 *
 * I build on Linux.  I have never tried the Pico C SDK on Windows.
 *
 * I like to flash using the picotool method (to avoid needing to hold
 * the button down), but you must use the button method for the first
 * deployment unless whatever is already in the Pico flash is configured
 * for picotool.
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

/* Assembled (generated) PIO program */
#include "grug_lo_gen.pio.h"

/* Choose output pins
 * F1_UNSHIFTED is F1_SHIFTED_PIN + 1
 */
#define F1_SHIFTED_PIN 14
#define INIT_FREQ (1290079.0)

int main()
{
  PIO pio = pio0;
  bool ok;
  float freq = INIT_FREQ, incr = 1000, sum = 0, actual;
  char ch;
  int new_freq = 0;
  int verbose = 0;

  /* PIO clock is derived from sys_clock */
  ok = set_sys_clock_khz(128000, false);
  
  stdio_init_all();
  sleep_ms(1);
  if (verbose) printf("starting grug_lo_gen\n");
  if (ok) {
    if (verbose) printf("clock set to %ld\n", clock_get_hz(clk_sys));
  }

  uint offset = pio_add_program(pio, &grug_lo_gen_program);

  uint sm = pio_claim_unused_sm(pio, true);
  grug_lo_gen_program_init(pio, sm, offset, F1_SHIFTED_PIN);

  sleep_ms(100);
  grug_lo_gen_set_freq(pio, sm, 4*freq, &actual);
  freq = actual/4.0;

  while (1) {
    ch = getchar();
    switch (ch) {
    case 'h':
      printf("[0-9]+: enter freq\n");
      printf("c: print current freq\n");
      printf("u|d: up or down by incr\n");
      printf("U|D: multiply/devide incr by 10\n");
      printf("r: goto to default freq (%f)\n", INIT_FREQ);
      break;
    case 'r':
      freq = INIT_FREQ;
      new_freq = 1;
      break;
    case 'c':
      printf("current: %f\n", freq);
      break;
    case 'u':
      freq += incr;
      new_freq = 1;
      break;
    case 'd':
      freq -= incr;
      if (freq < 0.0) freq = 1000;
      new_freq = 1;
      break;
    case 'U':
      incr *= 10.0;
      printf("incr: %f\n", incr);
      break;
    case 'D':
      incr /= 10.0;
      if (incr < 1.0) incr = 1.0;
      printf("incr: %f\n", incr);
      break;
    case '\r':
      if (verbose) putchar('\n');
      freq = sum;
      sum = 0;
      new_freq = 1;
      break;
    case 'v':
      verbose = 0;
      break;
    case 'V':
      verbose = 1;
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if (verbose) putchar(ch);
      sum = 10*sum + (ch - '0');
      break;
    default:
      break;
    }
    if (new_freq) {
      if (verbose) printf("setting %f\n", freq);
      grug_lo_gen_set_freq(pio, sm, 4*freq, &actual);
      printf("%ld\n", (long int) (actual/4.0 + 0.5));
      new_freq = 0;
    }
  }
  
  return 0;
}
