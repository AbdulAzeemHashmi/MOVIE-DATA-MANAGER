# Movie Data Manager (C++ No-STL)

A comprehensive **Movie Management System** built as part of the Data Structures course (Fall 2025) at **FAST-NUCES, Islamabad**. Designed to handle, search, and analyze the IMDb 5000 movie dataset efficiently using custom-built data structures.

> **Key Constraint:** The C++ Standard Template Library (STL) is strictly prohibited. Every data structure, from Linked Lists to Graphs, is implemented from scratch.

---

## Team Members

| Name | Roll Number | Email |
|------|-------------|--------------------|
| Abdul Ghafoor | 24I-0118 | i240118@nu.edu.pk |
| Abdul Azeem | 24I-2013 | i242013@nu.edu.pk |

---

## Core Data Structures

| Structure | Purpose |
|-----------|---------|
| **AVL Tree** | Stores all movie records; guarantees O(log n) search, insertion, and deletion by self-balancing on title key |
| **Hash Table** | Indexes movies by actor, genre, and director for near O(1) lookup; uses chaining for collision resolution |
| **Graph (Adjacency List)** | Models relationships between movies via shared actors/genres; powers the recommendation engine |
| **Linked List** | Backbone collection used across every structure; also drives the custom Stack and Queue |
| **Stack** | Used by DFS traversal for recommendation |
| **Queue** | Used by BFS traversal for recommendation and shortest path |

---

## Features

- **CSV Parser** - Custom-built parser handles quoted fields, escaped characters, and malformed rows while loading 5000+ records from `movie_metadata.csv`
- **Search by Title** - O(log n) AVL tree lookup with case-insensitive, whitespace-trimmed matching
- **Search by Actor / Genre / Director** - Hash table lookup returns all matching movies instantly
- **Search by Year** - Full in-order traversal filtered by release year
- **Search by Rating** - Range-based traversal (e.g., 7.0 to 9.0)
- **BFS Recommendations** - Suggests movies by exploring the closest graph neighbors first (breadth-first)
- **DFS Recommendations** - Follows deep chains of related movies (depth-first) for niche suggestions
- **Shortest Path (Movies)** - Finds the minimum connection between two movies using BFS and parent-pointer backtracking
- **Shortest Path (Actors/Directors)** - Finds how two people are connected through movies they share
- **Co-Actor Finder** - Lists all actors who have appeared in movies alongside a given actor
- **Update Rating** - Modify the rating of any existing movie in place
- **Delete Movie** - Removes a movie from the AVL tree, hash table indexes, and all graph adjacency lists cleanly

---

## Installation and Usage

### Prerequisites

- A C++ compiler supporting C++11 or later (e.g., `g++`)
- `movie_metadata.csv` placed in the same directory as the executable

### 1. Clone the repository

```bash
git clone https://github.com/AbdulAzeemHashmi/MOVIE-DATA-MANAGER.git
cd MOVIE-DATA-MANAGER
```

### 2. Compile

```bash
g++ "24I-0118_24I-2013_DS Project.cpp" -o MovieManager
```

### 3. Run

```bash
./MovieManager
```

---

## Menu Options

```
=== MOVIES MANAGER ===
 1.  Display All
 2.  Search Title
 3.  Search Actor / Genre / Director
 4.  Search Year
 5.  Search Rating
 6.  Recommendations (BFS)
 7.  Recommendations (DFS)
 8.  Shortest Path (Movies)
 9.  Shortest Path (Actors / Directors)
10.  Update Rating
11.  Delete Movie
12.  Find Co-Actors
13.  Exit
```

![Movies Manager Menu](https://github.com/AbdulAzeemHashmi/Movie-Data-Manager/blob/main/Screenshot%20from%202026-06-11%2020-53-20.png)

---

## Project Structure

```
MOVIE-DATA-MANAGER/
-- 24I-0118_24I-2013_DS Project.cpp   # Main source file (all structures + logic)
-- movie_metadata.csv                  # IMDb 5000 dataset (required at runtime)
-- README.md
```

---

## Implementation Notes

- `max_links = 25` caps the number of graph edges created per actor/genre bucket to prevent density explosion on very common names.
- `format_key()` normalizes all search keys to lowercase printable ASCII so searches are case-insensitive.
- AVL deletion uses an in-order successor swap strategy and re-indexes hash table references after the swap.
- Graph edges are bidirectional and created automatically at insert time inside `HashTable::insert_item()`.

---

## Course Info

- **Course:** Data Structures (Fall 2025)
- **University:** FAST-NUCES, Islamabad
