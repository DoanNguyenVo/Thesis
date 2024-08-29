import React, { useEffect, useState } from "react";
import { Device } from "@/types";
import Toast from "./Toast";

interface DeviceRegistrationProps {
  token: string;
}

const DeviceRegistration: React.FC<DeviceRegistrationProps> = ({ token }) => {
  const [devices, setDevices] = useState<Device[]>([]);
  const [deviceId, setDeviceId] = useState("");
  const [toast, setToast] = useState<{
    message: string;
    type: "success" | "error";
  } | null>(null);

  const fetchDevices = async () => {
    try {
      const response = await fetch(
        `${process.env.NEXT_PUBLIC_API_URL}/devices`,
        {
          headers: {
            Authorization: `Bearer ${token}`,
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
      setToast({ message: "Failed to fetch devices", type: "error" });
    }
  };

  useEffect(() => {
    fetchDevices();
  }, [token]);

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    try {
      const response = await fetch(
        `${process.env.NEXT_PUBLIC_API_URL}/devices/register-device`,
        {
          method: "POST",
          headers: {
            "Content-Type": "application/json",
            Authorization: `Bearer ${token}`,
          },
          body: JSON.stringify({ deviceId }),
        }
      );
      if (response.ok) {
        setDeviceId("");
        fetchDevices();
        setToast({
          message: "Device registered successfully",
          type: "success",
        });
      } else {
        throw new Error("Failed to register device");
      }
    } catch (error) {
      console.error("Failed to register device:", error);
      setToast({ message: "Failed to register device", type: "error" });
    }
  };

  const handleDelete = async (deviceId: string) => {
    try {
      const response = await fetch(
        `${process.env.NEXT_PUBLIC_API_URL}/devices/${deviceId}`,
        {
          method: "DELETE",
          headers: {
            Authorization: `Bearer ${token}`,
          },
        }
      );
      if (response.ok) {
        fetchDevices();
        setToast({ message: "Device deleted successfully", type: "success" });
      } else {
        throw new Error("Failed to delete device");
      }
    } catch (error) {
      console.error("Failed to delete device:", error);
      setToast({ message: "Failed to delete device", type: "error" });
    }
  };

  return (
    <div className="space-y-8">
      {toast && (
        <Toast
          message={toast.message}
          type={toast.type}
          onClose={() => setToast(null)}
        />
      )}

      <div className="bg-white p-8 rounded-lg shadow-md">
        <h2 className="text-2xl font-bold mb-6">Register Device</h2>
        <form onSubmit={handleSubmit} className="space-y-4">
          <div>
            <label
              htmlFor="deviceId"
              className="block text-lg font-medium text-gray-700 mb-2"
            >
              Device ID
            </label>
            <input
              type="text"
              id="deviceId"
              value={deviceId}
              onChange={(e) => setDeviceId(e.target.value)}
              required
              className="w-full px-4 py-3 rounded-md border border-gray-300 focus:outline-none focus:ring-2 focus:ring-blue-500 text-lg"
            />
          </div>
          <button
            type="submit"
            className="w-full bg-blue-500 hover:bg-blue-600 text-white font-bold py-3 px-4 rounded text-lg"
          >
            Register Device
          </button>
        </form>
      </div>

      <div className="bg-white p-8 rounded-lg shadow-md">
        <h2 className="text-2xl font-bold mb-6">Registered Devices</h2>
        <ul className="space-y-4">
          {devices.map((device) => (
            <li
              key={device._id}
              className="flex justify-between items-center border-b pb-4"
            >
              <div>
                <p className="font-medium text-lg">
                  Device ID: {device.deviceId}
                </p>
                <p className="text-gray-500">
                  Registered At:{" "}
                  {new Date(device.registeredAt).toLocaleString()}
                </p>
              </div>
              <button
                onClick={() => handleDelete(device.deviceId)}
                className="bg-red-500 hover:bg-red-600 text-white font-bold py-2 px-4 rounded text-lg"
              >
                Delete
              </button>
            </li>
          ))}
        </ul>
      </div>
    </div>
  );
};

export default DeviceRegistration;
