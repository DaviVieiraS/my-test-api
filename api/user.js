// Default users data (in production, use a database)
const defaultUsers = [
  {
    id: 1,
    name: "John Doe",
    status: "active",
    deviceModel: "iPhone 15 Pro"
  },
  {
    id: 2,
    name: "Jane Smith", 
    status: "inactive",
    deviceModel: "Samsung Galaxy S24"
  },
  {
    id: 3,
    name: "Mike Johnson",
    status: "active", 
    deviceModel: "Google Pixel 8"
  },
  {
    id: 4,
    name: "Sarah Wilson",
    status: "inactive",
    deviceModel: "OnePlus 12"
  }
];

// For demo purposes, we'll simulate user storage
// In production, use a real database
let users = [...defaultUsers];
let nextId = 5;

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
      
      let result = {};
      
      // Process the action
      switch (action) {
        case 'add':
          if (user && user.name && user.deviceModel) {
            const newUser = {
              id: user.id || nextId++,
              name: user.name,
              status: user.status || 'active',
              deviceModel: user.deviceModel
            };
            users.push(newUser);
            result = { message: `User '${newUser.name}' added successfully`, user: newUser };
          } else {
            throw new Error('Missing required fields: name and deviceModel');
          }
          break;
          
        case 'update':
          if (user && user.id) {
            const userIndex = users.findIndex(u => u.id === user.id);
            if (userIndex !== -1) {
              const oldUserData = { ...users[userIndex] };
              if (user.name) users[userIndex].name = user.name;
              if (user.status) users[userIndex].status = user.status;
              if (user.deviceModel) users[userIndex].deviceModel = user.deviceModel;
              result = { message: `User '${users[userIndex].name}' updated successfully`, user: users[userIndex], oldUser: oldUserData };
            } else {
              throw new Error(`User with ID ${user.id} not found`);
            }
          } else {
            throw new Error('Missing required field: user ID');
          }
          break;
          
        case 'delete':
          if (user && user.id) {
            const userIndex = users.findIndex(u => u.id === user.id);
            if (userIndex !== -1) {
              const deletedUser = users.splice(userIndex, 1)[0];
              result = { message: `User '${deletedUser.name}' deleted successfully`, user: deletedUser };
            } else {
              throw new Error(`User with ID ${user.id} not found`);
            }
          } else {
            throw new Error('Missing required field: user ID');
          }
          break;
          
        default:
          throw new Error(`Unknown action: ${action}`);
      }
      
      // Log the request for monitoring
      const logEntry = {
        timestamp: new Date().toISOString(),
        action: action || 'unknown',
        user: user || null,
        oldUser: oldUser || null,
        source: 'Quectel BG95',
        ip: request.headers['x-forwarded-for'] || request.connection.remoteAddress,
        result: result
      };
      
      console.log('BG95 Request Log:', JSON.stringify(logEntry, null, 2));
      
      response.status(200).json({
        success: true,
        message: result.message,
        timestamp: new Date().toISOString(),
        data: result.user,
        allUsers: users
      });
      
    } catch (error) {
      console.error('Error processing BG95 POST request:', error);
      response.status(500).json({
        success: false,
        error: 'Internal server error',
        message: error.message
      });
    }
    return;
  }
  
  // Handle GET requests
  const { username, all } = request.query;
  
  // Return all users if 'all' parameter is present
  if (all === 'true') {
    response.status(200).json({
      success: true,
      data: users,
      message: `Found ${users.length} users`,
      count: users.length
    });
    return;
  }
  
  // Return specific user by username (legacy functionality)
  if (username) {
    const user = users.find(u => u.name.toLowerCase().replace(/\s+/g, '_') === username.toLowerCase());
    if (user) {
      response.status(200).json({
        success: true,
        data: user,
        message: "User found successfully"
      });
    } else {
      response.status(404).json({ 
        success: false,
        error: "User not found",
        message: `No user found with username: ${username}`
      });
    }
    return;
  }
  
  // Default: return all users
  response.status(200).json({
    success: true,
    data: users,
    message: `Found ${users.length} users`,
    count: users.length
  });
}
