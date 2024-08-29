import React from "react";
import dynamic from "next/dynamic";

const Sidebar = dynamic(() => import("./Sidebar"), { ssr: false });

interface DashboardLayoutProps {
  children: React.ReactNode;
}

const DashboardLayout: React.FC<DashboardLayoutProps> = ({ children }) => {
  return (
    <div className="flex h-screen bg-gray-100">
      <Sidebar />
      <main className="flex-grow p-8 overflow-auto">{children}</main>
    </div>
  );
};

export default DashboardLayout;
