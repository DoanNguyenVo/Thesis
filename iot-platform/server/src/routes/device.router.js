const express = require("express");
const { Device } = require("../models");
const authenticate = require("../middleware/authenticate");
const router = express.Router();

router.post("/register-device", authenticate, async (req, res) => {
  const { deviceId } = req.body;
  const userId = req.user.id;

  try {
    // Kiểm tra xem thiết bị đã được đăng ký chưa
    const existingDevice = await Device.findOne({ deviceId });
    if (!existingDevice) {
      return res.status(400).json({ error: "Device not found" });
    }

    // Kiểm tra xem thiết bị đã có userId chưa
    if (existingDevice.userId) {
      return res.status(400).json({ error: "Device has been registered" });
    }

    // Nếu thiết bị chưa có userId, cập nhật userId và lưu thiết bị
    const newDevice = await Device.findOneAndUpdate(
      { deviceId },
      { userId, status: "connected", registeredAt: new Date() },
      { new: true }
    );

    res.status(201).json(newDevice);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

router.get("/", authenticate, async (req, res) => {
  try {
    const devices = await Device.find({ userId: req.user.id });
    res.status(200).json(devices);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

router.delete("/:deviceId", authenticate, async (req, res) => {
  try {
    const device = await Device.findOne({
      deviceId: req.params.deviceId,
      userId: req.user.id,
    });
    if (!device) {
      return res.status(404).json({ error: "Device not found" });
    }

    // Xóa liên kết người dùng và thiết bị
    device.userId = null;
    await device.save();

    res.status(200).json({
      message: "Device disconnected and deleted successfully",
      device,
    });
  } catch (error) {
    // Nếu có lỗi (bao gồm cả timeout), cập nhật trạng thái thiết bị
    if (device) {
      device.status = "error";
      await device.save();
    }
    res.status(500).json({ error: error.message });
  }
});

module.exports = router;
