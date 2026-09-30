"""Serves a directory as a cross-origin isolated page, which a threaded build needs.

Usage: python3 external/web/serve.py <directory> [port]
"""

import errno
import functools
import http.server
import sys

DEFAULT_DIRECTORY = "."
DEFAULT_PORT = 8000
LAST_PORT_SEARCHED = 8100
LOOPBACK_HOST = "127.0.0.1"


class CrossOriginIsolatedHandler(http.server.SimpleHTTPRequestHandler):
    extensions_map = {
        **http.server.SimpleHTTPRequestHandler.extensions_map,
        ".js": "text/javascript",
        ".wasm": "application/wasm",
    }

    def end_headers(self):
        self.send_header("Cross-Origin-Opener-Policy", "same-origin")
        self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
        self.send_header("Cache-Control", "no-store")
        super().end_headers()


def bind_to_first_free_port(directory, first_port):
    handler = functools.partial(CrossOriginIsolatedHandler, directory=directory)

    for port in range(first_port, max(first_port, LAST_PORT_SEARCHED) + 1):
        try:
            return http.server.ThreadingHTTPServer((LOOPBACK_HOST, port), handler)
        except OSError as error:
            if error.errno != errno.EADDRINUSE:
                raise

    return None


def main():
    directory = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_DIRECTORY
    first_port = int(sys.argv[2]) if len(sys.argv) > 2 else DEFAULT_PORT

    server = bind_to_first_free_port(directory, first_port)
    if server is None:
        sys.exit(f"Every port from {first_port} to {LAST_PORT_SEARCHED} is taken.")

    with server:
        served_port = server.server_address[1]
        print(f"Serving {directory} at http://{LOOPBACK_HOST}:{served_port}")
        server.serve_forever()


if __name__ == "__main__":
    main()
