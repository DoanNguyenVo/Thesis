function autocorrelation(data) {
  const n = data.length;
  const meanValue = mean(data);
  let result = new Array(n).fill(0);

  for (let lag = 0; lag < n; lag++) {
    for (let i = 0; i < n - lag; i++) {
      result[lag] += (data[i] - meanValue) * (data[i + lag] - meanValue);
    }
  }

  return result.map((val) => val / n);
}

function mean(data) {
  return data.reduce((sum, value) => sum + value, 0) / data.length;
}

function findPeaks(data, minPeakHeight, minPeakDistance) {
  let peaks = [];
  for (let i = 1; i < data.length - 1; i++) {
    if (
      data[i] > data[i - 1] &&
      data[i] > data[i + 1] &&
      data[i] >= minPeakHeight
    ) {
      if (
        peaks.length === 0 ||
        i - peaks[peaks.length - 1] >= minPeakDistance
      ) {
        peaks.push(i);
      }
    }
  }
  return peaks;
}

function CalculateHeartRate(irData, sampleRate = 100) {
  if (!Array.isArray(irData) || irData.length === 0) {
    throw new Error("Invalid IR data");
  }

  // Loại bỏ xu hướng tuyến tính
  const detrended = irData.map(
    (value, index) =>
      value -
      (irData[0] +
        ((irData[irData.length - 1] - irData[0]) * index) / (irData.length - 1))
  );

  // Tính autocorrelation
  const acf = autocorrelation(detrended);

  // Tìm các đỉnh trong hàm autocorrelation
  const minPeakHeight = 0.1 * Math.max(...acf);
  const minPeakDistance = 0.5 * sampleRate;
  const peaks = findPeaks(acf, minPeakHeight, minPeakDistance);

  if (peaks.length < 2) {
    return null; // Không đủ đỉnh để tính nhịp tim
  }

  // Tính khoảng cách trung bình giữa các đỉnh
  const avgPeakDistance =
    (peaks[peaks.length - 1] - peaks[0]) / (peaks.length - 1);

  // Tính nhịp tim (nhịp/phút)
  const heartRate = (60 * sampleRate) / avgPeakDistance;

  return Math.round(heartRate);
}

module.exports = CalculateHeartRate;
