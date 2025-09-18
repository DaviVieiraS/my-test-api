export default function handler(req, res) {
  // Enable CORS for all origins
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');
  
  // Handle preflight requests
  if (req.method === 'OPTIONS') {
    res.status(200).end();
    return;
  }
  
  // Only allow POST requests
  if (req.method !== 'POST') {
    res.status(405).json({ error: 'Method not allowed' });
    return;
  }
  
  const { username, productId, status } = req.body;
  
  // Validate required fields
  if (!username || !productId || !status) {
    res.status(400).json({ 
      success: false,
      error: 'Missing required fields',
      message: 'Username, productId, and status are required'
    });
    return;
  }
  
  // Validate status values
  const validStatuses = ['active', 'inactive', 'pending', 'discontinued'];
  if (!validStatuses.includes(status)) {
    res.status(400).json({
      success: false,
      error: 'Invalid status',
      message: `Status must be one of: ${validStatuses.join(', ')}`
    });
    return;
  }
  
  // Simulate database operation with some randomness for demo
  const success = Math.random() > 0.1; // 90% success rate for demo
  
  if (success) {
    // Simulate processing time
    const processingTime = Math.floor(Math.random() * 100) + 50; // 50-150ms
    
    res.status(200).json({ 
      success: true,
      message: "Product status updated successfully",
      data: {
        productId,
        status,
        updatedBy: username,
        updatedAt: new Date().toISOString(),
        processingTime: `${processingTime}ms`
      }
    });
  } else {
    res.status(500).json({ 
      success: false,
      error: "Failed to update product status",
      message: "Database connection error. Please try again later."
    });
  }
}
