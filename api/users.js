// Simple user management API for Vercel
export default function handler(request, response) {
  // Enable CORS
  response.setHeader('Access-Control-Allow-Origin', '*');
  response.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  response.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');
  
  if (request.method === 'OPTIONS') {
    response.status(200).end();
    return;
  }

  // Default users
  const defaultUsers = [
    { id: 1, name: "John Doe", status: "active", deviceModel: "iPhone 15 Pro" },
    { id: 2, name: "Jane Smith", status: "inactive", deviceModel: "Samsung Galaxy S24" },
    { id: 3, name: "Mike Johnson", status: "active", deviceModel: "Google Pixel 8" },
    { id: 4, name: "Sarah Wilson", status: "inactive", deviceModel: "OnePlus 12" }
  ];

  if (request.method === 'GET') {
    // Return all users
    response.status(200).json({
      success: true,
      data: defaultUsers,
      message: `Found ${defaultUsers.length} users`,
      count: defaultUsers.length
    });
    return;
  }

  if (request.method === 'POST') {
    try {
      const { action, user, timestamp } = request.body;
      
      console.log('POST request received:', {
        action,
        user,
        timestamp,
        source: 'API Request'
      });

      // For demo purposes, we'll just return success
      // In production, you would save to a database
      const responseData = {
        success: true,
        message: `Action '${action}' processed successfully`,
        timestamp: new Date().toISOString(),
        data: user,
        received: {
          action,
          user,
          timestamp,
          source: 'API Request'
        }
      };

      // Log the request
      console.log('User Management Request:', JSON.stringify(responseData, null, 2));

      response.status(200).json(responseData);
      
    } catch (error) {
      console.error('Error processing request:', error);
      response.status(500).json({
        success: false,
        error: 'Internal server error',
        message: error.message
      });
    }
    return;
  }

  response.status(405).json({ error: 'Method not allowed' });
}
