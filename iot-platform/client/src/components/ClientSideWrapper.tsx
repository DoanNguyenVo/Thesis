"use client";

import { useRouter } from "next/navigation";
import { PropsWithChildren } from "react";

export function ClientSideWrapper({ children }: PropsWithChildren) {
  useRouter(); // Để đảm bảo router được khởi tạo
  return <>{children}</>;
}
