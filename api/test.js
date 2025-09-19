export default function handler(request, response) {
  // Enable CORS
  response.setHeader('Access-Control-Allow-Origin', '*');
  response.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  response.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');
  
  if (request.method === 'OPTIONS') {
    response.status(200).end();
    return;
  }

  if (request.method === 'GET') {
    response.status(200).json({
      success: true,
      message: 'API is working!',
      timestamp: new Date().toISOString()
    });
    return;
  }

  if (request.method === 'POST') {
    const { action, user } = request.body;
    
    response.status(200).json({
      success: true,
      message: `Action '${action}' received successfully`,
      data: user,
      timestamp: new Date().toISOString()
    });
    return;
  }

  response.status(405).json({ error: 'Method not allowed' });
}
