const mongoose = require("mongoose");

const packetSchema = new mongoose.Schema({
  deviceId: {
    type: String,
    required: true,
    unique: true,
  },
  data: [
    {
      dataIndex: Number,
      irData: [Number],
      redData: [Number],
      SpO2: Number,
      heartRate: Number,
      timestamp: { type: Date, default: Date.now },
    },
  ],
});

const PacketData = mongoose.model("Packet", packetSchema);

module.exports = PacketData;
