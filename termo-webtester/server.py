import http.server
import socketserver
import os

PORT = 8000
WEB_DIR = "public"

os.chdir(WEB_DIR)

Handler = http.server.SimpleHTTPRequestHandler
httpd = socketserver.TCPServer(("", PORT), Handler)

print(f"Serving web app on http://localhost:{PORT}")
httpd.serve_forever()
