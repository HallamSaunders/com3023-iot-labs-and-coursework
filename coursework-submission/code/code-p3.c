// ===== CALCULATIONS =====
float calculateSquareRoot(float value)
{
  if (value <= 0) return 0; // Catch-all for negatives or 0

  // Calculate the square root using Babylonian method like seen in the labs
  // Initial value should be somewhat close to the real value (value/2)
  float difference = 0.0;
  float x = value / 2.0f;
  int i = 0;
  for (i = 0; i < square_root_max_iterations; i++) {
    float new_x = 0.5f * (x + value / x);
    difference = new_x - x;

    // If 
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
  // Calculate the standard deviation using the functions I defined above
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
