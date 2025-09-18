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
  
  // Handle both GET and POST requests
  if (request.method !== 'GET' && request.method !== 'POST') {
    response.status(405).json({ error: 'Method not allowed' });
    return;
  }
  
  // Handle POST requests from Quectel BG95
  if (request.method === 'POST') {
    try {
      const { action, user, oldUser, timestamp } = request.body;
      
      console.log('POST request received from BG95:', {
        action,
        user,
        timestamp,
        source: 'Quectel BG95'
      });
      
      // Log the request for monitoring
      const logEntry = {
        timestamp: new Date().toISOString(),
        action: action || 'unknown',
        user: user || null,
        oldUser: oldUser || null,
        source: 'Quectel BG95',
        ip: request.headers['x-forwarded-for'] || request.connection.remoteAddress
      };
      
      // In a real application, you would save this to a database
      console.log('BG95 Request Log:', JSON.stringify(logEntry, null, 2));
      
      response.status(200).json({
        success: true,
        message: `Action '${action}' processed successfully`,
        timestamp: new Date().toISOString(),
        received: logEntry
      });
      
    } catch (error) {
      console.error('Error processing BG95 POST request:', error);
      response.status(500).json({
        success: false,
        error: 'Internal server error',
        message: 'Failed to process BG95 request'
      });
    }
    return;
  }
  
  // Handle GET requests (existing functionality)
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
