# Data Structures Final Assignment

Five C programs covering core data structures: doubly linked list, binary search tree, directed graph, Dijkstra's shortest path, and Huffman compression. Each question is self-contained in its own directory.

## Layout

```
.
├── incident_tracker/    Q1 — Emergency Dispatch (Doubly Linked List)
├── procedure_validator/ Q2 — Maintenance Validator (BST)
├── route_analyzer/      Q3 — Airline Route Analyzer (Directed Graph)
├── delivery_robot/      Q4 — Campus Navigation (Dijkstra)
└── telemetry_compress/  Q5 — Telemetry Compression (Huffman)
```

## Build & Run

Each program is compiled with a single `gcc` command and run from its own directory.

```bash
gcc -Wall -Wextra -o incident_tracker/main incident_tracker/main.c
cd incident_tracker && ./main

gcc -Wall -Wextra -o procedure_validator/main procedure_validator/main.c
cd procedure_validator && ./main

gcc -Wall -Wextra -o route_analyzer/main route_analyzer/main.c
cd route_analyzer && ./main

gcc -Wall -Wextra -o delivery_robot/main delivery_robot/main.c
cd delivery_robot && ./main

gcc -Wall -Wextra -o telemetry_compress/main telemetry_compress/main.c
cd telemetry_compress && ./main
```

## Q1 — Emergency Dispatch Incident Tracker

Implements a **doubly linked list** to store up to 25 emergency incidents in chronological order. An operator navigates the list using single-character commands: `f` (forward to newer), `b` (backward to older), `l` (start live monitoring), `s` (stop live monitoring), `d` (delete all), and `q` (save session and quit). During live monitoring, new incidents are appended automatically every two seconds while navigation remains active. When the list reaches capacity, the oldest incident is evicted before a new one is inserted. The cursor is adjusted automatically if the incident it points to is evicted. On quit, the current incident list is saved to `session.log`.

Header: `incident_list.h` — `Incident` and `IncidentList` types, all list operations, capacity enforcement, file save, and synthetic incident generation.

## Q2 — Secure Maintenance Procedure Validator

Loads approved maintenance procedures from `procedures.txt` into a **binary search tree** at startup. When a technician submits a procedure name, it is looked up in the BST. An exact match approves execution immediately. If no exact match is found, two similarity strategies are available: Levenshtein edit distance (counts the minimum character-level edits needed to reach a known procedure) and BST floor/ceiling (finds the alphabetically adjacent approved procedure using the natural ordering of the tree). Unknown procedures with no close match are rejected and logged with a timestamp to `audit.log`. All BST memory is freed on exit.

Header: `bst.h` — `BSTNode` type, insert, search, in-order traversal, edit distance, floor/ceiling neighbor search, file loading, audit logging, and validation logic.

## Q3 — Airline Route Relationship Analyzer

Models airport connections as a **directed graph** backed by an adjacency matrix. Seven airports and six routes are loaded at startup (KGL, NBO, EBB, JNB, ADD, CAI, CPT). A menu-driven interface supports querying outgoing and incoming routes for any airport, adding and removing airports, adding and removing routes, and printing the full adjacency matrix. Removing an airport zeroes its entire row and column in the matrix and marks its slot inactive, so its index is not reused by accident. The matrix is printed dynamically, showing only active airports. Up to 20 airports are supported via `MAX_AIRPORTS`.

Header: `graph.h` — `Vertex` and `RouteGraph` types, all graph operations, and matrix display.

## Q4 — Campus Delivery Robot Navigation

Represents a university campus as a **weighted undirected graph** (adjacency matrix) with 7 buildings and 8 paths. Dijkstra's algorithm computes the shortest route from a user-selected starting building to the Dormitory. The algorithm uses an O(V²) array-based approach suited to the fixed graph size. Input is case-insensitive; invalid building names produce an error with the list of valid options. The program prints the full path and total travel distance.

Header: `graph.h` — `CampusGraph` and `DijkstraResult` types, node and edge management, Dijkstra's algorithm, and path printing.

Expected shortest distances to Dormitory:

| Start            | Distance | Path                                                                  |
|------------------|----------|-----------------------------------------------------------------------|
| Library          | 15       | Library → Cafeteria → Charging Station → Administration → Dormitory  |
| Cafeteria        | 9        | Cafeteria → Charging Station → Administration → Dormitory            |
| Engineering      | 8        | Engineering → Administration → Dormitory                             |
| Science Block    | 8        | Science Block → Dormitory                                            |
| Administration   | 3        | Administration → Dormitory                                           |
| Charging Station | 7        | Charging Station → Administration → Dormitory                        |

## Q5 — Telemetry Data Compression Utility

Implements **Huffman coding** to compress and decompress a telemetry log file. The program reads `telemetry.txt`, builds a character frequency table, constructs a Huffman tree using a min-selection loop, generates variable-length bit codes for each character, and packs the coded bitstream into `telemetry.huff`. The `.huff` format stores a compact header containing only the non-zero frequency entries (symbol + count pairs), followed by the total bit count and the packed bitstream. Decompression reads the header, reconstructs an identical Huffman tree from the saved frequencies, and decodes the bitstream bit-by-bit back into `telemetry_restored.txt`. The program reports original size, compressed size, and compression ratio, then verifies that the restored file matches the original byte-for-byte.

Headers: `huffman.h` — `HuffNode`, `HuffTree`, `HuffCode`, and `HuffCodeTable` types, tree construction and code generation. `codec.h` — compression, decompression, file save/load, statistics reporting, and verification.
