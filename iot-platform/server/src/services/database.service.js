const mongoose = require("mongoose");

class DatabaseService {
  constructor(databaseUrl) {
    this.databaseUrl = databaseUrl;
    this.connect();
  }

  connect() {
    mongoose.connect(this.databaseUrl, {
      useNewUrlParser: true,
      useUnifiedTopology: true,
    });

    const connection = mongoose.connection;
    connection.once("open", () => {
      console.log("Connected to MongoDB");
    });

    connection.on("error", (err) => {
      console.error(`Error connecting to MongoDB: ${err}`);
    });
  }

  disconnect() {
    mongoose.disconnect();
  }
}

module.exports = DatabaseService;
