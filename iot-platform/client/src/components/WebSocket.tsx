"use client";

import { useState, useEffect, useCallback } from "react";
import io, { Socket } from "socket.io-client";
import { PacketData } from "@/types";

interface WebSocketProps {
  onNewPacketData: (data: PacketData) => void;
  isEnabled: boolean;
}

function WebSocket({ onNewPacketData, isEnabled }: WebSocketProps) {
  const [socket, setSocket] = useState<Socket | null>(null);
  const [isConnected, setIsConnected] = useState(false);

  const connectSocket = useCallback(() => {
    const newSocket = io(
      process.env.NEXT_PUBLIC_WEBSOCKET_URL || "http://localhost:5000",
      {
        reconnectionAttempts: 5,
        reconnectionDelay: 1000,
      }
    );

    newSocket.on("connect", () => {
      console.log("Connected to Socket.IO server");
      setIsConnected(true);
    });

    newSocket.on("disconnect", () => {
      console.log("Disconnected from Socket.IO server");
      setIsConnected(false);
    });

    newSocket.on("connect_error", (error) => {
      console.error("Connection error:", error);
      setIsConnected(false);
    });

    newSocket.on("newPacketData", (data: PacketData) => {
      try {
        console.log("Received new packet data:", data);
        onNewPacketData(data);
      } catch (error) {
        console.error("Error processing packet data:", error);
      }
    });

    newSocket.on("error", (error) => {
      console.error("Socket error:", error);
    });

    setSocket(newSocket);

    return newSocket;
  }, [onNewPacketData]);

  useEffect(() => {
    let activeSocket: Socket | null = null;

    if (isEnabled) {
      activeSocket = connectSocket();
    }

    return () => {
      if (activeSocket) {
        console.log("Cleaning up socket connection");
        activeSocket.disconnect();
      }
    };
  }, [isEnabled, connectSocket]);

  // You can expose the connection status if needed
  return (
    <div style={{ display: "none" }}>
      WebSocket status: {isConnected ? "Connected" : "Disconnected"}
    </div>
  );
}

export default WebSocket;
