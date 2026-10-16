# Keyword File Indexer

A command-line **file indexing and search system in C**, combining a **trie**, a **binary max-heap**, and a **doubly linked list** to support keyword lookup, relevance ranking, and prefix search.

> **Context:** Developed for a university Data Structures course. This repository contains my solution rather than the course assignment PDF, grading checker, or supplied test data. Source files credit the laboratory example used for an error-handling macro.

## Features

- Add and delete indexed file records, each with an ID, relevance score, and keywords.
- Add or remove keywords while keeping the index updated.
- Find all files associated with a keyword (`FIND`).
- Retrieve the highest-ranked **k** matching files (`TOPK`).
- Search for files associated with any keyword beginning with a prefix (`PREFIX`, an optional extension of the assignment).
- Print the keyword index (`PRINT`).

## How the data structures work

| Structure | Role |
| --- | --- |
| **Trie** | Stores lowercase a–z keywords and references to matching files |
| **Binary max-heap** | Extracts top-ranked files for a keyword; higher scores rank first, with lexicographical file-ID tie-breaking |
| **Doubly linked list** | Stores the file records and supports insertion and removal |
| **Dynamically allocated arrays** | Store per-file keyword lists and collect distinct prefix matches |

The index is updated when files or their keywords are removed, rather than rebuilding the entire structure for every query.

## Build and run

Requires GCC (C99) and `make`. The program reads **`indexare.in`** and writes **`indexare.out`** in the current working directory.

```bash
make build
cp examples/demo.in indexare.in
./search_index
cat indexare.out
```

On Windows, WSL with GCC and Make is a straightforward option. In Windows Command Prompt, `copy examples\demo.in indexare.in` can replace `cp` if your installed toolchain provides the remaining commands.

A reproducible example is included in [`examples/demo.in`](examples/demo.in) with its corresponding [`expected output`](examples/demo.expected.txt).

## Commands

| Command | Behavior |
| --- | --- |
| `ADD` | Insert a file with a score and list of keywords |
| `DEL` | Remove a file and its index references |
| `ADDKW` / `DELKW` | Add or remove a keyword from an existing file |
| `FIND` | List file IDs associated with a given keyword |
| `TOPK` | Return up to k matching files ranked by relevance |
| `PREFIX` | Return distinct matching files for a keyword prefix |
| `PRINT` | Print the indexed keywords and their references |

Input uses whitespace-delimited commands and the record format specified by the original assignment. Keywords are expected to consist of **lowercase English letters (a–z)**; other character sets are not implemented.

## Project structure

- `main.c` — parses commands and coordinates updates and queries.
- `file.c/.h` — records, keyword arrays, and the linked list.
- `trie.c/.h` — keyword indexing, reference management, prefix traversal, and search.
- `heap.c/.h` — priority comparison and max-heap insertion/extraction.
- `Makefile` — build, run, and clean targets.
- `examples/` — newly created demonstration input and expected output.

## Verification and scope

The unchanged implementation **compiled with GCC** and matched **80 of 80** supplied course reference-output tests in a local review, including prefix-query cases. The course tests are intentionally not redistributed. This is not a performance benchmark or exhaustive correctness or memory-safety audit.
