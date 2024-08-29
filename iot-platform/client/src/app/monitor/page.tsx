"use client";

import React, { useState, useEffect, useCallback } from "react";
import { Line } from "react-chartjs-2";
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
} from "chart.js";
import { CircularProgressbar, buildStyles } from "react-circular-progressbar";
import "react-circular-progressbar/dist/styles.css";
import DashboardLayout from "@/components/DashboardLayout";
import WebSocket from "@/components/WebSocket";
import { PacketData, Device } from "@/types";

ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend
);

const MAX_DATA_POINTS = 300;

export default function Monitor() {
  const [packetData, setPacketData] = useState<PacketData | null>(null);
  const [irData, setIrData] = useState<number[]>([]);
  const [redData, setRedData] = useState<number[]>([]);
  const [spo2Data, setSpo2Data] = useState<number[]>([]);
  const [heartRate, setHeartRate] = useState<number | null>(null);
  const [devices, setDevices] = useState<Device[]>([]);
  const [selectedDeviceId, setSelectedDeviceId] = useState<string>("");

  const fetchDevices = async () => {
    try {
      const response = await fetch(
        `${process.env.NEXT_PUBLIC_API_URL}/devices`,
        {
          headers: {
            Authorization: `Bearer ${localStorage.getItem("token")}`,
          },
        }
      );
      if (response.ok) {
        const data = await response.json();
        setDevices(data);
      } else {
        throw new Error("Failed to fetch devices");
      }
    } catch (error) {
      console.error("Failed to fetch devices:", error);
    }
  };

  useEffect(() => {
    fetchDevices();
  }, []);

  const handleNewPacketData = useCallback(
    (data: PacketData) => {
      if (data.deviceId !== selectedDeviceId) return;

      setPacketData(data);
      setHeartRate(data.heartRate);

      setIrData((prevData) => {
        const newData = [...prevData, ...data.irData];
        return newData.slice(-MAX_DATA_POINTS);
      });

      setRedData((prevData) => {
        const newData = [...prevData, ...data.redData];
        return newData.slice(-MAX_DATA_POINTS);
      });

      setSpo2Data((prevData) => {
        const newData = [...prevData, data.SpO2];
        return newData.slice(-MAX_DATA_POINTS);
      });
    },
    [selectedDeviceId]
  );

  const generateChartData = (
    data: number[],
    label: string,
    borderColor: string,
    backgroundColor: string
  ) => {
    const labels = data.map((_, index) => index + 1);

    return {
      labels: labels,
      datasets: [
        {
          label: label,
          data: data,
          borderColor: borderColor,
          backgroundColor: backgroundColor,
          pointRadius: 0,
          pointHoverRadius: 0,
        },
      ],
    };
  };

  const chartOptions = {
    responsive: true,
    maintainAspectRatio: false,
    plugins: {
      legend: {
        display: false,
      },
    },
    scales: {
      x: {
        ticks: {
          display: false,
        },
      },
    },
  };

  const irChartData = generateChartData(
    irData,
    "IR PPG",
    "rgba(255, 99, 132, 1)",
    "rgba(255, 99, 132, 0.2)"
  );
  const redChartData = generateChartData(
    redData,
    "Red PPG",
    "rgba(54, 162, 235, 1)",
    "rgba(54, 162, 235, 0.2)"
  );

  return (
    <DashboardLayout>
      <div className="space-y-6">
        <div className="flex gap-6">
          <div className="bg-white p-6 rounded-lg shadow-md flex-1">
            <h2 className="text-xl font-bold mb-4">Select Device</h2>
            <select
              className="w-full px-4 py-3 rounded-md border border-gray-300 focus:outline-none focus:ring-2 focus:ring-blue-500 text-lg"
              value={selectedDeviceId}
              onChange={(e) => setSelectedDeviceId(e.target.value)}
            >
              <option value="" disabled>
                Select a device
              </option>
              {devices.map((device) => (
                <option key={device.deviceId} value={device.deviceId}>
                  {device.deviceId}
                </option>
              ))}
            </select>
          </div>

          {packetData && selectedDeviceId && (
            <div className="bg-white p-6 rounded-lg shadow-md flex-1">
              <h2 className="text-xl font-bold mb-4">Device Information</h2>
              <p>
                <strong>Device ID:</strong> {packetData.deviceId}
              </p>
              <p>
                <strong>Data Index:</strong> {packetData.dataIndex}
              </p>
              <p>
                <strong>Timestamp:</strong>{" "}
                {packetData.timestamp
                  ? new Date(packetData.timestamp).toLocaleString()
                  : ""}
              </p>
            </div>
          )}
        </div>

        {packetData && selectedDeviceId && (
          <>
            <div className="bg-white p-6 rounded-lg shadow-md">
              <div className="grid grid-cols-2 gap-6">
                <div className="text-center">
                  <h3 className="text-lg font-semibold mb-2">SpO2</h3>
                  <div className="w-32 h-32 mx-auto">
                    <CircularProgressbar
                      value={packetData.SpO2}
                      text={`${packetData.SpO2.toFixed(2)}%`}
                      styles={buildStyles({
                        textColor: "#000",
                        pathColor: "#3e98c7",
                        trailColor: "#d6d6d6",
                      })}
                    />
                  </div>
                </div>
                <div className="text-center">
                  <h3 className="text-lg font-semibold mb-2">Heart Rate</h3>
                  <div className="w-32 h-32 mx-auto">
                    <CircularProgressbar
                      value={100}
                      text={`${heartRate || 0} BPM`}
                      styles={buildStyles({
                        textColor: "#000",
                        pathColor: "#ff6384",
                        trailColor: "#ffe5e5",
                      })}
                    />
                  </div>
                </div>
              </div>
            </div>

            <div className="bg-white p-6 rounded-lg shadow-md">
              <h3 className="text-lg font-semibold mb-2 text-center">
                IR Signal
              </h3>
              <div className="h-32">
                <Line data={irChartData} options={chartOptions} />
              </div>
            </div>

            <div className="bg-white p-6 rounded-lg shadow-md">
              <h3 className="text-lg font-semibold mb-2 text-center">
                Red Signal
              </h3>
              <div className="h-32">
                <Line data={redChartData} options={chartOptions} />
              </div>
            </div>
          </>
        )}
      </div>
      <WebSocket onNewPacketData={handleNewPacketData} isEnabled={true} />
    </DashboardLayout>
  );
}
