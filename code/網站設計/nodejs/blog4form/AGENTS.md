# Blog4Form Project

Simple blog system with Node.js, Express, and SQLite. Twitter-like social platform.

## Tech Stack

- **Runtime**: Node.js
- **Server**: Express.js
- **Database**: SQLite (sqlite3)
- **Auth**: bcryptjs, express-session
- **Templating**: inline HTML templates

## Key Commands

```bash
# Install dependencies
npm install

# Start server (runs on port 3000)
npm start

# Remove database (reset state)
rm blog.db
```

**Note**: No tests currently exist. Adding tests would require setting up a test framework (e.g., Jest, Mocha).

## Code Style Guidelines

### General

- **Language**: Plain JavaScript (no TypeScript)
- **Architecture**: Callback-based async patterns (not Promises/async-await)
- **Structure**: Single-file server (`server.js`) with separate `database.js`

### Imports

```javascript
const express = require('express');
const session = require('express-session');
const bcrypt = require('bcryptjs');
const db = require('./database');
const marked = require('marked');
```

- Use `require()` for all imports
- External packages first, local modules last
- Single line per import

### Naming Conventions

| Type | Convention | Example |
|------|------------|---------|
| Functions | camelCase | `getUserPosts`, `addComment` |
| Database tables | lowercase, snake | `posts`, `user_likes` |
| Columns | snake_case | `created_at`, `author_id` |
| Routes | lowercase | `/login`, `/post/:id` |

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

- Always pass array for params: `[userId]`
- Use callback pattern: `(err, result)`
- Access `this.lastID` for INSERT

### Error Handling

```javascript
// Database errors
if (err) return callback(err, []);

// Not found
if (err || !user) return res.status(404).send('Not found');

// Server errors
if (err) return res.status(500).send('Error');
```

- Check errors immediately after DB calls
- Return empty array/object on error for queries
- Use appropriate HTTP status codes

### Auth Patterns

```javascript
// Middleware
function requireLogin(req, res, next) {
  if (!req.session.userId) {
    return res.redirect('/login');
  }
  next();
}

// Password hashing
bcrypt.hash(password, 10, (err, hash) => {
  if (err) return callback(err);
  // ...
});

// Password comparison
bcrypt.compare(password, user.password, (err, match) => {
  if (err || !match) return res.status(401).send('Invalid');
});
```

### Routes

```javascript
// GET - render page
app.get('/path', (req, res) => { ... });

// POST - form/data submission
app.post('/path', (req, res) => { ... });

// DELETE - remove resource
app.delete('/path/:id', (req, res) => { ... });
```

### HTML Templates

- Use template literals with `${}` interpolation
- Inline CSS in `<style>` tags
- Dark theme by default (#000 background, #fff text)

### Security

- Never expose raw SQL errors to clients
- Use parameterized queries (never string concatenation for SQL)
- Validate required fields before database operations
- Session-based auth only

## File Structure

```
blog4form/
├── server.js      # Main Express app (630 lines)
├── database.js   # SQLite setup & export
├── blog.db       # SQLite database file
└── package.json # Dependencies
```

## Common Patterns

### User Posts with Stats
```javascript
// Async iteration for counts
postsWithStats.forEach((post, index) => {
  db.get('SELECT COUNT(*) FROM comments WHERE post_id = ?', [post.id], (err, stats) => {
    // update post object
    processed++;
    if (processed === rows.length) {
      callback(null, postsWithStats);
    }
  });
});
```

### Toggle (Like/Share)
```javascript
// Check then insert or delete
db.get('SELECT id FROM likes WHERE post_id = ? AND user_id = ?', [postId, userId], (err, row) => {
  if (row) {
    db.run('DELETE FROM likes ...', (err) => callback(err, false));
  } else {
    db.run('INSERT INTO likes ...', (err) => callback(err, true));
  }
});
```