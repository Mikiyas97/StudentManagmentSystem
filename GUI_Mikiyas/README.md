# Student Management System

A desktop graphical user interface (GUI) application for managing student data, courses, grades, and generating reports. This project is built using C++ and the Qt 6 framework, utilizing a structured SQLite database for robust data persistence.

## System Modules

### 1. Administrator Module
The administrator manages the overall system and school records.
- **User Management**: Add, update, and remove records for students and teachers.
- **Student Records**: Store and manage student information such as name, ID, age, and class.
- **Course Management**: Add, update, and remove academic courses and curriculum details.
- **Enrollment Management**: Manage student enrollments into courses and classes.
- **System Reports**: View and generate reports about student performance.

### 2. Academic / Teacher Module
Teachers manage academic information related to students.
- **Grade Management**: Enter student grades for assignments, tests, and exams.
- **Academic Records**: View and update student academic information.

### 3. Student Information Access
Students can view important information about themselves and profile details.
- **Academic Information**: View grades and academic performance.

## Technology Stack

- **Language**: C++17
- **Framework**: Qt 6 (Core, GUI, Widgets)
- **Database**: SQLite (`QtSql` module)

## Project Structure

- `managers/`: Contains the core business logic and data access classes (`studentmanager`, `grademanager`, `coursemanager`, `reportmanager`).
- `pages/`: Contains the UI view components and dialogs corresponding to each feature module.
- `SchoolGUI.pro`: The qmake project configuration file.

## Requirements

To build and run this project, you will need:
- Qt 6.x (with the `sql` module installed)
- A C++17 compatible compiler (e.g., MSVC, GCC, Clang)
- Qt Creator (Recommended IDE)

## Build Instructions

### Using Qt Creator
1. Open Qt Creator.
2. Go to **File > Open File or Project...** and select the `SchoolGUI.pro` file.
3. Configure the project with your installed Qt 6 kit.
4. Click the **Build** button (or press `Ctrl+B`) to compile the project.
5. Click the **Run** button (or press `Ctrl+R`) to launch the application.

### Using Command Line (qmake)
Navigate to the project directory in your terminal and run:
```bash
qmake SchoolGUI.pro
make        # or nmake/mingw32-make depending on your compiler/OS
```

## Making Changes
> **Note:** If you are making changes to this codebase, please ensure that you read through this README file first to understand the project structure and dependencies. If you have any questions or uncertainties regarding architectural modifications (especially database schema changes or UI flows), please ask for clarification before proceeding with implementations.
