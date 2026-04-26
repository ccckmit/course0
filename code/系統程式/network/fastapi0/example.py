from fastapi0 import FastAPI0

app = FastAPI0()

@app.get("/")
async def read_root():
    return {"Hello": "World"}

@app.get("/ping")
async def ping():
    return {"ping": "pong"}

@app.post("/echo")
async def echo(request):
    data = await request.json()
    return {"you_sent": data}
