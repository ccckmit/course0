import json

class Request:
    def __init__(self, scope, receive):
        self.scope = scope
        self.receive = receive
        self._body = None

    @property
    def method(self) -> str:
        return self.scope.get("method", "")

    @property
    def url(self) -> str:
        return self.scope.get("path", "")

    @property
    def query_string(self) -> bytes:
        return self.scope.get("query_string", b"")

    @property
    def headers(self) -> dict:
        return {k.decode("utf-8"): v.decode("utf-8") for k, v in self.scope.get("headers", [])}

    async def body(self) -> bytes:
        if self._body is None:
            body = b""
            more_body = True
            while more_body:
                message = await self.receive()
                body += message.get("body", b"")
                more_body = message.get("more_body", False)
            self._body = body
        return self._body

    async def json(self):
        body = await self.body()
        if body:
            return json.loads(body)
        return None
