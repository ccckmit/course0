# Blog1Form Project

Simple blog system with Node.js, Express, and SQLite. Basic markdown blog platform.

## Tech Stack

- **Runtime**: Node.js
- **Server**: Express.js
- **Database**: SQLite (sqlite3)
- **Templating**: Inline HTML templates with template literals
- **Markdown**: marked

## Key Commands

```bash
# Install dependencies
npm install

# Start server (runs on port 3000)
npm start

# Remove database (reset state)
rm blog.db
```

**Note**: No tests currently exist. Adding tests would require setting up a test framework (e.g., Jest, Mocha, or node:test).

## Code Style Guidelines

### General

- **Language**: Plain JavaScript (no TypeScript)
- **Architecture**: Callback-based async patterns (not Promises/async-await)
- **Structure**: Single-file server (`server.js`) with separate `database.js`
- **Comments**: Chinese comments inline (educational code)

### Imports

```javascript
const express = require('express');
const db = require('./database');
const marked = require('marked');
```

- Use `require()` for all imports
- External packages first, local modules last
- Single line per import

### Naming Conventions

| Type | Convention | Example |
|------|------------|---------|
| Functions | camelCase | `getPosts`, `addPost`, `getPost` |
| Database tables | lowercase, snake | `posts` |
| Columns | snake_case | `created_at`, `author_id` |
| Routes | lowercase | `/`, `/post/:id`, `/posts` |

### Database Patterns

```javascript
// Get single row
db.get('SELECT ...', [params], (err, row) => {
  callback(err, row);
});

// Get multiple rows
db.all('SELECT ...', [params], (err, rows) => {
  callback(err, rows);
});

// Run statement
db.run('INSERT ...', [params], function(err) {
  callback(err, this.lastID);
});
```

- Always pass array for params: `[id]`
- Use callback pattern: `(err, result)`
- Access `this.lastID` for INSERT

### Error Handling

```javascript
// Database errors
if (err) return res.status(500).send('Error loading posts');

// Not found
if (err || !post) return res.status(404).send('Post not found');

// Validation errors
if (!title || !content) return res.status(400).send('Title and content required');
```

- Check errors immediately after DB calls
- Return appropriate HTTP status codes
- User-friendly error messages (not raw SQL errors)

### Routes

```javascript
// GET - render page
app.get('/path', (req, res) => { ... });

// POST - form/data submission
app.post('/posts', (req, res) => { ... });
```

### HTML Templates

```javascript
let html = `<!DOCTYPE html>
<html>
<head>
  <style>
    body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; }
  </style>
</head>
<body>
  <h1>Title</h1>
  ${dynamicContent}
</body></html>`;
```

- Use template literals with `${}` interpolation
- Inline CSS in `<style>` tags
- Use consistent typography: `-apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif`
- Max width: 800px, centered

### Security

- Never expose raw SQL errors to clients
- Use parameterized queries (never string concatenation for SQL)
- Validate required fields before database operations

## File Structure

```
blog1form/
├── server.js      # Main Express app (157 lines)
├── database.js   # SQLite setup & export (42 lines)
├── blog.db       # SQLite database file
└── package.json # Dependencies
```

## Common Patterns

### Get Posts with Excerpt
```javascript
db.all('SELECT id, title, substr(content, 1, 200) as excerpt, created_at FROM posts ORDER BY created_at DESC', [], (err, rows) => {
  callback(err, rows);
});
```

### Get Single Post
```javascript
db.get('SELECT * FROM posts WHERE id = ?', [id], (err, row) => {
  callback(err, row);
});
```

### Insert Post
```javascript
db.run('INSERT INTO posts (title, content) VALUES (?, ?)', [title, content], function(err) {
  callback(err, this.lastID);
});
```

### Middleware
```javascript
app.use(express.json());
app.use(express.urlencoded({ extended: true }));
```

### Markdown Rendering
```javascript
marked.parse(markdownContent) // returns HTML string
```

## Database Schema

```sql
CREATE TABLE posts (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  title TEXT NOT NULL,
  content TEXT NOT NULL,
  created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);
```

- `id`: Auto-increment primary key
- `title`: Required string
- `content`: Required string (Markdown)
- `created_at`: Auto-timestamp