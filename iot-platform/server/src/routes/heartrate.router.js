const express = require("express");
const PacketRoutes = express.Router();

const { PacketData } = require("../models");

// Route to get heartrate data
PacketRoutes.get("/heartrate/:deviceId", async (req, res) => {
  try {
    const deviceId = parseInt(req.params.gatewayId);

    // Find the packet data for the given gatewayId
    const packetData = await PacketData.findOne({ deviceId });

    // If no packet data is found, return a 404 error
    if (!packetData) {
      return res
        .status(404)
        .json({ message: "No data found for this deviceId." });
    }

    // Extract the irData and redData from the packet data
    const data = packetData.data;
    const irData = data.map((item) => item.irData);
    const redData = data.map((item) => item.redData);

    // Return the irData and redData in the response
    res.status(200).json({ irData, redData });
  } catch (error) {
    console.error(error);
    res.status(500).json({ message: "Server error." });
  }
});

module.exports = { PacketRoutes };
