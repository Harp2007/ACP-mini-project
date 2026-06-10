# Walkthrough - Advanced 2D Graphics Editor

We have successfully designed, implemented, and verified the Advanced 2D Graphics Editor.

---

## 1. Project Reference & Structure

### Code Files
*   **Main C Editor:** [editor.c](file:///c:/Users/harpr/OneDrive/Documents/Desktop/ACP%20project/editor.c)
*   **Linux NCurses Enhancement:** [editor_ncurses.c](file:///c:/Users/harpr/OneDrive/Documents/Desktop/ACP%20project/editor_ncurses.c)
*   **Automated Test Suite:** [test_input.txt](file:///c:/Users/harpr/OneDrive/Documents/Desktop/ACP%20project/test_input.txt)
*   **Semester Report:** [report.md](file:///c:/Users/harpr/OneDrive/Documents/Desktop/ACP%20project/report.md)

---

## 2. Editor Mechanics & Layout

### The Coordinate System
The editor uses a standard computer graphics coordinate grid:
*   **Resolution:** 60 columns wide by 20 rows high.
*   **X-Axis:** Ranges from `0` (leftmost column) to `59` (rightmost column).
*   **Y-Axis:** Ranges from `0` (topmost row) to `19` (bottommost row).
*   **Origin (0,0):** Located at the **top-left** corner of the canvas.
*   **Clipping:** Any coordinate coordinates that fall outside the `0-59` and `0-19` ranges are automatically clipped (ignored) during rendering by the `set_pixel` function to prevent array indexing overflows or program crashes.

```
(0,0)  X increases rightwards --->      (59,0)
  +---------------------------------------+
  |                                       |
Y |                                       |
  |                                       |
v |                                       |
  +---------------------------------------+
(0,19)                                  (59,19)
```

### Screen Layout
When you select Option `6` (Display Canvas), the program renders:
1.  **Top Border:** A cyan border outline starting with `+` and filled with `-`.
2.  **Drawing Grid:** The 20x60 grid area. Inactive pixels render as dark gray underscores (`_`) to form a coordinate grid, while active pixels render as bright yellow asterisks (`*`).
3.  **Side Borders:** Left and right edges of the canvas are marked by cyan pipes (`|`).
4.  **Bottom Border:** Bounded by `+` and `-`.

### The Add Shape Menu
When selecting Option `1. Add a Shape` from the main menu, the following choices are presented:
```
=== Add Shape ===
1. Line
2. Rectangle
3. Circle
4. Triangle
5. Diamond
6. Square
7. Right Triangle
8. Filled Rectangle
9. Back to Main Menu
Select Shape Type:
```

---

## 3. Cool Example Shapes to Try

Here are a few quick coordinates you can enter sequentially to build beautiful shapes:

### Example A: A House with a Triangular Roof
1.  **Add Filled Rectangle** (House Body):
    *   Top-Left X = `20`, Top-Left Y = `8`
    *   Width = `20`, Height = `10`
2.  **Add Triangle** (Roof):
    *   V1 X = `19`, V1 Y = `7`
    *   V2 X = `30`, V2 Y = `1`
    *   V3 X = `40`, V3 Y = `7`
3.  **Add Line** (Door Outline):
    *   Start X = `28`, Start Y = `12`
    *   End X = `32`, End Y = `12`

### Example B: Concentric Geometric Target
1.  **Add Circle** (Outer Rim):
    *   Center X = `30`, Center Y = `10`
    *   Radius = `8`
2.  **Add Diamond** (Mid Ring):
    *   Center X = `30`, Center Y = `10`
    *   Radius = `5`
3.  **Add Square** (Inner core):
    *   Top-Left X = `28`, Top-Left Y = `9`
    *   Side Length = `5`

---

## 4. Compilation & Verification

### Compilation on Windows
```cmd
& "C:\Users\harpr\AppData\Local\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260602-ucrt-x86_64\bin\clang.exe" -O2 editor.c -o editor.exe
```

### Verification Results
We verified execution by running:
```cmd
cmd.exe /c "editor.exe < test_input.txt"
```
1.  **Creation:** All 8 shapes successfully registered inside the shape array database (IDs 1 through 8).
2.  **Moving:** Shape 2 (Rectangle) shifted from `(5, 2)` to `(10, 5)`.
3.  **Resizing:** Shape 3 (Circle) radius updated from `4` to `6`.
4.  **Rendering:** Overlapping shapes merged naturally on the canvas.
5.  **Deletion:** Dynamic recalculation of coordinates upon removal of Shape ID 3.
6.  **Clearing:** Canvas wiped cleanly when requested, returning "No shapes drawn yet".
