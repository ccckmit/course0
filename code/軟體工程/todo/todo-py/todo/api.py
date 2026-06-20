from contextlib import asynccontextmanager
from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from .database import init_db, add_todo, list_todos, done_todo, delete_todo
from .models import TodoCreate, TodoUpdate, TodoOut


@asynccontextmanager
async def lifespan(app: FastAPI):
    init_db()
    yield


app = FastAPI(title="Todo API", lifespan=lifespan)

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)


@app.get("/api/todos", response_model=list[TodoOut])
def get_todos():
    return list_todos(include_done=True)


@app.post("/api/todos", response_model=dict)
def create_todo(body: TodoCreate):
    tid = add_todo(body.text)
    return {"id": tid, "text": body.text, "done": False}


@app.put("/api/todos/{todo_id}", response_model=dict)
def update_todo(todo_id: int, body: TodoUpdate):
    if body.done:
        if not done_todo(todo_id):
            raise HTTPException(404, "Todo not found")
    return {"id": todo_id, "done": body.done}


@app.delete("/api/todos/{todo_id}", response_model=dict)
def remove_todo(todo_id: int):
    if not delete_todo(todo_id):
        raise HTTPException(404, "Todo not found")
    return {"id": todo_id, "deleted": True}
