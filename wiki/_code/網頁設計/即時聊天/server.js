const express = require('express');
const { WebSocketServer } = require('ws');
const path = require('path');

const app = express();
const PORT = 3001;

app.use(express.static(path.join(__dirname, 'public')));

const server = app.listen(PORT, () => {
  console.log('Server running on port ' + PORT);
});

const wss = new WebSocketServer({ server });

const clients = new Map();
let clientId = 0;

wss.on('connection', (ws) => {
  const id = ++clientId;
  clients.set(id, ws);
  console.log('Client ' + id + ' connected');
  
  ws.send(JSON.stringify({ type: 'welcome', message: 'Welcome! You are client ' + id }));
  
  broadcastUserList();
  
  ws.on('message', (data) => {
    try {
      const message = JSON.parse(data);
      
      if (message.type === 'chat') {
        const broadcast = {
          type: 'chat',
          sender: id,
          text: message.text,
          timestamp: new Date().toISOString()
        };
        wss.clients.forEach(client => {
          if (client.readyState === 1) {
            client.send(JSON.stringify(broadcast));
          }
        });
      }
      else if (message.type === 'private') {
        const targetId = parseInt(message.to);
        const targetWs = clients.get(targetId);
        if (targetWs && targetWs.readyState === 1) {
          targetWs.send(JSON.stringify({
            type: 'private',
            from: id,
            text: message.text,
            timestamp: new Date().toISOString()
          }));
        }
      }
    } catch (e) {
      console.error('Message error:', e.message);
    }
  });
  
  ws.on('close', () => {
    console.log('Client ' + id + ' disconnected');
    clients.delete(id);
    broadcastUserList();
  });
});

function broadcastUserList() {
  const users = Array.from(clients.keys());
  const message = JSON.stringify({ type: 'users', list: users });
  wss.clients.forEach(client => {
    if (client.readyState === 1) {
      client.send(message);
    }
  });
}

module.exports = { app, server, wss };
