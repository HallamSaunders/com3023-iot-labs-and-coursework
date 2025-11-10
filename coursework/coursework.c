#include "contiki.h"
#include "dev/light-sensor.h"
#include "dev/sht11-sensor.h"
#include <stdio.h>

// Initialise some of the constants we'll need
#define BUFFER_SIZE 12
#define SAMPLE_INTERVAL (CLOCK_CONF_SECOND / 2)

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

/*---------------------------------------------------------------------------*/
PROCESS(coursework_process, "Coursework");
AUTOSTART_PROCESSES(&coursework_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(coursework_process, ev, data)
{
  static struct etimer timer; // Initialise a timer
  static float light_buffer[BUFFER_SIZE];
  static int count = 0; // Count current amount in buffer
  static int head = 0;

  PROCESS_BEGIN();
  SENSORS_ACTIVATE(light_sensor); // Initialise the light sensor
  etimer_set(&timer, SAMPLE_INTERVAL); // Rate of 2 readings per second

  while(1) {
    // Wait for timer event to do anything
    PROCESS_WAIT_EVENT();

    if (ev == PROCESS_EVENT_TIMER) {

      // Get and store the current light value
      float light = getLight();
      light_buffer[head] = light;

      // Update head value modulo the buffer size (make it generic)
      head = (head + 1) & BUFFER_SIZE;

      // Update count
      if (count < BUFFER_SIZE) {
        count++;
      }

      printf("Stored light val %d.%d\nBuffer count is %d\n\n", d1(light), d2(light), count);

      // Restart the timer
      etimer_reset(&timer);
    }
  }
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
