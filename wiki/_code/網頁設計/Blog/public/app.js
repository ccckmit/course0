const API_URL = '/api/posts';

document.addEventListener('DOMContentLoaded', () => {
  loadPosts();
  setupModal();
});

async function loadPosts() {
  const container = document.getElementById('posts-container');
  container.innerHTML = '<div class="loading">載入中...</div>';
  
  try {
    const response = await fetch(API_URL);
    if (!response.ok) throw new Error('Failed to load posts');
    const posts = await response.json();
    
    if (posts.length === 0) {
      container.innerHTML = '<div class="loading">尚無文章</div>';
      return;
    }
    
    container.innerHTML = posts.map(post => `
      <article class="post-card" data-id="${post.id}">
        <h2>${escapeHtml(post.title)}</h2>
        <div class="post-meta">
          作者: ${escapeHtml(post.author)} | 
          日期: ${new Date(post.created_at).toLocaleDateString('zh-TW')}
        </div>
        <div class="post-content">${escapeHtml(post.content)}</div>
        <div class="post-actions">
          <button class="edit-btn" onclick="editPost(${post.id})">編輯</button>
          <button class="delete-btn" onclick="deletePost(${post.id})">刪除</button>
        </div>
      </article>
    `).join('');
  } catch (err) {
    container.innerHTML = `<div class="error">錯誤: ${err.message}</div>`;
  }
}

function setupModal() {
  const modal = document.getElementById('post-modal');
  const closeBtn = document.querySelector('.close');
  const newBtn = document.getElementById('new-post-btn');
  const form = document.getElementById('post-form');
  
  newBtn.onclick = () => openModal();
  closeBtn.onclick = () => closeModal();
  modal.onclick = (e) => {
    if (e.target === modal) closeModal();
  };
  
  form.onsubmit = async (e) => {
    e.preventDefault();
    await savePost();
  };
}

function openModal(post = null) {
  const modal = document.getElementById('post-modal');
  const title = document.getElementById('modal-title');
  const idInput = document.getElementById('post-id');
  const titleInput = document.getElementById('title');
  const authorInput = document.getElementById('author');
  const contentInput = document.getElementById('content');
  
  if (post) {
    title.textContent = '編輯文章';
    idInput.value = post.id;
    titleInput.value = post.title;
    authorInput.value = post.author;
    contentInput.value = post.content;
    authorInput.disabled = true;
  } else {
    title.textContent = '新增文章';
    idInput.value = '';
    titleInput.value = '';
    authorInput.value = '';
    contentInput.value = '';
    authorInput.disabled = false;
  }
  
  modal.classList.add('show');
}

function closeModal() {
  document.getElementById('post-modal').classList.remove('show');
}

async function savePost() {
  const id = document.getElementById('post-id').value;
  const title = document.getElementById('title').value;
  const author = document.getElementById('author').value;
  const content = document.getElementById('content').value;
  
  const data = { title, author, content };
  const url = id ? `${API_URL}/${id}` : API_URL;
  const method = id ? 'PUT' : 'POST';
  
  try {
    const response = await fetch(url, {
      method,
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(data)
    });
    
    if (!response.ok) {
      const err = await response.json();
      throw new Error(err.error || 'Failed to save');
    }
    
    closeModal();
    loadPosts();
  } catch (err) {
    alert('錯誤: ' + err.message);
  }
}

async function editPost(id) {
  try {
    const response = await fetch(`${API_URL}/${id}`);
    if (!response.ok) throw new Error('Failed to load post');
    const post = await response.json();
    openModal(post);
  } catch (err) {
    alert('錯誤: ' + err.message);
  }
}

async function deletePost(id) {
  if (!confirm('確定要刪除這篇文章嗎？')) return;
  
  try {
    const response = await fetch(`${API_URL}/${id}`, { method: 'DELETE' });
    if (!response.ok) throw new Error('Failed to delete');
    loadPosts();
  } catch (err) {
    alert('錯誤: ' + err.message);
  }
}

function escapeHtml(text) {
  const div = document.createElement('div');
  div.textContent = text;
  return div.innerHTML;
}
