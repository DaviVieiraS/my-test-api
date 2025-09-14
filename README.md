# Test API - Request Viewer

A simple serverless function that displays incoming HTTP requests (method, query parameters, headers, and body) in a formatted HTML page. Perfect for testing APIs, webhooks, and debugging requests.

## Features

- ✅ Handles GET and POST requests
- ✅ Shows query parameters, headers, and request body
- ✅ CORS enabled for cross-origin testing
- ✅ Returns formatted HTML page
- ✅ Works with JSON, form data, and raw text
- ✅ Deploys easily to Vercel

## Quick Deploy to Vercel

### Option 1: GitHub + Vercel (Recommended)

1. **Create GitHub repository:**
   ```bash
   # If you have GitHub CLI
   gh repo create my-test-api --public --source=. --remote=origin --push
   
   # Or manually create on github.com and push:
   git remote add origin https://github.com/YOUR_USERNAME/my-test-api.git
   git push -u origin main
   ```

2. **Deploy to Vercel:**
   - Go to [vercel.com](https://vercel.com)
   - Sign up/Login and connect GitHub
   - Click "New Project" → "Import Git Repository"
   - Select `my-test-api`
   - Framework: Other/None
   - Click "Deploy"

3. **Your API will be available at:**
   ```
   https://my-test-api-XXXX.vercel.app/api/handler
   ```

### Option 2: Direct Deploy

1. Install Vercel CLI: `npm i -g vercel`
2. Run: `vercel` (follow prompts)
3. Your API will be live immediately

## Testing Your API

### GET Request (with query parameters)
```bash
curl "https://YOUR_PROJECT.vercel.app/api/handler?name=Davi&test=1"
```

Or open in browser to see the formatted HTML response.

### POST JSON
```bash
curl -X POST "https://YOUR_PROJECT.vercel.app/api/handler" \
  -H "Content-Type: application/json" \
  -d '{"motor":"ok","temp":32}'
```

### POST Form Data
```bash
curl -X POST -d "name=Davi&note=hello" "https://YOUR_PROJECT.vercel.app/api/handler"
```

### HTML Form (for browser testing)
Create a local HTML file:
```html
<form action="https://YOUR_PROJECT.vercel.app/api/handler" method="post">
  <input name="name" value="Davi" />
  <textarea name="note">hello</textarea>
  <button type="submit">Send</button>
</form>
```

## Project Structure

```
my-test-api/
├── api/
│   └── handler.js    # Serverless function
└── README.md
```

## How It Works

- **File Location**: `api/handler.js` - Vercel automatically maps `/api/*` to files in the `api/` folder
- **Function Export**: Uses ES6 `export default` for Vercel compatibility
- **CORS**: Enabled for cross-origin requests
- **Body Parsing**: Handles JSON, form data, and raw text automatically
- **HTML Response**: Returns formatted HTML showing all request data

## Troubleshooting

### 404 Error
- Ensure file is named `api/handler.js` (not `api/handler.ts` or other extensions)
- Check that deployment completed successfully in Vercel dashboard

### Empty Body
- Make sure to send `Content-Type: application/json` for JSON requests
- For form data, use `application/x-www-form-urlencoded`

### CORS Issues
- The function sets `Access-Control-Allow-Origin: *` for all origins
- If still having issues, check browser dev tools for preflight errors

## Security Notes

⚠️ **This endpoint echoes all data back** - don't send sensitive information!

To secure it:
1. Add API key validation in the handler
2. Limit to specific origins
3. Add rate limiting

## Next Steps

- Add database storage for request history
- Implement authentication
- Add webhook testing features
- Support file uploads

## License

MIT
