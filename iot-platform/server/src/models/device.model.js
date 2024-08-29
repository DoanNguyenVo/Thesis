const mongoose = require("mongoose");

const deviceSchema = new mongoose.Schema({
  deviceId: {
    type: String,
    required: true,
    unique: true,
  },
  userId: {
    type: mongoose.Schema.Types.ObjectId,
    ref: "User",
  },
  status: {
    type: String,
    enum: ["connected", "disconnected"],
    required: true,
    default: "disconnected",
  },
  lastUpdated: {
    type: Date,
    default: Date.now,
  },
  registeredAt: {
    type: Date,
    default: Date.now,
  },
});

const Device = mongoose.model("Device", deviceSchema);

module.exports = Device;
