# 🚀 User & Product Status API Tester

A complete API testing solution with both frontend interface and Vercel serverless backend. Test user information retrieval via GET requests and product status updates via POST requests.

## 🌐 Live Demo

Visit: `https://davivieiras.github.io/my-test-api/`

## 🚀 Features

- ✅ **API Testing Interface** - Beautiful, responsive UI for testing APIs
- ✅ **GET Endpoint Testing** - Check user information with username queries
- ✅ **POST Endpoint Testing** - Update product status with form data
- ✅ **Vercel Backend** - Serverless functions for real API endpoints
- ✅ **CORS Support** - Properly configured for cross-origin requests
- ✅ **Error Handling** - Comprehensive error messages and validation
- ✅ **Real-time Responses** - Live API response display with syntax highlighting
- ✅ **Multiple Deployment Options** - Works with Vercel, GitHub Pages, or local hosting

## 📋 Files

- **`api-tester.html`** - Main API testing interface
- **`api/user.js`** - Vercel serverless function for GET requests
- **`api/product.js`** - Vercel serverless function for POST requests
- **`vercel.json`** - Vercel deployment configuration
- **`index.html`** - Landing page with system overview
- **`dashboard.html`** - User management dashboard
- **`api-simulator.html`** - API endpoint simulator for user data
- **`post-viewer.html`** - View all POST requests in a list
- **`post-form.html`** - Test form to send POST requests

## 🎯 How to Use

### Frontend Testing
1. **Open `api-tester.html`** in your browser
2. **Check User Tab**: Enter username and test GET endpoint
3. **Update Product Tab**: Fill form and test POST endpoint
4. **Setup Instructions Tab**: Follow Vercel deployment guide

### Backend API Endpoints
- **GET `/api/user?username=john_doe`** - Retrieve user information
- **POST `/api/product`** - Update product status with JSON body

## 🛠️ Backend Setup (Vercel)

### Prerequisites
- Node.js installed
- Vercel CLI installed (`npm i -g vercel`)

### Deployment Steps
1. **Clone/Download** this repository
2. **Install Vercel CLI**: `npm i -g vercel`
3. **Deploy**: Run `vercel` in the project directory
4. **Follow prompts** to configure your deployment
5. **Update endpoints** in the HTML form with your Vercel URL

### API Endpoints

#### GET /api/user
```javascript
// Query parameters
{
  "username": "john_doe"  // Required
}

// Response
{
  "success": true,
  "data": {
    "name": "John Doe",
    "email": "john@example.com",
    "role": "admin",
    "status": "active",
    "lastLogin": "2024-01-15T10:30:00Z",
    "createdAt": "2023-06-01T00:00:00Z"
  },
  "message": "User found successfully"
}
```

#### POST /api/product
```javascript
// Request body
{
  "username": "john_doe",     // Required
  "productId": "prod_12345",  // Required
  "status": "active"          // Required: active, inactive, pending, discontinued
}

// Response
{
  "success": true,
  "message": "Product status updated successfully",
  "data": {
    "productId": "prod_12345",
    "status": "active",
    "updatedBy": "john_doe",
    "updatedAt": "2024-01-15T10:30:00Z",
    "processingTime": "87ms"
  }
}
```

## 📁 Project Structure

```
my-test-api/
├── api-tester.html      # Main API testing interface
├── api/
│   ├── user.js         # GET endpoint for user data
│   └── product.js      # POST endpoint for product updates
├── vercel.json         # Vercel deployment config
├── index.html          # Landing page
├── dashboard.html      # User management dashboard
├── api-simulator.html  # API endpoint simulator
├── post-viewer.html    # POST request viewer
├── post-form.html      # Test form
└── README.md          # This file
```

## 🎨 API Tester Features

- **Modern UI**: Dark theme with gradient backgrounds and glassmorphism effects
- **Tabbed Interface**: Easy switching between GET and POST testing
- **Real-time Validation**: Form validation with helpful error messages
- **Syntax Highlighting**: JSON responses with proper formatting
- **CORS Support**: Properly configured for cross-origin requests
- **Error Handling**: Comprehensive error messages and status indicators
- **Responsive Design**: Works on desktop and mobile devices
- **Setup Instructions**: Built-in guide for Vercel deployment

## 🚀 Deployment Options

### Option 1: Vercel (Recommended)
1. Run `vercel` in project directory
2. Update endpoints in HTML form
3. Access via your Vercel domain

### Option 2: GitHub Pages
1. Push to GitHub repository
2. Enable GitHub Pages in repository settings
3. Access via `https://yourusername.github.io/repository-name/`

### Option 3: Local Development
1. Use `vercel dev` for local development
2. Test with `http://localhost:3000`

## 📝 License

MIT License - Feel free to use and modify!