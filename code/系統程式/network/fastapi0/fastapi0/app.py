from .requests import Request
from .responses import JSONResponse, Response

class FastAPI0:
    def __init__(self):
        self.routes = {}

    def get(self, path):
        return self.route("GET", path)

    def post(self, path):
        return self.route("POST", path)

    def route(self, method, path):
        def decorator(func):
            if path not in self.routes:
                self.routes[path] = {}
            self.routes[path][method] = func
            return func
        return decorator

    async def __call__(self, scope, receive, send):
        if scope["type"] != "http":
            return # We only handle HTTP for now

        request = Request(scope, receive)
        path = scope["path"]
        method = scope["method"]

        if path in self.routes and method in self.routes[path]:
            handler = self.routes[path][method]
            
            # Simple signature mapping - if they need the request, we theoretically pass it,
            # but for minimalism (matching example), we just call it if it takes no args.
            import inspect
            sig = inspect.signature(handler)
            kwargs = {}
            if "request" in sig.parameters:
                kwargs["request"] = request

            response = await handler(**kwargs)

            # If the response is a dictionary, wrap it in JSONResponse (like FastAPI)
            if isinstance(response, dict):
                response = JSONResponse(response)
                
            # If they just returned a string, wrap in Response
            elif isinstance(response, str):
                response = Response(content=response.encode('utf-8'))
                
            if isinstance(response, Response):
                await response(scope, receive, send)
            else:
                # Fallback error for unsupported return types
                error = JSONResponse({"error": "Invalid response type"}, status_code=500)
                await error(scope, receive, send)
        else:
            # 404 Not Found
            not_found = JSONResponse({"detail": "Not Found"}, status_code=404)
            await not_found(scope, receive, send)
