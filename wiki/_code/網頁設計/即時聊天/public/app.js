const ws = new WebSocket('ws://localhost:3001');

let myId = null;
let selectedUser = null;

const messagesDiv = document.getElementById('messages');
const userList = document.getElementById('user-list');
const messageInput = document.getElementById('message-input');
const sendBtn = document.getElementById('send-btn');

ws.onmessage = (event) => {
  const data = JSON.parse(event.data);
  
  if (data.type === 'welcome') {
    console.log(data.message);
    addSystemMessage(data.message);
  }
  else if (data.type === 'users') {
    updateUserList(data.list);
  }
  else if (data.type === 'chat') {
    addMessage(data.sender, data.text, data.timestamp, false, data.sender === myId);
  }
  else if (data.type === 'private') {
    addMessage('Client ' + data.from, data.text, data.timestamp, true, false);
  }
};

function addMessage(sender, text, timestamp, isPrivate, isMine) {
  const div = document.createElement('div');
  div.className = 'message' + (isPrivate ? ' private' : '') + (isMine ? ' my-message' : '');
  
  const time = new Date(timestamp).toLocaleTimeString('zh-TW', { hour: '2-digit', minute: '2-digit' });
  
  div.innerHTML = `
    <div class="sender">${isPrivate ? '[私人] ' : ''}${sender}</div>
    <div class="text">${escapeHtml(text)}</div>
    <div class="time">${time}</div>
  `;
  
  messagesDiv.appendChild(div);
  messagesDiv.scrollTop = messagesDiv.scrollHeight;
}

function addSystemMessage(text) {
  const div = document.createElement('div');
  div.className = 'message system';
  div.textContent = text;
  messagesDiv.appendChild(div);
}

function updateUserList(users) {
  userList.innerHTML = '';
  users.forEach(id => {
    if (id === myId) return;
    const li = document.createElement('li');
    li.textContent = 'Client ' + id;
    li.onclick = () => selectUser(id, li);
    if (selectedUser === id) li.classList.add('active');
    userList.appendChild(li);
  });
}

function selectUser(id, element) {
  selectedUser = id;
  document.querySelectorAll('#user-list li').forEach(li => li.classList.remove('active'));
  element.classList.add('active');
}

function sendMessage() {
  const text = messageInput.value.trim();
  if (!text) return;
  
  if (selectedUser) {
    ws.send(JSON.stringify({ type: 'private', to: selectedUser, text: text }));
    addMessage('You', text, new Date().toISOString(), true, true);
  } else {
    ws.send(JSON.stringify({ type: 'chat', text: text }));
    addMessage('You', text, new Date().toISOString(), false, true);
  }
  
  messageInput.value = '';
}

sendBtn.onclick = sendMessage;
messageInput.onkeypress = (e) => {
  if (e.key === 'Enter') sendMessage();
};

function escapeHtml(text) {
  const div = document.createElement('div');
  div.textContent = text;
  return div.innerHTML;
}
