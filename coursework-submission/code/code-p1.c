#include "contiki.h"
#include "dev/light-sensor.h"
#include "dev/sht11-sensor.h"
#include <stdio.h>

// Macros for buffer size and sample interval etc
#define BUFFER_SIZE 12
#define SAMPLE_INTERVAL (CLOCK_CONF_SECOND / 2)

// Square root
static int square_root_max_iterations = 20;
static float square_root_precision = 0.001f;

// Deviation thresholds
static float low_deviation_threshold = 100.0f;
static float high_deviation_threshold = 400.0f;

// Light buffer
float light_buffer[BUFFER_SIZE];
static int count = 0;
static int k = 12;

// ===== CUSTOM PRINTING =====
int d1(float f)
{
  // Integer part of the float
  return((int)f);
}

unsigned int d2(float f)
{
  // Find decimal part of the float
  if (f>0)
    return(1000*(f-d1(f)));
  else
    return(1000*(d1(f)-f));
}

void printFloat(float f)
{
  // Using the above functions, print a float
  printf("%d.%d", d1(f), d2(f));
}
