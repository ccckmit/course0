from pydantic import BaseModel


class TodoCreate(BaseModel):
    text: str


class TodoUpdate(BaseModel):
    done: bool


class TodoOut(BaseModel):
    id: int
    text: str
    done: bool
