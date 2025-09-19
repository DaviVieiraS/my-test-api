// User management API for Vercel with persistent storage
// In production, use a real database like MongoDB, PostgreSQL, etc.

// Global user storage (resets on serverless function restart)
let users = [
  { id: 1, name: "John Doe", status: "active", deviceModel: "iPhone 15 Pro" },
  { id: 2, name: "Jane Smith", status: "inactive", deviceModel: "Samsung Galaxy S24" },
  { id: 3, name: "Mike Johnson", status: "active", deviceModel: "Google Pixel 8" },
  { id: 4, name: "Sarah Wilson", status: "inactive", deviceModel: "OnePlus 12" }
];

let nextId = 5;

export default function handler(request, response) {
  // Enable CORS
  response.setHeader('Access-Control-Allow-Origin', '*');
  response.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  response.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization, User-Agent');
  
  // Enhanced logging for debugging
  console.log('=== API REQUEST RECEIVED ===');
  console.log('Method:', request.method);
  console.log('URL:', request.url);
  console.log('Headers:', JSON.stringify(request.headers, null, 2));
  console.log('Body:', JSON.stringify(request.body, null, 2));
  console.log('================================');
  
  if (request.method === 'OPTIONS') {
    console.log('Handling OPTIONS request');
    response.status(200).end();
    return;
  }

  if (request.method === 'GET') {
    // Return all users
    response.status(200).json({
      success: true,
      data: users,
      message: `Found ${users.length} users`,
      count: users.length
    });
    return;
  }

  if (request.method === 'POST') {
    try {
      console.log('Processing POST request...');
      
      // Validate request body exists
      if (!request.body) {
        throw new Error('Request body is missing or empty');
      }
      
      const { action, user, timestamp } = request.body;
      
      console.log('POST request received:', {
        action,
        user,
        timestamp,
        source: 'API Request',
        bodyType: typeof request.body,
        bodyKeys: Object.keys(request.body || {})
      });

      // Validate required fields
      if (!action) {
        throw new Error('Missing required field: action');
      }
      
      if (!user) {
        throw new Error('Missing required field: user');
      }

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
            result = { 
              message: `User '${newUser.name}' added successfully`, 
              user: newUser,
              allUsers: users
            };
            console.log('User added:', newUser);
            console.log('Total users now:', users.length);
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
              result = { 
                message: `User '${users[userIndex].name}' updated successfully`, 
                user: users[userIndex], 
                oldUser: oldUserData,
                allUsers: users
              };
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
              result = { 
                message: `User '${deletedUser.name}' deleted successfully`, 
                user: deletedUser,
                allUsers: users
              };
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

      const responseData = {
        success: true,
        message: result.message,
        timestamp: new Date().toISOString(),
        data: result.user,
        allUsers: result.allUsers || users,
        count: users.length
      };

      // Log the request
      console.log('User Management Request:', JSON.stringify(responseData, null, 2));

      response.status(200).json(responseData);
      
    } catch (error) {
      console.error('=== ERROR PROCESSING REQUEST ===');
      console.error('Error message:', error.message);
      console.error('Error stack:', error.stack);
      console.error('Request body:', JSON.stringify(request.body, null, 2));
      console.error('================================');
      
      response.status(500).json({
        success: false,
        error: 'Internal server error',
        message: error.message,
        timestamp: new Date().toISOString()
      });
    }
    return;
  }

  console.log('Method not allowed:', request.method);
  response.status(405).json({ 
    error: 'Method not allowed',
    method: request.method,
    allowedMethods: ['GET', 'POST', 'OPTIONS']
  });
}
