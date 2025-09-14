// api/handler.js
// Vercel / Next-style serverless function that returns an HTML page
// showing the incoming request (GET query, POST body, headers).
export default async function handler(req, res) {
  // CORS: allow cross-origin testing from browser/dev tools
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET,POST,OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

  if (req.method === 'OPTIONS') {
    // required for browser preflight
    return res.status(204).end();
  }

  const method = req.method || '';
  const query = req.query || {};
  const headers = req.headers || {};

  // Try to get a useful body (works for JSON, form urlencoded, raw text)
  let body = '';
  if (method === 'POST' || method === 'PUT' || method === 'PATCH') {
    if (req.body && Object.keys(req.body).length !== 0) {
      body = req.body;
    } else {
      // read raw body fallback
      body = await readRawBody(req);
      try { body = JSON.parse(body); } catch (e) { /* leave as string */ }
    }
  }

  const html = buildHtml(method, query, headers, body);
  res.setHeader('Content-Type', 'text/html; charset=utf-8');
  res.status(200).send(html);
}

function readRawBody(req) {
  return new Promise((resolve) => {
    let data = '';
    req.on('data', chunk => (data += chunk));
    req.on('end', () => resolve(data));
    req.on('error', () => resolve(''));
  });
}

function buildHtml(method, query, headers, body) {
  const q = JSON.stringify(query, null, 2);
  const h = JSON.stringify(headers, null, 2);
  const b = (typeof body === 'object') ? JSON.stringify(body, null, 2) : String(body || '');
  return `<!doctype html>
<html>
<head><meta charset="utf-8"><title>Request viewer</title>
<style>body{font-family:system-ui,Segoe UI,Roboto,Arial;margin:20px}pre{background:#f5f5f5;padding:12px;border-radius:6px}</style>
</head>
<body>
  <h1>Request viewer</h1>
  <p><strong>Method:</strong> ${escapeHtml(method)}</p>
  <h2>Query</h2><pre>${escapeHtml(q)}</pre>
  <h2>Headers</h2><pre>${escapeHtml(h)}</pre>
  <h2>Body</h2><pre>${escapeHtml(b)}</pre>
  <hr>
  <p>Tip: use <code>curl</code>, Postman or an HTML form to hit this URL.</p>
</body>
</html>`;
}

function escapeHtml(s) {
  return String(s)
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;');
}
