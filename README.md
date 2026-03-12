# cserve

A simple C HTTP server that handles basic web requests.

## Project Structure

- `src/main.c`: The entry point with server setup and routing logic.
- `src/http.c`: HTTP related utility functions (`buildResp`, `respond`, etc.).
- `src/handlers.c`: Endpoint handlers (e.g. echo, user-agent, file endpoints).
- `include/`: Header files defining functions used across the project.

## Endpoints

- `GET /`: Returns `200 OK`.
- `GET /echo/{str}`: Returns the provided string in the body.
- `GET /user-agent`: Returns the client's `User-Agent`.
- `GET /files/{filename}`: Returns the requested file contents from the shared directory.
- `POST /files/{filename}`: Creates or overrides a file in the shared directory with the payload body.

## Build and Run

To compile the server:
```sh
make
```

To run the server locally:
```sh
./cserve
```

To run with file directory for endpoints:
```sh
./cserve --directory /tmp/
```
