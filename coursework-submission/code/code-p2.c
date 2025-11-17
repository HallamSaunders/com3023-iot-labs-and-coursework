void printCollection(float collection[], int degree)
{
  // Iterate through the given collection of floats (e.g light_buffer) and print it using the above function
  printf("[ ");
  int i = 0;
  for (i = 0; i < degree; i++) {
    printFloat(collection[i]);
    printf(", ");
  }
  printf("]\n");
}

// ===== MEASUREMENTS =====
float getLight(void)
{
  float V_sensor = 1.5 * light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC)/4096;
                // ^ ADC-12 uses 1.5V_REF
  float I = V_sensor/100000;         // xm1000 uses 100kohm resistor
  float light_lx = 0.625*1e6*I*1000; // convert from current to light intensity
  return light_lx;
}

// ===== BUFFER MANAGEMENT =====
void updateBuffer(void)
{
  // Record a new value to the buffer at the current count position
  float light = getLight();
  light_buffer[count] = light;
  count++;
}
