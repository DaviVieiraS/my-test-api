export default function handler(request, response) {
  // Enable CORS for all origins
  response.setHeader('Access-Control-Allow-Origin', '*');
  response.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  response.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');
  
  // Handle preflight requests
  if (request.method === 'OPTIONS') {
    response.status(200).end();
    return;
  }
  
  // Only allow GET requests
  if (request.method !== 'GET') {
    response.status(405).json({ error: 'Method not allowed' });
    return;
  }
  
  const { username } = request.query;
  
  if (!username) {
    response.status(400).json({ error: 'Username parameter is required' });
    return;
  }
  
  // Example user data - replace with your own logic
  const users = {
    john_doe: { 
      name: "John Doe", 
      email: "john@example.com", 
      role: "admin",
      status: "active",
      lastLogin: "2024-01-15T10:30:00Z",
      createdAt: "2023-06-01T00:00:00Z"
    },
    jane_smith: { 
      name: "Jane Smith", 
      email: "jane@example.com", 
      role: "user",
      status: "active",
      lastLogin: "2024-01-14T15:45:00Z",
      createdAt: "2023-08-15T00:00:00Z"
    },
    admin_user: {
      name: "Admin User",
      email: "admin@example.com",
      role: "super_admin",
      status: "active",
      lastLogin: "2024-01-15T09:15:00Z",
      createdAt: "2023-01-01T00:00:00Z"
    }
  };
  
  if (users[username]) {
    response.status(200).json({
      success: true,
      data: users[username],
      message: "User found successfully"
    });
  } else {
    response.status(404).json({ 
      success: false,
      error: "User not found",
      message: `No user found with username: ${username}`
    });
  }
}
