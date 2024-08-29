// src/hooks/useAuth.ts
import { useState, useEffect } from 'react';
import { useRouter } from 'next/navigation';

export const useAuth = () => {
  const [isAuthenticated, setIsAuthenticated] = useState(false);
  const [isLoading, setIsLoading] = useState(true);
  const router = useRouter();

  useEffect(() => {
    const checkAuth = async () => {
      const token = localStorage.getItem('token');
      if (!token) {
        setIsAuthenticated(false);
        router.push('/login');
      } else {
        try {
          const response = await fetch(`${process.env.NEXT_PUBLIC_API_URL}/users/me`, {
            headers: {
              'Authorization': `Bearer ${token}`,
            },
          });
          if (response.ok) {
            setIsAuthenticated(true);
          } else {
            setIsAuthenticated(false);
            localStorage.removeItem('token');
            router.push('/login');
          }
        } catch (error) {
          console.error('Auth check failed:', error);
          setIsAuthenticated(false);
          localStorage.removeItem('token');
          router.push('/login');
        }
      }
      setIsLoading(false);
    };

    checkAuth();
  }, [router]);

  return { isAuthenticated, isLoading };
};