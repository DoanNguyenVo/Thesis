require("dotenv").config();

const cors = require("cors");

const express = require("express");
const http = require("http");

const { userRouter, deviceRouter } = require("./routes");

const { MQTTService, DatabaseService, SocketIOService } = require("./services");

const { PacketDataController } = require("./controllers");

// Initialize Server
const app = express();
const server = http.createServer(app);
app.use(express.json());

// Connect to MongoDB
const databaseService = new DatabaseService(process.env.MONGODB_URI);

// Connect to MQTT Broker
const mqttService = new MQTTService(process.env.MQTT_BROKER_URL, {
  username: process.env.MQTT_USERNAME,
  password: process.env.MQTT_PASSWORD,
});

app.set("mqttService", mqttService);

// Start Socket.IO server
const socketIOService = new SocketIOService(server);

PacketDataController(mqttService, socketIOService.io);

// Sử dụng CORS
app.use(
  cors({
    origin: "http://localhost:3000", // URL frontend của bạn
    methods: "GET,HEAD,PUT,PATCH,POST,DELETE",
    credentials: true,
  })
);

app.use("/api/users", userRouter);
app.use("/api/devices", deviceRouter);

app.get("/", (req, res) => {
  res.send("Server is running");
});

const PORT = process.env.PORT || 5000;
server.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});
