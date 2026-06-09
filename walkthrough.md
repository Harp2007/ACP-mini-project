# Walkthrough: 2D Graphics Editor in C

We have successfully designed, implemented, and verified the menu-driven 2D Graphics Editor in C. The editor allows users to interactively add, modify, delete, and display shapes on a 2D ASCII canvas.

## Changes Made

### 1. Created [editor.c](file:///c:/Users/harpr/OneDrive/Documents/Desktop/ACP%20project/editor.c)
- **Canvas System**:
  - Implemented a 2D character array of size 20 rows by 60 columns.
  - Initialized with `_` representing empty space and `*` representing pixels drawn by shapes.
- **Rendering & Math**:
  - **Bresenham's Line Algorithm**: Implemented for drawing high-precision pixel lines.
  - **Bresenham's Midpoint Circle Algorithm**: Implemented to render smooth circle outlines without floating point math.
  - **Rectangle Draw**: Draws top, bottom, left, and right lines outlining a bounding box.
  - **Triangle Draw**: Connects 3 vertices using 3 line drawing operations.
- **Shape Management**:
  - Employs a vector-like structure list to track active shapes.
  - Supports adding shapes, listing active shapes, modifying parameters by shape ID, and deleting shapes by ID.
  - Each edit clears the canvas and redraws active shapes, enabling simple delete and modify operations.
- **Aesthetic Enhancements**:
  - Styled using ANSI escape codes: borders are colored in cyan, shapes in bright yellow, empty spaces in dimmed gray, and menus/errors in distinct, readable colors.
  - Added Windows Console virtual terminal processing initialization so ANSI colors render correctly on Windows CLI.

---

## Compilation

The project compiles on Windows with the self-contained LLVM Clang compiler:
```cmd
& "C:\Users\harpr\AppData\Local\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260602-ucrt-x86_64\bin\clang.exe" -O2 editor.c -o editor.exe
```

---

## Validation Results

We performed automated keyboard-simulation tests by feeding a sequence of inputs to `editor.exe`. 

### 1. Canvas Rendering with Multiple Shapes
When we added a line, rectangle, circle, and triangle to the canvas:
- The line, rectangle, and triangle rendered precisely.
- Overlapping regions merged naturally on the canvas.
- Display output was highlighted with color (bright yellow for `*` and dark gray for `_`).

### 2. Modifying Shapes
- We successfully modified Shape ID 2 (Rectangle) from `Top-Left (5, 2), Width 12, Height 6` to `Top-Left (10, 5), Width 8, Height 4`.
- The canvas recalculated the pixels automatically.

### 3. Deleting Shapes
- We successfully deleted Shape ID 3 (Circle).
- The circle was removed from the active list, and the canvas was re-rendered, leaving other shapes intact.

---

## How to Run & Interact

1. In PowerShell or Command Prompt, run the executable:
   ```cmd
   .\editor.exe
   ```
2. You will be greeted with a visual menu:
   ```
   ==================================
         ASCII 2D GRAPHICS EDITOR    
   ==================================
   1. Add a Shape
   2. Modify an Existing Shape
   3. Delete a Shape
   4. Display Canvas
   5. List Active Shapes
   6. Exit
   ----------------------------------
   Select Option (1-6):
   ```
3. Use the menu choices to build your drawing and view it using Option 4.
