const { MQTTService } = require("../services");
const {
  ParsePacketData,
  CalculateSpO2,
  CalculateHeartRate,
} = require("../utils");
const { PacketData, Device } = require("../models");
const { default: mqtt } = require("mqtt");

async function getPreviousFrames(deviceId, frameCount) {
  const packet = await PacketData.findOne({ deviceId });
  if (packet) {
    const data = packet.data.slice(-frameCount); // Lấy các frame cuối cùng
    return data;
  }
  return [];
}

async function PacketDataController(mqttService, io) {
  mqttService.subscribe("#", async (topic, message) => {
    try {
      if (topic === "registration") {
        await handleRegistration(message.toString(), mqttService);
      } else if (topic.endsWith("ppg")) {
        await handlePPGData(message, io, mqttService);
      }
    } catch (error) {
      console.error("Error handling packet:", error);
    }
  });
}

async function handleRegistration(deviceId, mqttService) {
  console.log(`Handling registration for device: ${deviceId}`);
  const existingDevice = await Device.findOne({ deviceId });
  console.log("Existing device:", existingDevice);

  if (!existingDevice) {
    // Nếu thiết bị chưa được đăng ký, tạo mới thiết bị
    await Device.create({
      deviceId,
      status: "connected",
      userId: null,
    });
    console.log(`New device registered: ${deviceId}`);
  }

  // Nếu thiết bị đã có userId, gửi connected_ack
  if (existingDevice && existingDevice.userId) {
    mqttService.publish(`${deviceId}`, "connected_ack");
    console.log(`Sent connected_ack for device ${deviceId}`);
  }
}

async function handlePPGData(message, io, mqttService) {
  const packet = ParsePacketData(message);
  const existingDevice = await Device.findOne({
    deviceId: packet.deviceId,
    status: "connected",
  });

  if (!existingDevice) {
    console.log(
      `Device ${packet.deviceId} is not registered or not connected.`
    );

    // Tạo mới thiết bị nếu chưa tồn tại
    await Device.create({
      deviceId: packet.deviceId,
      status: "connected",
      userId: null,
    });

    if (existingDevice && existingDevice.userId) {
      mqttService.publish(`${deviceId}`, "connected_ack");
      console.log(`Sent connected_ack for device ${deviceId}`);
    }

    return;
  }

  const SpO2 = CalculateSpO2(packet.irData, packet.redData);
  const previousFrames = (await getPreviousFrames(packet.deviceId, 1)) || [];
  const combinedIrData = [
    ...previousFrames.flatMap((frame) => frame.irData),
    ...packet.irData,
  ];
  const heartRate = CalculateHeartRate(combinedIrData);

  await Device.findOneAndUpdate(
    { deviceId: packet.deviceId },
    { lastUpdated: new Date() },
    { new: true }
  );

  const newData = {
    dataIndex: packet.dataIndex,
    irData: packet.irData,
    redData: packet.redData,
    SpO2: SpO2,
    heartRate: heartRate,
    timestamp: new Date(),
  };

  await PacketData.findOneAndUpdate(
    { deviceId: packet.deviceId },
    { $push: { data: newData } },
    { upsert: true, new: true }
  );

  io.emit("newPacketData", { deviceId: packet.deviceId, ...newData });
}

module.exports = PacketDataController;
