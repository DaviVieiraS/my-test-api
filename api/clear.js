// Clear requests endpoint
let requests = []; // Shared with post-viewer.js (in real app, use database)

export default function handler(req, res) {
  if (req.method === 'POST') {
    requests = []; // Clear all requests
    res.status(200).json({ success: true, message: 'All requests cleared' });
  } else {
    res.status(405).json({ error: 'Method not allowed' });
  }
}
