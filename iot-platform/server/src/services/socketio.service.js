const { Server } = require("socket.io");

class SocketIOService {
  constructor(server) {
    this.io = new Server(server, {
      cors: {
        origin: "*",
      },
    });

    this.io.on("connection", (socket) => {
      console.log("Client connected");

      socket.on("disconnect", () => {
        console.log("Client disconnected");
      });
    });

    console.log("Socket.IO server is running");
  }

  broadcast(event, message) {
    this.io.emit(event, message);
  }
}

module.exports = SocketIOService;
