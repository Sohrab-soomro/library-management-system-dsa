# Library Management System — C++ (PF & Data Structures)

![C++](https://img.shields.io/badge/Language-C%2B%2B17-00599C?logo=c%2B%2B&logoColor=white)
![Data Structures](https://img.shields.io/badge/DSA-BST%20%7C%20Linked%20List%20%7C%20Queue%20%7C%20Stack-1F8A55)
![University](https://img.shields.io/badge/FAST%20NUCES-Peshawar-D6112C)

A C++ **Library Management System** engineered using core **Programming Fundamentals (PF)** and **Data Structures & Algorithms (DSA)** concepts to manage book inventories, student/faculty members, borrowing lifecycles, reservation waitlists, and persistent CSV records.

---

## Data Structures & Algorithmic Complexity

| Module / Operation | Underlying Data Structure | Time Complexity (Avg) | Purpose |
| :--- | :--- | :---: | :--- |
| **Book Catalog & Lookup** | **Binary Search Tree (`BookNode*`)** | $\mathcal{O}(\log n)$ | Fast insertion, ISBN/ID search, and sorted in-order catalog printing |
| **Member Registry** | **Singly Linked List (`MemberNode*`)** | $\mathcal{O}(n)$ | Dynamic registration and active loan tracking without fixed array bounds |
| **Book Reservation Waitlist** | **FIFO Queue (`std::queue<int>`)** | $\mathcal{O}(1)$ | Fair first-come-first-served auto-assignment when a checked-out book is returned |
| **Transaction Audit Trail** | **LIFO Stack (`std::stack<string>`)** | $\mathcal{O}(1)$ | Chronological history of recent issue, return, and waitlist events |
| **Catalog Persistence** | **File Streams (`std::ofstream`)** | $\mathcal{O}(n)$ | Exports the sorted BST catalog to `catalog_export.csv` |

---

## System Workflow

```mermaid
flowchart LR
    A["Student Requests Book ID"] --> B{"Search BST Catalog O(log n)"}
    B -- "Available > 0" --> C["Decrement Copy & Issue to Member"]
    B -- "Available == 0" --> D["Enqueue Student ID in FIFO Waitlist"]
    E["Student Returns Book"] --> F{"Waitlist Queue Empty?"}
    F -- "No" --> G["Dequeue Next Student & Auto-Issue Book"]
    F -- "Yes" --> H["Increment Available Copies in BST"]
```

---

## How to Compile & Run

```bash
g++ src/LibraryManagementSystem.cpp -o library_dsa
./library_dsa
```
