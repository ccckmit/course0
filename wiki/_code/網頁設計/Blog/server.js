const express = require('express');
const sqlite3 = require('sqlite3').verbose();
const path = require('path');

const app = express();
const PORT = 3000;

const db = new sqlite3.Database(':memory:');

app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

function initDB() {
  return new Promise((resolve, reject) => {
    db.serialize(() => {
      db.run(`CREATE TABLE posts (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        title TEXT NOT NULL,
        content TEXT NOT NULL,
        author TEXT NOT NULL,
        created_at DATETIME DEFAULT CURRENT_TIMESTAMP
      )`, (err) => {
        if (err) reject(err);
        else resolve();
      });
    });
  });
}

function getAllPosts() {
  return new Promise((resolve, reject) => {
    db.all('SELECT * FROM posts ORDER BY created_at DESC', (err, rows) => {
      if (err) reject(err);
      else resolve(rows);
    });
  });
}

function getPostById(id) {
  return new Promise((resolve, reject) => {
    db.get('SELECT * FROM posts WHERE id = ?', [id], (err, row) => {
      if (err) reject(err);
      else resolve(row);
    });
  });
}

function createPost(title, content, author) {
  return new Promise((resolve, reject) => {
    const now = new Date().toISOString();
    db.run('INSERT INTO posts (title, content, author, created_at) VALUES (?, ?, ?, ?)',
      [title, content, author, now], function(err) {
        if (err) reject(err);
        else resolve({ id: this.lastID, title, content, author, created_at: now });
      });
  });
}

function updatePost(id, title, content) {
  return new Promise((resolve, reject) => {
    db.run('UPDATE posts SET title = ?, content = ? WHERE id = ?',
      [title, content, id], function(err) {
        if (err) reject(err);
        else resolve({ changes: this.changes });
      });
  });
}

function deletePost(id) {
  return new Promise((resolve, reject) => {
    db.run('DELETE FROM posts WHERE id = ?', [id], function(err) {
      if (err) reject(err);
      else resolve({ changes: this.changes });
    });
  });
}

app.get('/api/posts', async (req, res) => {
  try {
    const posts = await getAllPosts();
    res.json(posts);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

app.get('/api/posts/:id', async (req, res) => {
  try {
    const post = await getPostById(req.params.id);
    if (!post) return res.status(404).json({ error: 'Post not found' });
    res.json(post);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

app.post('/api/posts', async (req, res) => {
  try {
    const { title, content, author } = req.body;
    if (!title || !content || !author) {
      return res.status(400).json({ error: 'Missing required fields' });
    }
    const post = await createPost(title, content, author);
    res.status(201).json(post);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

app.put('/api/posts/:id', async (req, res) => {
  try {
    const { title, content } = req.body;
    const result = await updatePost(req.params.id, title, content);
    if (result.changes === 0) {
      return res.status(404).json({ error: 'Post not found' });
    }
    res.json({ success: true });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

app.delete('/api/posts/:id', async (req, res) => {
  try {
    const result = await deletePost(req.params.id);
    if (result.changes === 0) {
      return res.status(404).json({ error: 'Post not found' });
    }
    res.json({ success: true });
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

let server;

function startServer() {
  return new Promise((resolve) => {
    server = app.listen(PORT, () => {
      console.log('Server running on port ' + PORT);
      resolve();
    });
  });
}

function stopServer() {
  return new Promise((resolve) => {
    if (server) {
      server.close(() => {
        console.log('Server stopped');
        resolve();
      });
    } else {
      resolve();
    }
  });
}

module.exports = { app, initDB, getAllPosts, getPostById, createPost, updatePost, deletePost, startServer, stopServer };

if (require.main === module) {
  initDB().then(() => {
    app.listen(PORT, () => {
      console.log('Blog server running on port ' + PORT);
    });
  });
}
