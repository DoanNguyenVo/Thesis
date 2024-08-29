// src/components/AuthForm.tsx
import React, { useState } from "react";
import Link from "next/link";

interface AuthFormProps {
  onSubmit: (email: string, password: string, name?: string) => void;
  isLogin: boolean;
  showLink?: boolean;
  linkHref?: string;
  linkText?: string;
}

const AuthForm: React.FC<AuthFormProps> = ({
  onSubmit,
  isLogin,
  showLink = true,
  linkHref = isLogin ? "/register" : "/login",
  linkText = isLogin
    ? "Don't have an account? Register"
    : "Already have an account? Login",
}) => {
  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");
  const [name, setName] = useState("");

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    if (isLogin) {
      onSubmit(email, password);
    } else {
      onSubmit(email, password, name);
    }
  };

  return (
    <form onSubmit={handleSubmit} className="space-y-4">
      {!isLogin && (
        <div>
          <label
            htmlFor="name"
            className="block text-sm font-medium text-gray-700"
          >
            Name
          </label>
          <input
            type="text"
            id="name"
            value={name}
            onChange={(e) => setName(e.target.value)}
            required
            className="mt-1 block w-full rounded-md border-gray-300 shadow-sm focus:border-indigo-500 focus:ring-indigo-500"
          />
        </div>
      )}
      <div>
        <label
          htmlFor="email"
          className="block text-sm font-medium text-gray-700"
        >
          Email
        </label>
        <input
          type="email"
          id="email"
          value={email}
          onChange={(e) => setEmail(e.target.value)}
          required
          className="mt-1 block w-full rounded-md border-gray-300 shadow-sm focus:border-indigo-500 focus:ring-indigo-500"
        />
      </div>
      <div>
        <label
          htmlFor="password"
          className="block text-sm font-medium text-gray-700"
        >
          Password
        </label>
        <input
          type="password"
          id="password"
          value={password}
          onChange={(e) => setPassword(e.target.value)}
          required
          className="mt-1 block w-full rounded-md border-gray-300 shadow-sm focus:border-indigo-500 focus:ring-indigo-500"
        />
      </div>
      <button
        type="submit"
        className="w-full flex justify-center py-2 px-4 border border-transparent rounded-md shadow-sm text-sm font-medium text-white bg-blue-600 hover:bg-blue-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-blue-500"
      >
        {isLogin ? "Login" : "Register"}
      </button>
      {showLink && (
        <div className="text-sm text-center mt-4">
          <Link
            href={linkHref}
            className="font-medium text-blue-600 hover:text-blue-500"
          >
            {linkText}
          </Link>
        </div>
      )}
    </form>
  );
};

export default AuthForm;
