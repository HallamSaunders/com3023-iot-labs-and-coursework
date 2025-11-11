#include "contiki.h"
#include "dev/light-sensor.h"
#include "dev/sht11-sensor.h"
#include <stdio.h>

// Initialise some of the constants we'll need
#define BUFFER_SIZE 12
#define SAMPLE_INTERVAL (CLOCK_CONF_SECOND / 2)
#define LOW_DEVIATION_THRESHOLD 10
#define HIGH_DEVIATION_THRESHOLD 20
#define READING_INTERVAL_K 12

int square_root_max_iterations = 20;
float square_root_precision = 0.001f;
float light_buffer[BUFFER_SIZE];
static int count = 0;

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

float getLight(void)
{
  float V_sensor = 1.5 * light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC)/4096;
                // ^ ADC-12 uses 1.5V_REF
  float I = V_sensor/100000;         // xm1000 uses 100kohm resistor
  float light_lx = 0.625*1e6*I*1000; // convert from current to light intensity
  return light_lx;
}

void printBuffer(void)
{
  int i = 0;
  printf("Buffer = ");

  for (i = 0; i < BUFFER_SIZE; i++) {
    printf("%d.%d ", d1(light_buffer[i]), d2(light_buffer[i]));
  }

  printf("\n");
}

void updateBuffer(void)
{
  float light = getLight();
  light_buffer[count] = light;
  count++;
}

float calculateSquareRoot(float value)
{
  // Calculate the square root using Babylonian method like seen in the labs, except that the initial value should be somewhat close to the real value, so let's use a heuristic sqrt value
  float difference = 0.0;
  float x = value;
  while (x > 100.0f) x *= 0.1f; // Approximate square root
  x *= 10.0f;
  int i = 0;
  for (i = 0; i < square_root_max_iterations; i++) {
    x = 0.5 * (x * value / x);
    difference = x * x - value;
    if (difference < 0) difference = -difference;
    if (difference < square_root_precision) break;
  }
  return x;
}

float calculateMean(float collection[], int start_index, int end_index)
{
  float total = 0;
  int value_count = end_index - start_index;

  int i = start_index;
  for (i = start_index; i < end_index; i++) {
    total += collection[i];
  }
  float mean = total / value_count;
  return mean;
}

float calculateStandardDeviation(float collection[], float mean, int start_index, int end_index)
{
  int value_count = end_index - start_index;
  float variance_sum = 0;

  int i = start_index;
  for (i = start_index; i < end_index; i++) {
    float deviation = collection[i] - mean;
    variance_sum += deviation * deviation;
  }
  float variance = variance_sum / value_count;
  float standard_deviation = calculateSquareRoot(variance);
  return standard_deviation;
}

void printAggregation(int degree)
{
  printf("Aggregation: %d-into-1.\n", degree);
}

/*---------------------------------------------------------------------------*/
PROCESS(coursework_process, "Coursework");
AUTOSTART_PROCESSES(&coursework_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(coursework_process, ev, data)
{
  static struct etimer timer; // Initialise a timer

  PROCESS_BEGIN();
  SENSORS_ACTIVATE(light_sensor); // Initialise the light sensor
  etimer_set(&timer, SAMPLE_INTERVAL); // Rate of 2 readings per second

  while(1) {
    // Wait for timer event to do anything
    PROCESS_WAIT_EVENT();

    if (ev == PROCESS_EVENT_TIMER) {
      updateBuffer();

      // If we have filled the buffer, we need to process
      if (count == BUFFER_SIZE) {
        // First print all the values as shown in the screenshot in the brief
        printBuffer();

        // Calculate mean of these values
        float mean = calculateMean(light_buffer, 0, BUFFER_SIZE);

        // Calculate standard deviation
        float standard_deviation = calculateStandardDeviation(light_buffer, mean, 0, BUFFER_SIZE);

        // Print these values as shown in the screenshot in brief
        printf("Standard deviation: %d.%d\n", d1(standard_deviation), d2(standard_deviation));

        // Print the aggregation technique
        printAggregation(12);
        printf("\n");

        count = 0;
      }

      // Restart the timer
      etimer_reset(&timer);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
