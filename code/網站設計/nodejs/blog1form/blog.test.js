const request = require('supertest');
const db = require('./database');
const app = require('./server');

describe('Blog API', () => {
  beforeAll((done) => {
    db.run('DELETE FROM posts', done);
  });

  afterAll((done) => {
    db.close(done);
  });

  test('GET / returns HTML with blog title', (done) => {
    request(app)
      .get('/')
      .expect(200)
      .expect('Content-Type', /html/)
      .end((err, res) => {
        if (err) return done(err);
        expect(res.text).toContain('<h1>My Blog</h1>');
        done();
      });
  });

  test('GET / shows new post form', (done) => {
    request(app)
      .get('/')
      .end((err, res) => {
        if (err) return done(err);
        expect(res.text).toContain('<form method="post" action="/posts"');
        done();
      });
  });

  test('POST /posts creates new post', (done) => {
    request(app)
      .post('/posts')
      .send({ title: 'Test Post', content: '# Hello World' })
      .expect(302)
      .end(done);
  });

  test('POST /posts validation - missing title returns 400', (done) => {
    request(app)
      .post('/posts')
      .send({ content: 'Content only' })
      .expect(400)
      .end(done);
  });

  test('POST /posts validation - missing content returns 400', (done) => {
    request(app)
      .post('/posts')
      .send({ title: 'Title only' })
      .expect(400)
      .end(done);
  });

  test('GET /post/:id returns post with markdown', (done) => {
    db.run('INSERT INTO posts (title, content) VALUES (?, ?)', ['Markdown Post', '# Heading'], function(err) {
      if (err) return done(err);
      const postId = this.lastID;
      request(app)
        .get(`/post/${postId}`)
        .expect(200)
        .end((err, res) => {
          if (err) return done(err);
          expect(res.text).toContain('<h1>Markdown Post</h1>');
          expect(res.text).toContain('<h1>Heading</h1>');
          done();
        });
    });
  });

  test('GET /post/:id returns 404 for non-existent post', (done) => {
    request(app)
      .get('/post/99999')
      .expect(404)
      .expect('Post not found')
      .end(done);
  });

  test('GET / displays existing posts', (done) => {
    db.run('INSERT INTO posts (title, content) VALUES (?, ?)', ['Display Test', 'Content here'], function(err) {
      if (err) return done(err);
      request(app)
        .get('/')
        .end((err, res) => {
          if (err) return done(err);
          expect(res.text).toContain('Display Test');
          done();
        });
    });
  });
});