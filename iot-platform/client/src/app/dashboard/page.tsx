"use client";

import { useState, useEffect } from "react";
import { useRouter } from "next/navigation";
import { useAuth } from "@/hooks/useAuth";
import { User } from "@/types";
import UserInfo from "@/components/UserInfo";
import DashboardLayout from "@/components/DashboardLayout";
import DeviceRegistration from "@/components/DeviceRegistration";

export default function Dashboard() {
  const [user, setUser] = useState<User | null>(null);
  const router = useRouter();
  const { isAuthenticated, isLoading } = useAuth();

  useEffect(() => {
    const fetchUserData = async () => {
      if (isAuthenticated) {
        const token = localStorage.getItem("token");
        try {
          const response = await fetch(
            `${process.env.NEXT_PUBLIC_API_URL}/users/me`,
            {
              headers: {
                Authorization: `Bearer ${token}`,
              },
            }
          );
          if (response.ok) {
            const userData: User = await response.json();
            setUser(userData);
          } else {
            throw new Error("Failed to fetch user data");
          }
        } catch (error) {
          console.error("Error fetching user data:", error);
          localStorage.removeItem("token");
          router.push("/login");
        }
      }
    };

    fetchUserData();
  }, [isAuthenticated, router]);

  const handleLogout = () => {
    localStorage.removeItem("token");
    router.push("/login");
  };

  if (isLoading) {
    return <div>Loading...</div>;
  }

  if (!isAuthenticated) {
    return null; // Sẽ được chuyển hướng bởi useAuth hook
  }

  return (
    <DashboardLayout>
      <div className="space-y-8">
        <UserInfo user={user} onLogout={handleLogout} />
        <DeviceRegistration token={localStorage.getItem("token") || ""} />
      </div>
    </DashboardLayout>
  );
}
