import json

class Response:
    def __init__(self, content=b"", status_code=200, headers=None, media_type=None):
        self.content = content
        self.status_code = status_code
        self.headers = headers or {}
        self.media_type = media_type

    async def __call__(self, scope, receive, send):
        # Prepare headers
        encoded_headers = [(k.lower().encode('utf-8'), str(v).encode('utf-8')) for k, v in self.headers.items()]
        
        if self.media_type:
            encoded_headers.append((b'content-type', self.media_type.encode('utf-8')))

        # Send start message
        await send({
            'type': 'http.response.start',
            'status': self.status_code,
            'headers': encoded_headers,
        })

        # Send body
        await send({
            'type': 'http.response.body',
            'body': self.content,
        })

class JSONResponse(Response):
    def __init__(self, content, status_code=200, headers=None):
        body = json.dumps(content).encode('utf-8')
        super().__init__(content=body, status_code=status_code, headers=headers, media_type="application/json")
