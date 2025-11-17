// ===== UTILITY FUNCTIONS =====
int findAggregation(float std_dev)
{
  // From the standard deviation, find which of the aggregation values should be used
  // The specification defines three types of aggregation: every 12 values (full), every 4, and every 1 (no aggregation)
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
