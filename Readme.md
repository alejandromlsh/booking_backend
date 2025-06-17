
# MOVIE BOOKING SYSTEM

## Overview
This movie booking system is built with modern C++20. Some of the software design choices and C++ features are:
 - **interface-based architecture** with **dependency injection** for decoupling make easier future extensions:
  Visible in the Interface/ directory
 - **Factory creational pattern** for decoupling the creation of different types of Seats.
 - **move semantics**,**perfect forwarding**, **type traits** and other modern C++ performance enhancers:
 Move semantics for adding movies and perfect forwarding for adding seats
 - **TCP server connection** to clients with **JSON messages**:
 In the TCP server file
 - **Unit Testing** and **functional testing** using GTest and Mocking interfaces:
 In the Test/ directory
 - **Multithreading** for asynchronous manage of requests:
 Managed in the TCPServer and with protections in the relevant places.

 Overall I designed the application with SOLID principles in mind, and applying modern C++ features to increase the performance of the system.
 
 Some extra features I could have implemented:
  - A TheaterFactory similar to the SeatFactory. It has sense since there could also be several types of theaters (like IMAX, Standard and so on).
  - A HTTP server on top of the TCP server.
  - A GUI for the client using Qt
  - A User Authorization System
  - Replace all locks with atomics operation for increased performance.
  - Metaprogramming for the computations in the code as the number of rows in the theater. But since that computation is small and does not affect the end user but the AdministrationService the improvement of performance is neglibible.
  - Config File
  - And of course a DataBase although that was explicitly excluded


## Building the system

A Dockerfile is provided that contains all the necessary dependencies to build and run this system. I recommend to use it to avoid getting into troubles with the version of BOOST needed (1.85) that is one of the latest stable versions.

This version was choosen because it has support to json objects, that are crucial for the networking.

In addition to the Dockerfile I provide 2 shell scripts to easy work with the Dockerfile:

- build_docker.sh -> used the first time to build and enter the docker image
- enter_docker.sh -> create a terminal session inside the docker image.

So, first time this will be necessary to build the docker image
```sh
chmod +x build_docker.sh
chmod +x enter_docker.sh
./build_docker.sh
```

### Build Booking App and Tests
```sh
./enter_docker.sh
mkdir build
cd build
cmake ..
make
```
This will create the next binaries:
- movie_booking -> main application
- unit_tests -> run the unit tests suite
- functional_tests -> run the functional tests

### Building the Client that interact with the final User
A folder called **client** is also included in the project directory. It contains a SimpleClient.cpp file that communicate with the main application via TCP using json formated messages and that display the options to the end-user via command line. Using the simple client you can see movies, theaters and book tickets for movies. 
This client could be easily replaced by a GUI if a extension of the app would be needed.

The Client contains its own CMakeLists.txt because it is thought as a separate app/

To build the client do the next:

```sh
cd client
mkdir build
cd build
cmake ..
make
```

This will create the next binary:

- movie_booking_client

## Running and Testing the system

The core functions of the system are managed by the main app, and the interaction with the user is done by the client.

The main app will be run in one terminal sessions, and then you need at least one more terminal session to run the client. You can also do several client session simultaneusly to stress the system:

### Running the main app

Simply open a linux terminal in the project directory and follow the next steps:
```sh
./enter_docker.sh
cd build
./movie_booking
```

### Running one or more client sessions
Open one or more linux terminal in the project directory and follow the next steps:
```sh
./enter_docker.sh
cd client
cd build
./movie_booking_client
```
Now you can use its instructions to communicate and book tickets. The architecture design is more though to replace this simple client by a GUI,
so that a human does not need to see those json objects, but that is something for a future extension.

### Testing the system
Simply go to the test build folder and run the tests there:
```sh
cd build
./unit_tests
./functional_tests
```

## Reflection

### What aspect of this exercise did you find the most interesting?

For me it was interesting to use Boost 1.85 new functionalities to work with json format, before that you need to use other third party libraries that are not so well tested as Boost.

Also the reflection about the system. Build such booking system in a "dirty way" is easy, but are you doing it correctly? Are you choosing a extensible design and architecture? Is it easibly extensible? It is able to support some load? Am I focusing in the important areas to get a production system or in the wrong?

I did a couple of big refactorings along the way, when I realise some decisions were wrong. For example in the first version both the BookingService and the AdministrativeService were part of the same class.
Also at the beginning the TCPServer communicate wihout json format, making it much less powerful.

### What did you find the most cumberstone?

The code itself it is not very difficult, although always some integration with boost can be tricky. The multithreading part is also relatively easy since with the current functionalities the protection is quite straightforward. The most exotic features I implemented in the tests indeed.

The most tricky part was to think about the scope of the application, I wanted to do something relatively production ready so the question is what features are missing?
For Example, Should I implement a User Authorization Simple System? Or should I stay with the plain requirements and just use the best C++ and algorithm choices to do the biggest possible optimization.
Similar with the multithreading, right now it works, it is relatively system but seems to be appropiate for the scope. However, if the system would be use in real-world it would face a much higher load and the system could bottleneck under certain cirmcumstances.
Also in some of the algorithms, for example right not the search for theathers that has a movie is linear. This could be improved, should I do it? Or it is better to focus in another feature.

Since my experience is in a different domain this questions were insteresting.




## Architectural design decisions

Here you can find the explanation of the architecture and design choices, as well as the API explanation.

1. INTERFACE-BASED DESIGN & DEPENDENCY INJECTION

   The system is built around the Dependency Inversion Principle, using abstract interfaces to keep modules independent:
   - IDataStore: Handles all data storage operations
   - ITheater: Defines theater behavior
   - ISeat: Allows different seat types (regular, VIP, accessible)
   - IBookingService: Contains booking logic
   - IAdministrationService: Manages administration functions

   Advantages:
   * Components are independent and easy to replace or update
   * Unit testing is simple using mock implementations
   * Supports runtime polymorphism for seat types
   * Easy to add new storage backends in the future
   * Follows SOLID principles for maintainable code

2. MODIFIED MVC PATTERN (SERVICE-ORIENTED)

   The architecture uses a service-oriented version of the MVC pattern:
   
   MODEL LAYER:
   - Core entities: Movie, Seat, Theater
   - CentralDataStore: Thread-safe data repository using shared_mutex
   - Seat booking uses atomic operations to prevent race conditions

   VIEW LAYER:
   - All responses are JSON with structured error reporting
   - REST-style commands (LIST_MOVIES, BOOK, etc.)
   - Client-server communication through TCP sockets

   CONTROLLER LAYER:
   - TcpServer: Handles network I/O and request routing
   - BookingService: Processes booking logic
   - AdministrationService: Manages admin operations

   Benefits:
   * Business logic can be tested independently
   * Multiple client interfaces (TCP, HTTP, CLI) are possible
   * Clear separation of data, logic, and presentation

3. NETWORKING ARCHITECTURE & JSON PROTOCOL

   TCP SERVER:
   - Uses Boost.Asio for asynchronous I/O
   - Thread pool for handling multiple client sessions at once
   - Persistent connections for multiple requests per client
   - Handles errors and disconnects gracefully

   JSON API:
   - Requests and responses use structured JSON for type safety
   - Command-based operations with parameter validation
   - Clear error responses and sample formats for clients

   Supported Commands:
   * LIST_MOVIES: Get all available movies
   * LIST_THEATERS: Find theaters showing a movie  
   * LIST_SEATS: See available seats for a movie showing
   * BOOK: Reserve seats with atomic booking

   Advantages:
   * Clients can be written in any language
   * Easy integration with web frontends or other systems
   * Robust error handling and validation
   * Easy to add more commands

4. CONCURRENCY & THREAD SAFETY

   LOCKING STRATEGY:
   - CentralDataStore uses shared_mutex for efficient reading and writing
   - Seat objects use atomic operations for lock-free booking
   - Theater seat maps are protected by mutex for consistency
   - Thread pool keeps the server responsive under heavy load

   Performance Optimizations:
   - Multiple readers, single writer for efficiency
   - Lock-free seat booking for high throughput
   - Smart pointers for safe and efficient memory management
   - RAII principles for resource safety

5. TESTING ARCHITECTURE

   COMPREHENSIVE TEST COVERAGE:
   
   UNIT TESTS (unit_test.cpp):
   - Each component is tested with mock dependencies
   - Concurrency stress tests
   - Checks for race conditions in seat booking
   - Thread safety tests for all components

   FUNCTIONAL TESTS (functional_test_tcp.cpp):
   - End-to-end TCP server integration tests
   - JSON protocol validation and error handling
   - Simulate multiple clients booking at once
   - Full user journey from discovery to booking

   Test Design:
   
   ✓ Interfaces (IDataStore, ITheater) are used for all tests  
   ✓ Mock implementations follow interface contracts  
   ✓ Dependency injection enables isolated testing  
   ✓ Concurrency and thread safety are validated  
   ✓ Integration tests cover full workflows

6. CONTAINERIZATION & DEPLOYMENT

   DOCKER SETUP:
   - Multi-stage builds for optimized production images
   - Boost library integration and dependency management
   - Consistent development environment on any platform
   - Volume mounting for live code development

   SHELL SCRIPTS:
   - build_docker.sh: Automated container build and run
   - enter_docker.sh: Access the development environment
   - Consistent workflow for all team members

7. BUILD SYSTEM & DEPENDENCIES

   CMAKE CONFIGURATION:
   - Modern, target-based CMake setup
   - Separate library and executable targets
   - Integrated testing with CTest
   - Boost and Google Test managed by version
   - Clean dependency management

## API Reference Guide - Protocol

### TRANSPORT LAYER

Protocol: TCP with persistent connections
Port: 12345 (configurable)
Message Format: JSON objects terminated with newline (\n)
Character Encoding: UTF-8
Connection Model: Long-lived connections with request/response cycles

### MESSAGE STRUCTURE

All messages are JSON objects with consistent structure:

REQUEST FORMAT:
{
  "command": "COMMAND_NAME",
  "parameter1": value1,
  "parameter2": value2,
  ...additional parameters
}

RESPONSE FORMAT (Success):
{
  "field1": value1,
  "field2": value2,
  ...response data
}

RESPONSE FORMAT (Error):
{
  "error": "ERROR_TYPE",
  "message": "Human-readable error description",
  "received_command": "invalid_command_if_applicable",
  "sample_format": { ...example_request_structure... }
}

## API Endpoints

1. LIST_MOVIES
--------------
PURPOSE: Retrieve all available movies in the system
SCOPE: Read-only operation, no authentication required

REQUEST:
{
  "command": "LIST_MOVIES"
}

RESPONSE:
{
  "movies": [
    {
      "id": 1,
      "name": "Movie Title"
    },
    {
      "id": 2, 
      "name": "Another Movie"
    }
  ]
}

IMPLEMENTATION NOTES FOR DEVELOPERS:
- Movies are sorted by ID for consistent ordering
- Response uses array format for easy iteration
- Movie objects contain minimal data for performance
- Backend uses shared_lock for concurrent read access

ERROR CONDITIONS:
- No specific errors (always returns empty array if no movies)

2. LIST_THEATERS
----------------
PURPOSE: Find theaters showing a specific movie
SCOPE: Read-only operation, requires valid movie_id

REQUEST:
{
  "command": "LIST_THEATERS",
  "movie_id": 123
}

RESPONSE:
{
  "theaters": [
    {
      "id": 1,
      "name": "Cinema One"
    },
    {
      "id": 2,
      "name": "Cinema Two"  
    }
  ]
}

IMPLEMENTATION NOTES FOR DEVELOPERS:
- movie_id must be integer type (JSON number)
- Backend iterates through all theaters checking shows_movie()
- Returns empty array if movie not found (not an error condition)
- Thread-safe operation using shared_lock on data store

ERROR CONDITIONS:
- Invalid JSON structure
- Missing movie_id parameter
- movie_id not integer type

3. LIST_SEATS
-------------
PURPOSE: Get available seats for a specific movie in a theater
SCOPE: Read-only operation, requires theater_id and movie_id

REQUEST:
{
  "command": "LIST_SEATS",
  "theater_id": 1,
  "movie_id": 123
}

RESPONSE:
{
  "theater_id": 1,
  "movie_id": 123,
  "available_seats": ["a1", "a2", "a3", "b1", "b2"],
  "total_available": 5
}

SEAT NAMING CONVENTION:
- Format: [row_letter][seat_number]
- Rows: a, b, c, d, e... (lowercase letters)
- Seats: 1, 2, 3, 4, 5... (numbers starting from 1)
- Layout: Grid-based, calculated as ceil(sqrt(total_seats)) per row

IMPLEMENTATION NOTES FOR DEVELOPERS:
- Theater uses 5x4 grid layout (20 seats total by default)
- Seat availability checked atomically using std::atomic<bool>
- Response includes both array and count for client convenience
- Empty array returned if theater/movie combination not found

ERROR CONDITIONS:
- Invalid theater_id or movie_id
- Theater doesn't show specified movie
- Missing required parameters

4. BOOK
-------
PURPOSE: Reserve specified seats for a movie showing
SCOPE: Write operation, atomic booking with conflict resolution

REQUEST:
{
  "command": "BOOK",
  "theater_id": 1,
  "movie_id": 123,
  "seats": ["a1", "a2", "b3"]
}

RESPONSE (Success):
{
  "status": "BOOKED",
  "theater_id": 1,
  "movie_id": 123,
  "seats": ["a1", "a2", "b3"],
  "timestamp": 1640995200
}

RESPONSE (Failure):
{
  "status": "FAILED",
  "theater_id": 1,
  "movie_id": 123,
  "seats": ["a1", "a2", "b3"],
  "timestamp": 1640995200
}

BOOKING ALGORITHM:
1. Validate all requested seats exist and are available
2. If any seat is unavailable, entire booking fails (atomic operation)
3. If all seats available, book all seats atomically
4. Return success/failure status with timestamp

IMPLEMENTATION NOTES FOR DEVELOPERS:
- Uses compare_exchange_strong for atomic seat booking
- All-or-nothing booking policy (no partial bookings)
- Thread-safe operation with mutex protection at theater level
- Timestamp is Unix epoch seconds (std::time(nullptr))

ERROR CONDITIONS:
- One or more seats already booked
- Invalid seat IDs (seats don't exist)
- Theater/movie combination not found
- Empty seats array

## Error Handling Reference

### ERROR TYPES AND RESPONSES

1. **UNKNOWN_COMMAND**

{
  "error": "UNKNOWN_COMMAND",
  "received_command": "INVALID_CMD",
  "valid_commands": ["LIST_MOVIES", "LIST_THEATERS", "LIST_SEATS", "BOOK"]
}

2. **INVALID_REQUEST**

{
  "error": "INVALID_REQUEST",
  "message": "JSON parsing error or missing required fields",
  "sample_format": {
    "LIST_MOVIES": {"command": "LIST_MOVIES"},
    "LIST_THEATERS": {"command": "LIST_THEATERS", "movie_id": 123},
    "LIST_SEATS": {"command": "LIST_SEATS", "theater_id": 1, "movie_id": 123},
    "BOOK": {"command": "BOOK", "theater_id": 1, "movie_id": 123, "seats": ["a1"]}
  }
}

HTTP-STYLE STATUS MAPPING:
- Successful operations: Equivalent to HTTP 200 OK
- Invalid requests: Equivalent to HTTP 400 Bad Request  
- Unknown commands: Equivalent to HTTP 404 Not Found
- Server errors: Equivalent to HTTP 500 Internal Server Error

## Backend Implementation guide

### INTERFACE ARCHITECTURE

The system uses dependency injection with abstract interfaces:

CORE INTERFACES:
- IDataStore: Data persistence abstraction
- ITheater: Theater behavior contract
- ISeat: Seat implementation interface  
- IBookingService: Booking operations interface
- IAdministrationService: Admin operations interface

IMPLEMENTATION REQUIREMENTS:
1. All interfaces must be implemented with thread safety
2. Atomic operations required for seat booking
3. Shared/exclusive locking for data consistency
4. Exception safety with RAII principles

THREADING MODEL

- TcpServer uses thread pool for client session handling
- CentralDataStore uses shared_mutex (multiple readers, single writer)
- Theater uses mutex for seat map consistency
- Seat uses atomic<bool> for lock-free booking

MEMORY MANAGEMENT

- Smart pointers throughout (shared_ptr, unique_ptr)
- RAII for resource management
- No manual memory allocation/deallocation
- Exception-safe constructors and destructors

### EXTENDING THE SYSTEM

ADDING NEW SEAT TYPES:
1. Implement ISeat interface
2. Add seat type to Theater::initialize_seats()
3. Update seat creation logic
4. No changes required to booking logic (polymorphic)

ADDING NEW STORAGE BACKENDS:
1. Implement IDataStore interface
2. Maintain thread safety guarantees
3. Preserve atomic booking semantics
4. Update dependency injection in main()

ADDING NEW PROTOCOLS:
1. Create new server class (HttpServer, WebSocketServer)
2. Reuse existing service interfaces
3. Implement protocol-specific request/response handling
4. Maintain same business logic through services

### PERFORMANCE CONSIDERATIONS

- Seat booking: O(1) atomic operation per seat
- Movie listing: O(n log n) for sorting, O(n) for retrieval
- Theater search: O(n) linear search through theaters
- Concurrent clients: Limited by thread pool size

SCALABILITY RECOMMENDATIONS:
- Replace linear theater search with indexed lookup
- Implement connection pooling for high client count
- Add caching layer for frequently accessed data
- Consider database backend for persistence

### TESTING STRATEGY

The system includes comprehensive test coverage:

UNIT TESTS:
- Individual component testing with mock dependencies
- Concurrency stress testing with race condition validation
- Thread safety verification across all components
- Interface contract compliance testing

FUNCTIONAL TESTS:
- End-to-end TCP server integration testing
- JSON protocol validation and error handling
- Concurrent client simulation and booking workflows
- Complete user journey testing

INTEGRATION TESTING APPROACH:
- Use dependency injection for test doubles
- Mock implementations follow interface contracts
- Isolated component testing with controlled dependencies
- Real integration tests with actual TCP connections


## Technical Specifications

- LANGUAGE: C++20
- LIBRARIES: Boost.Asio, Boost.JSON, Google Test
- CONCURRENCY: Thread pool and atomic operations
- NETWORKING: TCP server with persistent connections  
- PROTOCOL: JSON over TCP (newline-delimited)
- TESTING: Unit and functional integration tests
- DEPLOYMENT: Docker containerization with scripts

## Performance Characteristics
---------------------------

- Handles many clients at once using a thread pool
- Lock-free seat booking for high throughput
- Shared/exclusive locking for data consistency
- Smart pointers for efficient memory use
- Scalable, with support for multiple storage backends

This architecture is built for real-world use: it is maintainable, testable, and performs well even with many users at the same time.
