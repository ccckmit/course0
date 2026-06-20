import { useState, useEffect } from 'react'

function App() {
  const [todos, setTodos] = useState([])
  const [text, setText] = useState('')

  useEffect(() => { fetchTodos() }, [])

  async function fetchTodos() {
    const res = await fetch('/api/todos')
    setTodos(await res.json())
  }

  async function addTodo() {
    if (!text.trim()) return
    await fetch('/api/todos', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ text })
    })
    setText('')
    fetchTodos()
  }

  async function doneTodo(id) {
    await fetch(`/api/todos/${id}`, {
      method: 'PUT',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ done: true })
    })
    fetchTodos()
  }

  async function deleteTodo(id) {
    await fetch(`/api/todos/${id}`, { method: 'DELETE' })
    fetchTodos()
  }

  return (
    <div className="app">
      <h1>Todo</h1>
      <div className="input-row">
        <input value={text} onChange={e => setText(e.target.value)}
               onKeyDown={e => e.key === 'Enter' && addTodo()}
               placeholder="Add a todo..." />
        <button onClick={addTodo}>Add</button>
      </div>
      <ul className="todo-list">
        {todos.map(t => (
          <li key={t.id} className={t.done ? 'done' : ''}>
            <span className="todo-text">{t.text}</span>
            <div className="todo-actions">
              {!t.done && <button className="btn-done" onClick={() => doneTodo(t.id)}>✓</button>}
              <button className="btn-del" onClick={() => deleteTodo(t.id)}>✕</button>
            </div>
          </li>
        ))}
      </ul>
    </div>
  )
}

export default App
