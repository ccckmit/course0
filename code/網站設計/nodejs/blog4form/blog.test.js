const request = require('supertest');
const db = require('./database');
const app = require('./server');

describe('Blog4 API', () => {
  beforeAll((done) => {
    db.run('DELETE FROM comments', done);
  });

  beforeEach((done) => {
    db.run('DELETE FROM likes', () => {
      db.run('DELETE FROM shares', () => {
        db.run('DELETE FROM comments', () => {
          db.run('DELETE FROM posts', () => {
            db.run('DELETE FROM users', done);
          });
        });
      });
    });
  });

  afterAll((done) => {
    db.close(done);
  });

  describe('Auth Routes', () => {
    test('GET /login shows login form', (done) => {
      request(app).get('/login')
        .expect(200)
        .end((err, res) => {
          if (err) return done(err);
          expect(res.text).toContain('<h1 class="form-title">Log in to youbook</h1>');
          done();
        });
    });

    test('GET /register shows registration form', (done) => {
      request(app).get('/register')
        .expect(200)
        .end((err, res) => {
          if (err) return done(err);
          expect(res.text).toContain('<h1 class="form-title">Create account</h1>');
          done();
        });
    });

    test('GET / redirects to /login when not logged in', (done) => {
      request(app).get('/')
        .expect(302)
        .expect('Location', '/login')
        .end(done);
    });

    test('GET /logout destroys session', (done) => {
      request(app).get('/logout')
        .expect(302)
        .expect('Location', '/')
        .end(done);
    });
  });

  describe('Registration', () => {
    test('POST /register creates new user', (done) => {
      request(app).post('/register')
        .send({ username: 'newuser', password: 'pass123' })
        .expect(302)
        .expect('Location', '/login')
        .end(done);
    });

    test('POST /register duplicate username returns 400', (done) => {
      db.run('INSERT INTO users (username, password) VALUES (?, ?)', ['dupuser', '$2a$10$hash'], () => {
        request(app).post('/register')
          .send({ username: 'dupuser', password: 'pass123' })
          .expect(400)
          .end(done);
      });
    });

    test('POST /register missing fields returns 400', (done) => {
      request(app).post('/register')
        .send({ username: '' })
        .expect(400)
        .end(done);
    });
  });

  describe('Login', () => {
    test('POST /login invalid credentials returns 401', (done) => {
      request(app).post('/login')
        .send({ username: 'nobody', password: 'wrong' })
        .expect(401)
        .end(done);
    });
  });

  describe('Public Access', () => {
    test('GET /user/99999 returns 404', (done) => {
      request(app).get('/user/99999')
        .expect(404)
        .end(done);
    });

    test('GET /post/99999 returns 404', (done) => {
      request(app).get('/post/99999')
        .expect(404)
        .end(done);
    });
  });

  describe('Database Integrity', () => {
    let userId, postId;

    beforeEach((done) => {
      db.run('INSERT INTO users (username, password) VALUES (?, ?)', ['authuser', '$2a$10$h'], function(err) {
        if (err) return done(err);
        userId = this.lastID;
        db.run('INSERT INTO posts (title, content, author_id) VALUES (?, ?, ?)', ['Test', 'Content', userId], function(err) {
          if (err) return done(err);
          postId = this.lastID;
          done();
        });
      });
    });

    test('posts table has foreign key to users', (done) => {
      db.get('SELECT * FROM posts WHERE id = ?', [postId], (err, post) => {
        if (err) return done(err);
        expect(post.author_id).toBe(userId);
        done();
      });
    });

    test('can add comment to post', (done) => {
      db.run('INSERT INTO comments (post_id, author_id, content) VALUES (?, ?, ?)', [postId, userId, 'Comment'], function(err) {
        if (err) return done(err);
        db.get('SELECT * FROM comments WHERE post_id = ?', [postId], (err, comment) => {
          if (err) return done(err);
          expect(comment.content).toBe('Comment');
          done();
        });
      });
    });

    test('can toggle like', (done) => {
      db.run('INSERT INTO likes (post_id, user_id) VALUES (?, ?)', [postId, userId], function(err) {
        if (err) return done(err);
        db.get('SELECT id FROM likes WHERE post_id = ? AND user_id = ?', [postId, userId], (err, like) => {
          if (err) return done(err);
          expect(like).toBeTruthy();
          done();
        });
      });
    });
  });
});