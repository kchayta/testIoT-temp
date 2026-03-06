// Analog input pin numbers for X and Y axes
const int xAxisPin = A0;
const int yAxisPin = A1;

void setup() {
  // No setup needed for this example
}

void loop() {
  // Read the analog values from X and Y axes
  int xAxisValue = analogRead(xAxisPin);
  int yAxisValue = analogRead(yAxisPin);

  // Map the analog values to a range of -100 to 100 (optional)
  int mappedXValue = map(xAxisValue, 0, 1023, -100, 100);
  int mappedYValue = map(yAxisValue, 0, 1023, -100, 100);

  // Display the mapped values in the Serial Monitor
  Serial.print("X-axis: ");
  Serial.print(mappedXValue);
  Serial.print("\tY-axis: ");
  Serial.println(mappedYValue);

  // Add a small delay for smooth readings
  delay(100);
}