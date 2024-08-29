function calculateSpO2(irData, redData) {
  // Validate input data
  if (
    !Array.isArray(irData) ||
    !Array.isArray(redData) ||
    irData.length === 0 ||
    redData.length === 0
  ) {
    throw new Error("Invalid IR or Red data");
  }

  // Calculate average (DC component)
  const irAverage =
    irData.reduce((sum, value) => sum + value, 0) / irData.length;
  const redAverage =
    redData.reduce((sum, value) => sum + value, 0) / redData.length;

  // Calculate AC component (fluctuation)
  const irAcComponent = irData.map((value) => value - irAverage);
  const redAcComponent = redData.map((value) => value - redAverage);

  // Calculate RMS (Root Mean Square) of AC components
  const irAcRms = Math.sqrt(
    irAcComponent
      .map((value) => value * value)
      .reduce((sum, value) => sum + value, 0) / irAcComponent.length
  );
  const redAcRms = Math.sqrt(
    redAcComponent
      .map((value) => value * value)
      .reduce((sum, value) => sum + value, 0) / redAcComponent.length
  );

  // Calculate ratio R
  const ratioR = redAcRms / redAverage / (irAcRms / irAverage);

  // Calibration coefficients for MAX30101
  const calibrationA = 1.5958422;
  const calibrationB = -34.6596622;
  const calibrationC = 112.6898759;

  // Calculate SpO2
  const spO2 =
    calibrationA * ratioR * ratioR + calibrationB * ratioR + calibrationC;

  return spO2;
}

module.exports = calculateSpO2;
