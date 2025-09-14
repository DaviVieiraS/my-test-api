// Simple POST request viewer that stores and displays requests
let requests = []; // In-memory storage (resets on server restart)

export default async function handler(req, res) {
  // Enable CORS
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET,POST,OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

  if (req.method === 'OPTIONS') {
    return res.status(204).end();
  }

  // Handle POST requests
  if (req.method === 'POST') {
    const requestData = {
      id: Date.now(),
      timestamp: new Date().toISOString(),
      method: req.method,
      headers: req.headers,
      body: req.body,
      query: req.query
    };
    
    requests.push(requestData);
    
    // Return simple success message
    return res.status(200).json({
      success: true,
      message: 'POST request received!',
      requestId: requestData.id,
      totalRequests: requests.length
    });
  }

  // Handle GET requests - show the request list
  if (req.method === 'GET') {
    const html = `
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <title>POST Request Viewer</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; background: #f5f5f5; }
        .container { max-width: 1200px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; }
        .header { text-align: center; margin-bottom: 30px; }
        .stats { background: #e3f2fd; padding: 15px; border-radius: 5px; margin-bottom: 20px; }
        .request { border: 1px solid #ddd; margin: 15px 0; padding: 15px; border-radius: 5px; background: #fafafa; }
        .request-header { background: #007cba; color: white; padding: 10px; margin: -15px -15px 15px -15px; border-radius: 5px 5px 0 0; }
        .request-id { font-weight: bold; }
        .timestamp { color: #666; font-size: 0.9em; }
        .body { background: #f8f9fa; padding: 10px; border-radius: 3px; margin-top: 10px; }
        .empty { text-align: center; color: #666; padding: 40px; }
        .clear-btn { background: #dc3545; color: white; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; margin: 10px; }
        .clear-btn:hover { background: #c82333; }
        pre { white-space: pre-wrap; word-wrap: break-word; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>📨 POST Request Viewer</h1>
            <p>Simple API to capture and view POST requests</p>
        </div>
        
        <div class="stats">
            <strong>Total Requests: ${requests.length}</strong>
            ${requests.length > 0 ? `<button class="clear-btn" onclick="clearRequests()">Clear All</button>` : ''}
        </div>

        ${requests.length === 0 ? 
          '<div class="empty"><h3>No POST requests yet</h3><p>Send a POST request to this endpoint to see it here!</p></div>' :
          requests.reverse().map(req => `
            <div class="request">
                <div class="request-header">
                    <span class="request-id">Request #${req.id}</span>
                    <span class="timestamp">${new Date(req.timestamp).toLocaleString()}</span>
                </div>
                <h4>Body:</h4>
                <div class="body">
                    <pre>${JSON.stringify(req.body, null, 2)}</pre>
                </div>
                <details>
                    <summary>Show Headers</summary>
                    <div class="body">
                        <pre>${JSON.stringify(req.headers, null, 2)}</pre>
                    </div>
                </details>
            </div>
          `).join('')
        }
    </div>

    <script>
        function clearRequests() {
            if (confirm('Are you sure you want to clear all requests?')) {
                fetch('/api/clear', { method: 'POST' })
                    .then(() => location.reload());
            }
        }
    </script>
</body>
</html>`;

    res.setHeader('Content-Type', 'text/html');
    return res.status(200).send(html);
  }

  // Method not allowed
  res.status(405).json({ error: 'Method not allowed' });
}
