# API Protocol

The server communicates using **JSON over TCP** with newline-delimited messages on port **12345**.

### Connection Format
- **Protocol**: TCP
- **Port**: 12345
- **Format**: JSON messages terminated with `\n`
- **Encoding**: UTF-8

### API Endpoints

#### 1. List Movies
Request: {"command": "LIST_MOVIES"}
Response: {"movies": [{"id": 1, "name": "Inception"}, {"id": 2, "name": "The Matrix"}]}

#### 2. List Theaters
Request: {"command": "LIST_THEATERS", "movie_id": 1}
Response: {"theaters": [{"id": 1, "name": "Cinema One"}, {"id": 2, "name": "Cinema Two"}]}


#### 3. List Available Seats
Request: {"command": "LIST_SEATS", "theater_id": 1, "movie_id": 1}
Response: {"theater_id": 1, "movie_id": 1, "available_seats": ["a1", "a2", "a3"], "total_available": 3}


#### 4. Book Seats
Request: {"command": "BOOK", "theater_id": 1, "movie_id": 1, "seats": ["a1", "a2"]}
Success: {"status": "BOOKED", "theater_id": 1, "movie_id": 1, "seats": ["a1", "a2"], "timestamp": 1750118286}
Failure: {"status": "FAILED", "theater_id": 1, "movie_id": 1, "seats": ["a1", "a2"], "timestamp": 1750118286}


### Seat Layout System

The system uses a grid-based layout with automatic sizing:
- **Layout**: `ceil(sqrt(seat_count))` seats per row
- **Naming**: Row letters (a, b, c...) + seat numbers (1, 2, 3...)
- **Example for 20 seats** (5×4 grid):

a1 a2 a3 a4 a5
b1 b2 b3 b4 b5
c1 c2 c3 c4 c5
d1 d2 d3 d4 d5


### Error Handling
Invalid Command: {"error": "UNKNOWN_COMMAND", "valid_commands": ["LIST_MOVIES", "LIST_THEATERS", "LIST_SEATS", "BOOK"]}
Malformed JSON: {"error": "INVALID_REQUEST", "message": "JSON parsing error details"}


### Client Example (Python)

import socket, json

def send_request(command_data):
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(('127.0.0.1', 12345))
request = json.dumps(command_data) + '\n'
sock.send(request.encode('utf-8'))
response = sock.recv(4096).decode('utf-8').strip()
sock.close()
return json.loads(response)

List movies

movies = send_request({"command": "LIST_MOVIES"})
Book seats

booking = send_request({
"command": "BOOK",
"theater_id": 1,
"movie_id": 1,
"seats": ["a1", "a2"]
})


## Features

- **Thread-safe seat booking** with atomic operations
- **Concurrent client handling** using configurable thread pools
- **Real-time seat availability tracking**
- **Grid-based seat layout** system
- **Comprehensive error handling** and validation
- **High-performance architecture** supporting hundreds of concurrent connections
