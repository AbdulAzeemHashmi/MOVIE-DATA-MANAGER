<div align="center">

# 🎬 Movie Data Manager

### C++ No STL Edition

<img src="https://readme-typing-svg.demolab.com/?font=Fira+Code&size=22&pause=1000&color=F75C7E&center=true&vCenter=true&width=600&lines=Custom+AVL+Trees+%2B+Hash+Tables+%2B+Graphs;Zero+STL.+Pure+Data+Structures.;Powering+the+IMDb+5000+Dataset+%F0%9F%8E%A5" alt="Typing SVG" />

[![C++](https://img.shields.io/badge/C%2B%2B-11%2B-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![No STL](https://img.shields.io/badge/STL-Not%20Used-red?style=for-the-badge&logo=cachet&logoColor=white)]()
[![University](https://img.shields.io/badge/FAST%20NUCES-Islamabad-1e3a8a?style=for-the-badge)]()
[![Stars](https://img.shields.io/github/stars/AbdulAzeemHashmi/Movie-Data-Manager?style=for-the-badge&color=yellow)](https://github.com/AbdulAzeemHashmi/Movie-Data-Manager/stargazers)
[![Forks](https://img.shields.io/github/forks/AbdulAzeemHashmi/Movie-Data-Manager?style=for-the-badge&color=orange)](https://github.com/AbdulAzeemHashmi/Movie-Data-Manager/network)

📚 A comprehensive **Movie Management System** built for the Data Structures course (Fall 2025) at **FAST NUCES, Islamabad**. Every structure, from Linked Lists to Graphs, is handcrafted from scratch to search, index, and analyze the IMDb 5000 movie dataset.

</div>

---

## ⚠️ Key Constraint

> 🚫 The C++ Standard Template Library (STL) is strictly prohibited. AVL Trees, Hash Tables, Graphs, Linked Lists, Stacks, and Queues are all implemented manually.

---

## 👥 Team Members

| 🧑‍💻 Name | 🆔 Roll Number | 📧 Email |
|---|---|---|
| Abdul Ghafoor | 24I 0118 | i240118@nu.edu.pk |
| Abdul Azeem | 24I 2013 | i242013@nu.edu.pk |

---

## 🧠 Core Data Structures

| 🏗️ Structure | 🎯 Purpose |
|---|---|
| 🌳 **AVL Tree** | Stores all movie records, guarantees O(log n) search, insertion, and deletion by self balancing on title key |
| 🔑 **Hash Table** | Indexes movies by actor, genre, and director for near O(1) lookup, uses chaining for collision resolution |
| 🕸️ **Graph (Adjacency List)** | Models relationships between movies via shared actors and genres, powers the recommendation engine |
| 🔗 **Linked List** | Backbone collection used across every structure, also drives the custom Stack and Queue |
| 📥 **Stack** | Used by DFS traversal for recommendation |
| 📤 **Queue** | Used by BFS traversal for recommendation and shortest path |

---

## ✨ Features

- 📄 **CSV Parser** — Custom built parser handles quoted fields, escaped characters, and malformed rows while loading 5000+ records from `movie_metadata.csv`
- 🔍 **Search by Title** — O(log n) AVL tree lookup with case insensitive, whitespace trimmed matching
- 🎭 **Search by Actor, Genre, or Director** — Hash table lookup returns all matching movies instantly
- 📅 **Search by Year** — Full in order traversal filtered by release year
- ⭐ **Search by Rating** — Range based traversal (e.g. 7.0 to 9.0)
- 🌐 **BFS Recommendations** — Suggests movies by exploring the closest graph neighbors first (breadth first)
- 🕳️ **DFS Recommendations** — Follows deep chains of related movies (depth first) for niche suggestions
- 🛤️ **Shortest Path (Movies)** — Finds the minimum connection between two movies using BFS and parent pointer backtracking
- 🤝 **Shortest Path (Actors/Directors)** — Finds how two people are connected through movies they share
- 🎬 **Co Actor Finder** — Lists all actors who have appeared in movies alongside a given actor
- ✏️ **Update Rating** — Modify the rating of any existing movie in place
- 🗑️ **Delete Movie** — Removes a movie from the AVL tree, hash table indexes, and all graph adjacency lists cleanly

---

## 🚀 Installation and Usage

### ✅ Prerequisites

- A C++ compiler supporting C++11 or later (e.g. `g++`)
- `movie_metadata.csv` placed in the same directory as the executable

### 1️⃣ Clone the repository

```bash
git clone https://github.com/AbdulAzeemHashmi/Movie-Data-Manager.git
cd Movie-Data-Manager
```

### 2️⃣ Compile

```bash
g++ "24I-0118_24I-2013_DS Project.cpp" -o MovieManager
```

### 3️⃣ Run

```bash
./MovieManager
```

---

## 📋 Menu Options

```
🎬 === MOVIES MANAGER === 🎬
 1.  📃 Display All
 2.  🔍 Search Title
 3.  🎭 Search Actor / Genre / Director
 4.  📅 Search Year
 5.  ⭐ Search Rating
 6.  🌐 Recommendations (BFS)
 7.  🕳️ Recommendations (DFS)
 8.  🛤️ Shortest Path (Movies)
 9.  🤝 Shortest Path (Actors / Directors)
10.  ✏️ Update Rating
11.  🗑️ Delete Movie
12.  🎬 Find Co Actors
13.  🚪 Exit
```

<div align="center">

![Movies Manager Menu](https://github.com/AbdulAzeemHashmi/Movie-Data-Manager/blob/main/Screenshot%20from%202026-06-11%2020-53-20.png)

</div>

---

## 🗂️ Project Structure

```
MOVIE-DATA-MANAGER/
├── 24I-0118_24I-2013_DS Project.cpp   # Main source file (all structures + logic)
├── movie_metadata.csv                  # IMDb 5000 dataset (required at runtime)
└── README.md
```

---

## 🛠️ Implementation Notes

- 🔢 `max_links = 25` caps the number of graph edges created per actor/genre bucket to prevent density explosion on very common names.
- 🔡 `format_key()` normalizes all search keys to lowercase printable ASCII so searches are case insensitive.
- 🔄 AVL deletion uses an in order successor swap strategy and re indexes hash table references after the swap.
- 🔗 Graph edges are bidirectional and created automatically at insert time inside `HashTable::insert_item()`.

---

## 🎓 Course Info

- 📘 **Course:** Data Structures (Fall 2025)
- 🏛️ **University:** FAST NUCES, Islamabad

---

<div align="center">

### 🌟 If you found this project useful, consider giving it a star!

[![GitHub followers](https://img.shields.io/github/followers/AbdulAzeemHashmi?style=social)](https://github.com/AbdulAzeemHashmi)
[![Star History Chart](https://img.shields.io/github/stars/AbdulAzeemHashmi/Movie-Data-Manager?style=social)](https://github.com/AbdulAzeemHashmi/Movie-Data-Manager)

Made with 💙 and a lot of pointer chasing by [Abdul Azeem](https://github.com/AbdulAzeemHashmi) and Abdul Ghafoor

</div>
