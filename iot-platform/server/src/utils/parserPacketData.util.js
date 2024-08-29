const ID_DEVICE_LEN = 32; // Thay thế giá trị thật của bạn
const DATA_LEN = 100; // Thay thế giá trị thật của bạn

// Tính toán PACKET_LEN dựa trên các hằng số đã cho
const PACKET_LEN = ID_DEVICE_LEN + 4 + DATA_LEN * 4 + DATA_LEN * 4;

function ParsePacketData(buffer) {
  if (buffer.length !== PACKET_LEN) {
    throw new Error(
      `Invalid packet length: expected ${PACKET_LEN}, got ${buffer.length}`
    );
  }

  const deviceId = buffer
    .slice(0, ID_DEVICE_LEN)
    .toString("utf-8")
    .replace(/\0/g, "");
  const dataIndex = buffer.readUInt32LE(ID_DEVICE_LEN);

  const irData = [];
  const redData = [];
  let offset = ID_DEVICE_LEN + 4;

  for (let i = 0; i < DATA_LEN; i++) {
    irData.push(buffer.readUInt32LE(offset));
    offset += 4;
  }

  for (let i = 0; i < DATA_LEN; i++) {
    redData.push(buffer.readUInt32LE(offset));
    offset += 4;
  }

  return { deviceId, dataIndex, irData, redData };
}

module.exports = ParsePacketData;
