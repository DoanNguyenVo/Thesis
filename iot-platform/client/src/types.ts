// src/types.ts

export interface User {
  _id: string;
  name: string;
  email: string;
  createdAt: string;
}
  
export interface AuthResponse {
  token: string;
}

export interface Device {
  _id: string;
  deviceId: string;
  userId: string;
  status: 'connected' | 'disconnected';
  lastUpdated: string;
  registeredAt: string;
}

export interface PacketData {
  deviceId: string;
  dataIndex: number;
  irData: number[];
  redData: number[];
  SpO2: number;
  heartRate: number;
  timestamp?: Date;
}