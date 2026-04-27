# Project: DSA Terminal

## Project Overview
A terminal-based application for Data Structures and Algorithms assignments.

## Development Log

### 2026-04-27
- **Workspace Reset:** Reverted the project to a blank state.
- **Initialization:** Created `GEMINI.md` to track project evolution.
- **Step 1: Auth System:**
    - Implemented `registerStudent()` with file persistence (`users.txt`).
    - Implemented `loginStudent()` and `loginAdmin()`.
    - Names are handled as two separate inputs (First and Last).
    - Admin credentials are hardcoded (`admin`/`admin`).
    - Login system asks for user type (Admin/Student).

## Current Goals
1. Add core management functions (linked list operations) for the Admin.
2. Define logic for the Student dashboard.

## Context & Constraints
- **Language:** C++
- **Compiler:** g++ (MinGW/TDM-GCC) - Note: Use `NULL` instead of `nullptr` for compatibility.
- **OS:** Windows (win32)
