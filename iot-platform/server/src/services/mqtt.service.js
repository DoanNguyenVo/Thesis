const mqtt = require("mqtt");
const { Device } = require("../models");

class MQTTService {
  constructor(brokerUrl, options) {
    this.client = mqtt.connect(brokerUrl, options);
    this.client.on("connect", () => {
      console.log("Connected to MQTT broker");
    });
  }

  subscribe(topic, callback) {
    this.client.subscribe(topic, (err) => {
      if (err) {
        console.error(`Error subscribing to ${topic}`);
      } else {
        console.log(`Subscribed to ${topic}`);
      }
    });

    this.client.on("message", callback);
  }

  publish(topic, message) {
    this.client.publish(topic, message, (err) => {
      if (err) {
        console.error(`Error publishing to ${topic}`);
      } else {
        console.log(`Published to ${topic}`);
      }
    });
  }
}

module.exports = MQTTService;
