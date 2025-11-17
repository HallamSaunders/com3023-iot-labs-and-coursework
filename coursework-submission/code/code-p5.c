void processCollection(float collection[])
{
  // Take in a collection of floats (e.g: light_buffer) and do the following:
  // 1. Print the whole collection.
  // 2. Calculate the standard deviation.
  // 3. Decide upon an aggregation degree based on that standard deviation.
  // 4. Perform aggregation and print.

  // Print buffer
  printf("B = ");
  printCollection(light_buffer, count);

  // Find standard deviation
  float mean = calculateMean(light_buffer, 0, count);
  float standard_deviation = calculateStandardDeviation(light_buffer, mean, 0, count);
  printf("StdDev = ");
  printFloat(standard_deviation);
  printf("\n");

  // Print that aggregate collection with averages calculated using mean
  int degree = findAggregation(standard_deviation);

  printf("X = ");

  if (degree == 1) printCollection(light_buffer, count); // For 1-into-1, the array doesn't change

  if (degree == 4) {
    // If we need 4-into-1, we need to aggregate every 4 values into one average
    int aggregate_count = BUFFER_SIZE / degree; // If we want to aggregate n values into 1, we will end up with (total / n) aggregate results

    float aggregate_buffer[aggregate_count];

    int i = 0;
    for (i = 0; i < aggregate_count; i++)
    {
      aggregate_buffer[i] = calculateMean(light_buffer, (i * degree), ((i + 1) * degree));
    }

    printCollection(aggregate_buffer, aggregate_count);
  }

  if (degree == 12) printFloat(mean); // For 12-into-1 the entire array is averaged, already did this earlier so we can save some computation by reusing 

  printf("\n\n");
}
