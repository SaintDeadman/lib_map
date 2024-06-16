# lib_map

This project is a part of assessment assignment. Description of task you can find in doc/task.md

# About struct of project
The project is divided into two main parts:
- Table Implementations: Located in the map_lib folder.
- Tests: Located in the test folder

# Table Implementations
Three types of tables have been implemented:

1) *Mock Table*: This is a stub table used to implement the default map API. It serves as a basic placeholder for testing purposes.
2) *Simple Table*: This table acts as a reference implementation with open addressing. It is used for comparison and validation of the main table.
3) *Double Table*: This is the main table implementation, featuring double hashing for improved performance and collision handling.

# Tests
Two types of tests have been implemented to ensure the correctness and performance of the tables:

1) *Behavior Tests*: These tests verify the functionality of the table APIs. They ensure that all operations perform as expected according to the defined API.
2) *Measuring Tests*: These are more complex tests designed to measure the execution time of the table APIs. They evaluate performance in both single-threaded and multi-threaded modes, providing insights into the efficiency and scalability of the tables.

# How to Build it
To build the project, follow these steps:
```bash
mkdir build
cd build
cmake ..
make
```

# How to Run Test
To run the tests, use the following commands:

```bash
./bin/run_test_00
./bin/run_test_01
./bin/run_messuring_simple
./bin/run_messuring_double
```

# Constraints
- The project has not been tested on Windows. It is likely that it won't build on this platform.


