#include "contiki.h"
#include "dev/light-sensor.h"
#include "dev/sht11-sensor.h"
#include <stdio.h>

// Initialise some of the constants we'll need
#define BUFFER_SIZE 12
#define SAMPLE_INTERVAL (CLOCK_CONF_SECOND / 2)
#define READING_INTERVAL_K 12

int square_root_max_iterations = 20;
float square_root_precision = 0.001f;
float low_deviation_threshold = 50.0f;
float high_deviation_threshold = 200.0f;
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

void printCollection(float collection[], int degree)
{
  int i = 0;
  printf("[ ");

  for (i = 0; i < degree; i++) {
    printf("%d.%d ", d1(collection[i]), d2(collection[i]));
  }

  printf("]\n");
}

void updateBuffer(void)
{
  float light = getLight();
  light_buffer[count] = light;
  count++;
}

float calculateSquareRoot(float value)
{
  if (value <= 0) return 0;

  // Calculate the square root using Babylonian method like seen in the labs, except that the initial value should be somewhat close to the real value, so let's use a heuristic sqrt value
  float difference = 0.0;
  float x = value / 2.0f;
  int i = 0;
  for (i = 0; i < square_root_max_iterations; i++) {
    float new_x = 0.5f * (x + value / x);
    difference = new_x - x;

    if (difference < square_root_precision && difference > -square_root_precision) break;

    x = new_x;
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
  float variance = variance_sum / (value_count - 1);
  float standard_deviation = calculateSquareRoot(variance);
  return standard_deviation;
}

int findAggregation(float std_dev)
{
  if (std_dev < low_deviation_threshold) {
    printf("Aggregation = 12-into-1.\n");
    return 12;
  }

  if (std_dev < high_deviation_threshold) {
    printf("Aggregation = 4-into-1.\n");
    return 4;
  }

  printf("No aggregation needed.\n");
  return 1;
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
        printf("Light buffer = ");
        printCollection(light_buffer, count);

        // Calculate mean of these values
        float mean = calculateMean(light_buffer, 0, count);

        // Calculate standard deviation
        float standard_deviation = calculateStandardDeviation(light_buffer, mean, 0, count);

        // Print these values as shown in the screenshot in brief
        printf("Standard deviation = %d.%d\n", d1(standard_deviation), d2(standard_deviation));

        // Find the aggregation from the standard deviation (returns 12, 4, 1)
        int degree = findAggregation(standard_deviation);

        // Calculate and print the aggregate array
        printf("Aggregate = ");

        if (degree == 1)
        {
          // 1-into-1 aggregation, so the array doesn't change
          printCollection(light_buffer, count);
        }

        if (degree == 4)
        {
          // 4-into-1 aggregation, array length is 4 and will contain the mean of each 3 values in light buffer
          float aggregate_buffer[degree];

          int i = 0;
          for (i = 0; i < degree; i++)
          {
            aggregate_buffer[i] = calculateMean(light_buffer, (i * degree), ((i + 1) * degree));
          }

          printCollection(aggregate_buffer, degree);
        }

        if (degree == 12)
        {
          // 12-into-1 aggregation, array returned is just one float value (mean of whole array)
          printf("%d.%d", d1(mean), d2(mean));
        }

        printf("\n\n");
        count = 0;
      }

      // Restart the timer
      etimer_reset(&timer);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
