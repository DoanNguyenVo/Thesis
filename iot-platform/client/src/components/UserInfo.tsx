import { User } from "@/types";

interface UserInfoProps {
  user: User | null;
  onLogout: () => void;
}

const UserInfo = ({ user, onLogout }: UserInfoProps) => {
  if (!user) return null;

  return (
    <div className="bg-white shadow-md rounded-lg p-6 mb-8">
      <div className="flex items-center mb-4">
        <div className="w-20 h-20 bg-gray-300 rounded-full flex items-center justify-center mr-4">
          <svg
            className="w-12 h-12 text-gray-600"
            fill="none"
            stroke="currentColor"
            viewBox="0 0 24 24"
            xmlns="http://www.w3.org/2000/svg"
          >
            <path
              strokeLinecap="round"
              strokeLinejoin="round"
              strokeWidth={2}
              d="M16 7a4 4 0 11-8 0 4 4 0 018 0zM12 14a7 7 0 00-7 7h14a7 7 0 00-7-7z"
            />
          </svg>
        </div>
        <div>
          <h2 className="text-2xl font-bold">User Info</h2>
          <p className="text-lg">
            <span className="font-medium">Name:</span> {user.name}
          </p>
          <p className="text-lg">
            <span className="font-medium">Email:</span> {user.email}
          </p>
          <p className="text-lg">
            <span className="font-medium">Created At:</span>{" "}
            {new Date(user.createdAt).toLocaleString()}
          </p>
        </div>
      </div>
      <button
        onClick={onLogout}
        className="w-full bg-blue-500 hover:bg-blue-600 text-white font-bold py-3 px-4 rounded text-lg"
      >
        Logout
      </button>
    </div>
  );
};

export default UserInfo;
