// Simple test function to verify Vercel deployment
export default function handler(req, res) {
  res.status(200).json({
    message: 'Hello from Vercel!',
    method: req.method,
    url: req.url,
    timestamp: new Date().toISOString()
  });
}
