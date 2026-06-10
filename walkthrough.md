# Walkthrough - Advanced 2D Graphics Editor

We have successfully designed, implemented, and verified the Advanced 2D Graphics Editor.

## Changes Made

### 1. Main C Editor: [editor.c](file:///c:/Users/harpr/OneDrive/Documents/Desktop/ACP%20project/editor.c)
- **8-Shape Drawing Core**:
  - **Line**: Bresenham's line algorithm.
  - **Rectangle**: Outlines borders using horizontal and vertical coordinates.
  - **Circle**: Bresenham's Midpoint Circle Algorithm.
  - **Triangle**: Outlines by connecting three arbitrary coordinate points.
  - **Diamond**: Outlines using 4 diagonal lines centered around a point.
  - **Square**: Outlines a square using the core rectangle method with equal width and height.
  - **Right Triangle**: Draws horizontal base, vertical height, and connecting diagonal hypotenuse.
  - **Filled Rectangle**: Uses nested loops to fill the bounds of a rectangle with `*`.
- **Comprehensive Database Operations**:
  - **Add Shape**: Asks shape parameters and assigns an incremental ID.
  - **Delete Shape**: Removes shapes by ID and shifts the array.
  - **Modify Shape**: Rewrites coordinates/sizes for an existing shape ID.
  - **Move Shape**: Shifts coordinates of any shape to a new location.
  - **Resize Shape**: Changes size parameters (radius, side, width, height) of an existing shape.
  - **Clear Canvas**: Resets the database, removing all shapes.
  - **List Shapes**: Lists details of all drawn shapes.
  - **Display Canvas**: Re-renders the canvas with borders and ANSI colors.
- **Beginner-Friendly Architecture**:
  - Employs a flat structure `Shape` avoiding pointers and unions.
  - Features robust integer validation using custom buffer reads.

### 2. Optional Enhancement: [editor_ncurses.c](file:///c:/Users/harpr/OneDrive/Documents/Desktop/ACP%20project/editor_ncurses.c)
- A Linux-based split-window Terminal UI (TUI).
- Uses `ncurses.h` for window handling, custom key listeners (`a` to add, `d` to delete, `m` to move, `r` to resize, `l` to list, `c` to clear, `q` to quit), and live drawing displays.

### 3. Automated Test Suite: [test_input.txt](file:///c:/Users/harpr/OneDrive/Documents/Desktop/ACP%20project/test_input.txt)
- Simulated menu keystrokes to draw all 8 shapes, list them, render them, perform move/resize shifts, verify deletion/clearing, and exit.

### 4. Semester Report: [report.md](file:///c:/Users/harpr/OneDrive/Documents/Desktop/ACP%20project/report.md)
- Contains all 11 required sections including Problem Statement, Algorithms (pseudo-code), Data Structures, Function Descriptions, and Limitations.

---

## Compilation & Verification

### Compilation on Windows
The project compiles with standard compilers:
```cmd
& "C:\Users\harpr\AppData\Local\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260602-ucrt-x86_64\bin\clang.exe" -O2 editor.c -o editor.exe
```

### Verification Results
We ran the compiled binary with `test_input.txt` to verify execution:
```cmd
cmd.exe /c "editor.exe < test_input.txt"
```
1. **Creation**: All 8 shapes successfully registered inside the shape array database (IDs 1 through 8).
2. **Moving**: Shape 2 (Rectangle) shifted from `(5, 2)` to `(10, 5)`.
3. **Resizing**: Shape 3 (Circle) radius updated from `4` to `6`.
4. **Rendering**: Overlapping shapes (Line, Rectangle, Circle, Diamond, Square, Triangle, Filled Rectangle, Right Triangle) merged on the canvas, showing a multi-shape vector layout.
5. **Deletion**: Circle (ID 3) was deleted, and the canvas dynamically recalculated the picture without it.
6. **Clearing**: Canvas wiped cleanly when requested, returning "No shapes drawn yet".
7. **Exit**: Gracefully terminated.
