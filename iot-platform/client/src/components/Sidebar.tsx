"use client";

import React from "react";
import Link from "next/link";
import Image from "next/image";
import { useRouter, usePathname } from "next/navigation";
import { FaThLarge, FaHeartbeat, FaSignOutAlt } from "react-icons/fa";
import { ClientSideWrapper } from "./ClientSideWrapper";

const Sidebar: React.FC = () => {
  const router = useRouter();
  const pathname = usePathname();

  const isActive = (path: string) => pathname === path;

  const handleLogout = () => {
    localStorage.removeItem("token");
    router.push("/login");
  };

  return (
    <div className="bg-white w-64 h-screen shadow-lg flex flex-col">
      <div className="p-4 border-b">
        <h1 className="text-xl font-bold flex items-center">
          <Image
            src="/logo.png"
            alt="IoT Project Logo"
            width={50}
            height={50}
            className="mr-2"
          />
          IoT Project
        </h1>
      </div>
      <nav className="flex-grow">
        <ul className="space-y-2 p-4">
          <li>
            <Link
              href="/dashboard"
              className={`flex items-center p-2 rounded ${
                isActive("/dashboard")
                  ? "bg-blue-100 text-blue-600"
                  : "hover:bg-gray-100"
              }`}
            >
              <FaThLarge className="mr-2" /> Dashboard
            </Link>
          </li>
          <li>
            <Link
              href="/monitor"
              className={`flex items-center p-2 rounded ${
                isActive("/monitor")
                  ? "bg-blue-100 text-blue-600"
                  : "hover:bg-gray-100"
              }`}
            >
              <FaHeartbeat className="mr-2" /> Monitor
            </Link>
          </li>
        </ul>
      </nav>
      <div className="p-4 border-t">
        <button
          onClick={handleLogout}
          className="w-full py-2 px-4 bg-red-500 text-white rounded hover:bg-red-600 flex items-center justify-center"
        >
          <FaSignOutAlt className="mr-2" /> Logout
        </button>
      </div>
    </div>
  );
};

export default function WrappedSidebar() {
  return (
    <ClientSideWrapper>
      <Sidebar />
    </ClientSideWrapper>
  );
}
