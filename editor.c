#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

// Canvas dimensions and constraints
#define HEIGHT 20
#define WIDTH 60
#define MAX_SHAPES 100

// ANSI Style Escape Codes for colorful CLI aesthetics
#define COLOR_RESET   "\x1b[0m"
#define COLOR_DIM     "\x1b[90m" // Dark gray for background canvas grid
#define COLOR_BRIGHT  "\x1b[93m" // Bright yellow for drawings '*'
#define COLOR_BORDER  "\x1b[36m" // Cyan for editor panels and borders
#define COLOR_ERROR   "\x1b[31m" // Red for error messages
#define COLOR_SUCCESS "\x1b[32m" // Green for success messages
#define COLOR_INFO    "\x1b[34m" // Blue for information listing

// Shape Types definition
typedef enum {
    SHAPE_LINE = 1,
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE,
    SHAPE_DIAMOND,
    SHAPE_SQUARE,
    SHAPE_RIGHT_TRIANGLE,
    SHAPE_FILLED_RECTANGLE
} ShapeType;

// Flat Structure representing a Shape (Beginner-friendly, no complex pointers or unions)
typedef struct {
    int id;
    ShapeType type;
    int x1, y1;         // Primary point (Start X/Y, Top-Left, Center, Vertex 1)
    int x2, y2;         // Secondary point (End X/Y, Vertex 2)
    int x3, y3;         // Tertiary point (Vertex 3)
    int width, height;  // Dimensions for Rectangles, Triangles
    int radius;         // Radius for Circle/Diamond
    int side;           // Side length for Square
} Shape;

// Global Editor State variables
Shape shapes[MAX_SHAPES];
int shape_count = 0;
int next_id = 1;
char canvas[HEIGHT][WIDTH];

// Enable virtual terminal processing on Windows for ANSI colors support
void enable_ansi() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

// Canvas rendering helper functions
void clear_canvas() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            canvas[y][x] = '_';
        }
    }
}

// Plots a character on the canvas with boundary checks (Clipping)
void set_pixel(int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        canvas[y][x] = '*';
    }
}

// 1. Bresenham's Line Algorithm
void draw_line(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    
    while (1) {
        set_pixel(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// 2. Rectangle Outline Function
void draw_rectangle(int x, int y, int w, int h) {
    // Top border
    for (int i = 0; i < w; i++) set_pixel(x + i, y);
    // Bottom border
    for (int i = 0; i < w; i++) set_pixel(x + i, y + h - 1);
    // Left border
    for (int j = 0; j < h; j++) set_pixel(x, y + j);
    // Right border
    for (int j = 0; j < h; j++) set_pixel(x + w - 1, y + j);
}

// 3. Bresenham's Midpoint Circle Algorithm
void draw_circle(int cx, int cy, int r) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    
    while (y >= x) {
        set_pixel(cx + x, cy + y);
        set_pixel(cx - x, cy + y);
        set_pixel(cx + x, cy - y);
        set_pixel(cx - x, cy - y);
        set_pixel(cx + y, cy + x);
        set_pixel(cx - y, cy + x);
        set_pixel(cx + y, cy - x);
        set_pixel(cx - y, cy - x);
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
    }
}

// 4. Triangle Drawing Function (Connects three arbitrary points)
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(x1, y1, x2, y2);
    draw_line(x2, y2, x3, y3);
    draw_line(x3, y3, x1, y1);
}

// 5. Diamond Drawing Function (4 diagonal borders from center)
void draw_diamond(int cx, int cy, int r) {
    draw_line(cx, cy - r, cx + r, cy);
    draw_line(cx + r, cy, cx, cy + r);
    draw_line(cx, cy + r, cx - r, cy);
    draw_line(cx - r, cy, cx, cy - r);
}

// 6. Square Drawing Function
void draw_square(int x, int y, int side) {
    draw_rectangle(x, y, side, side);
}

// 7. Right-Angled Triangle Drawing Function
void draw_right_triangle(int x, int y, int w, int h) {
    draw_line(x, y, x + w - 1, y);                  // Horizontal base leg
    draw_line(x, y, x, y + h - 1);                  // Vertical height leg
    draw_line(x + w - 1, y, x, y + h - 1);          // Hypotenuse leg
}

// 8. Filled Rectangle Drawing Function
void draw_filled_rectangle(int x, int y, int w, int h) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            set_pixel(x + i, y + j);
        }
    }
}

// Rebuild and clear canvas from shapes array database (Redraw everything)
void render_canvas() {
    clear_canvas();
    for (int i = 0; i < shape_count; i++) {
        Shape s = shapes[i];
        switch (s.type) {
            case SHAPE_LINE:
                draw_line(s.x1, s.y1, s.x2, s.y2);
                break;
            case SHAPE_RECTANGLE:
                draw_rectangle(s.x1, s.y1, s.width, s.height);
                break;
            case SHAPE_CIRCLE:
                draw_circle(s.x1, s.y1, s.radius);
                break;
            case SHAPE_TRIANGLE:
                draw_triangle(s.x1, s.y1, s.x2, s.y2, s.x3, s.y3);
                break;
            case SHAPE_DIAMOND:
                draw_diamond(s.x1, s.y1, s.radius);
                break;
            case SHAPE_SQUARE:
                draw_square(s.x1, s.y1, s.side);
                break;
            case SHAPE_RIGHT_TRIANGLE:
                draw_right_triangle(s.x1, s.y1, s.width, s.height);
                break;
            case SHAPE_FILLED_RECTANGLE:
                draw_filled_rectangle(s.x1, s.y1, s.width, s.height);
                break;
        }
    }
}

// Display canvas buffer to stdout
void display_canvas() {
    render_canvas();
    
    // Header border
    printf(COLOR_BORDER "+");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n" COLOR_RESET);
    
    // Canvas grid
    for (int y = 0; y < HEIGHT; y++) {
        printf(COLOR_BORDER "|" COLOR_RESET);
        for (int x = 0; x < WIDTH; x++) {
            char c = canvas[y][x];
            if (c == '*') {
                printf(COLOR_BRIGHT "%c" COLOR_RESET, c);
            } else {
                printf(COLOR_DIM "%c" COLOR_RESET, c);
            }
        }
        printf(COLOR_BORDER "|\n" COLOR_RESET);
    }
    
    // Footer border
    printf(COLOR_BORDER "+");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n" COLOR_RESET);
}

// Database helper functions
int add_shape(Shape s) {
    if (shape_count >= MAX_SHAPES) {
        return -1;
    }
    s.id = next_id++;
    shapes[shape_count++] = s;
    return s.id;
}

int delete_shape(int id) {
    int index = -1;
    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].id == id) {
            index = i;
            break;
        }
    }
    if (index == -1) return -1;
    
    // Shift elements left
    for (int i = index; i < shape_count - 1; i++) {
        shapes[i] = shapes[i + 1];
    }
    shape_count--;
    return 0;
}

int modify_shape(int id, Shape updated) {
    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].id == id) {
            updated.id = id; // Preserve id
            shapes[i] = updated;
            return 0;
        }
    }
    return -1;
}

void list_shapes() {
    printf(COLOR_INFO "\nActive Shapes in Picture:\n" COLOR_RESET);
    if (shape_count == 0) {
        printf("  (No shapes drawn yet)\n");
        return;
    }
    for (int i = 0; i < shape_count; i++) {
        Shape s = shapes[i];
        switch (s.type) {
            case SHAPE_LINE:
                printf("  [%d] Line: Start (%d, %d) to End (%d, %d)\n", 
                       s.id, s.x1, s.y1, s.x2, s.y2);
                break;
            case SHAPE_RECTANGLE:
                printf("  [%d] Rectangle: Top-Left (%d, %d), Width %d, Height %d\n", 
                       s.id, s.x1, s.y1, s.width, s.height);
                break;
            case SHAPE_CIRCLE:
                printf("  [%d] Circle: Center (%d, %d), Radius %d\n", 
                       s.id, s.x1, s.y1, s.radius);
                break;
            case SHAPE_TRIANGLE:
                printf("  [%d] Triangle: Vertices (%d, %d), (%d, %d), (%d, %d)\n", 
                       s.id, s.x1, s.y1, s.x2, s.y2, s.x3, s.y3);
                break;
            case SHAPE_DIAMOND:
                printf("  [%d] Diamond: Center (%d, %d), Radius %d\n", 
                       s.id, s.x1, s.y1, s.radius);
                break;
            case SHAPE_SQUARE:
                printf("  [%d] Square: Top-Left (%d, %d), Side Length %d\n", 
                       s.id, s.x1, s.y1, s.side);
                break;
            case SHAPE_RIGHT_TRIANGLE:
                printf("  [%d] Right Triangle: Corner (%d, %d), Width (Base) %d, Height %d\n", 
                       s.id, s.x1, s.y1, s.width, s.height);
                break;
            case SHAPE_FILLED_RECTANGLE:
                printf("  [%d] Filled Rectangle: Top-Left (%d, %d), Width %d, Height %d\n", 
                       s.id, s.x1, s.y1, s.width, s.height);
                break;
        }
    }
}

// User input validation helper
int read_int(const char *prompt, int min_val, int max_val) {
    int val;
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            continue;
        }
        buffer[strcspn(buffer, "\n")] = '\0';
        
        if (strlen(buffer) == 0) {
            continue;
        }
        
        char *endptr;
        long parsed = strtol(buffer, &endptr, 10);
        if (endptr == buffer || *endptr != '\0') {
            printf(COLOR_ERROR "Invalid integer format. Please try again.\n" COLOR_RESET);
            continue;
        }
        if (parsed < min_val || parsed > max_val) {
            printf(COLOR_ERROR "Value out of bounds. Must be between %d and %d.\n" COLOR_RESET, min_val, max_val);
            continue;
        }
        val = (int)parsed;
        break;
    }
    return val;
}

// User input forms for shapes
Shape prompt_line() {
    Shape s;
    s.type = SHAPE_LINE;
    printf("\n--- Draw Line ---\n");
    s.x1 = read_int("Enter Start X (0 to 59): ", 0, WIDTH - 1);
    s.y1 = read_int("Enter Start Y (0 to 19): ", 0, HEIGHT - 1);
    s.x2 = read_int("Enter End X (0 to 59): ", 0, WIDTH - 1);
    s.y2 = read_int("Enter End Y (0 to 19): ", 0, HEIGHT - 1);
    return s;
}

Shape prompt_rectangle() {
    Shape s;
    s.type = SHAPE_RECTANGLE;
    printf("\n--- Draw Rectangle ---\n");
    s.x1 = read_int("Enter Top-Left X (0 to 59): ", 0, WIDTH - 1);
    s.y1 = read_int("Enter Top-Left Y (0 to 19): ", 0, HEIGHT - 1);
    s.width = read_int("Enter Width (1 to 60): ", 1, WIDTH);
    s.height = read_int("Enter Height (1 to 20): ", 1, HEIGHT);
    return s;
}

Shape prompt_circle() {
    Shape s;
    s.type = SHAPE_CIRCLE;
    printf("\n--- Draw Circle ---\n");
    s.x1 = read_int("Enter Center X (0 to 59): ", 0, WIDTH - 1);
    s.y1 = read_int("Enter Center Y (0 to 19): ", 0, HEIGHT - 1);
    s.radius = read_int("Enter Radius (0 to 30): ", 0, 30);
    return s;
}

Shape prompt_triangle() {
    Shape s;
    s.type = SHAPE_TRIANGLE;
    printf("\n--- Draw Triangle ---\n");
    s.x1 = read_int("Enter Vertex 1 X (0 to 59): ", 0, WIDTH - 1);
    s.y1 = read_int("Enter Vertex 1 Y (0 to 19): ", 0, HEIGHT - 1);
    s.x2 = read_int("Enter Vertex 2 X (0 to 59): ", 0, WIDTH - 1);
    s.y2 = read_int("Enter Vertex 2 Y (0 to 19): ", 0, HEIGHT - 1);
    s.x3 = read_int("Enter Vertex 3 X (0 to 59): ", 0, WIDTH - 1);
    s.y3 = read_int("Enter Vertex 3 Y (0 to 19): ", 0, HEIGHT - 1);
    return s;
}

Shape prompt_diamond() {
    Shape s;
    s.type = SHAPE_DIAMOND;
    printf("\n--- Draw Diamond ---\n");
    s.x1 = read_int("Enter Center X (0 to 59): ", 0, WIDTH - 1);
    s.y1 = read_int("Enter Center Y (0 to 19): ", 0, HEIGHT - 1);
    s.radius = read_int("Enter Radius (half-diagonal length) (0 to 20): ", 0, 20);
    return s;
}

Shape prompt_square() {
    Shape s;
    s.type = SHAPE_SQUARE;
    printf("\n--- Draw Square ---\n");
    s.x1 = read_int("Enter Top-Left X (0 to 59): ", 0, WIDTH - 1);
    s.y1 = read_int("Enter Top-Left Y (0 to 19): ", 0, HEIGHT - 1);
    s.side = read_int("Enter Side Length (1 to 20): ", 1, 20);
    return s;
}

Shape prompt_right_triangle() {
    Shape s;
    s.type = SHAPE_RIGHT_TRIANGLE;
    printf("\n--- Draw Right Triangle ---\n");
    s.x1 = read_int("Enter Corner X (0 to 59): ", 0, WIDTH - 1);
    s.y1 = read_int("Enter Corner Y (0 to 19): ", 0, HEIGHT - 1);
    s.width = read_int("Enter Base Width (1 to 60): ", 1, WIDTH);
    s.height = read_int("Enter Height (1 to 20): ", 1, HEIGHT);
    return s;
}

Shape prompt_filled_rectangle() {
    Shape s;
    s.type = SHAPE_FILLED_RECTANGLE;
    printf("\n--- Draw Filled Rectangle ---\n");
    s.x1 = read_int("Enter Top-Left X (0 to 59): ", 0, WIDTH - 1);
    s.y1 = read_int("Enter Top-Left Y (0 to 19): ", 0, HEIGHT - 1);
    s.width = read_int("Enter Width (1 to 60): ", 1, WIDTH);
    s.height = read_int("Enter Height (1 to 20): ", 1, HEIGHT);
    return s;
}

void menu_add_shape() {
    printf("\n" COLOR_BORDER "=== Add Shape ===" COLOR_RESET "\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");
    printf("5. Diamond\n");
    printf("6. Square\n");
    printf("7. Right Triangle\n");
    printf("8. Filled Rectangle\n");
    printf("9. Back to Main Menu\n");
    int choice = read_int("Select Shape Type: ", 1, 9);
    
    Shape s;
    switch (choice) {
        case 1: s = prompt_line(); break;
        case 2: s = prompt_rectangle(); break;
        case 3: s = prompt_circle(); break;
        case 4: s = prompt_triangle(); break;
        case 5: s = prompt_diamond(); break;
        case 6: s = prompt_square(); break;
        case 7: s = prompt_right_triangle(); break;
        case 8: s = prompt_filled_rectangle(); break;
        case 9: return;
    }
    
    int id = add_shape(s);
    if (id == -1) {
        printf(COLOR_ERROR "Error: Canvas shape limit reached (%d max)!\n" COLOR_RESET, MAX_SHAPES);
    } else {
        printf(COLOR_SUCCESS "Shape added successfully with ID %d.\n" COLOR_RESET, id);
    }
}

void menu_modify_shape() {
    printf("\n" COLOR_BORDER "=== Modify Shape ===" COLOR_RESET "\n");
    list_shapes();
    if (shape_count == 0) return;
    
    int id = read_int("Enter Shape ID to modify: ", 1, next_id - 1);
    int index = -1;
    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].id == id) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        printf(COLOR_ERROR "Error: Shape with ID %d not found.\n" COLOR_RESET, id);
        return;
    }
    
    Shape updated;
    switch (shapes[index].type) {
        case SHAPE_LINE: updated = prompt_line(); break;
        case SHAPE_RECTANGLE: updated = prompt_rectangle(); break;
        case SHAPE_CIRCLE: updated = prompt_circle(); break;
        case SHAPE_TRIANGLE: updated = prompt_triangle(); break;
        case SHAPE_DIAMOND: updated = prompt_diamond(); break;
        case SHAPE_SQUARE: updated = prompt_square(); break;
        case SHAPE_RIGHT_TRIANGLE: updated = prompt_right_triangle(); break;
        case SHAPE_FILLED_RECTANGLE: updated = prompt_filled_rectangle(); break;
    }
    
    modify_shape(id, updated);
    printf(COLOR_SUCCESS "Shape with ID %d updated successfully.\n" COLOR_RESET, id);
}

void menu_move_shape() {
    printf("\n" COLOR_BORDER "=== Move Shape ===" COLOR_RESET "\n");
    list_shapes();
    if (shape_count == 0) return;
    
    int id = read_int("Enter Shape ID to move: ", 1, next_id - 1);
    int index = -1;
    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].id == id) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        printf(COLOR_ERROR "Error: Shape with ID %d not found.\n" COLOR_RESET, id);
        return;
    }
    
    Shape *s = &shapes[index];
    printf("\nMoving Shape [%d] (Current coordinates: X=%d, Y=%d)\n", s->id, s->x1, s->y1);
    int new_x = read_int("Enter new X coordinate: ", 0, WIDTH - 1);
    int new_y = read_int("Enter new Y coordinate: ", 0, HEIGHT - 1);
    
    int dx = new_x - s->x1;
    int dy = new_y - s->y1;
    
    // Shift position relative to the main point
    s->x1 = new_x;
    s->y1 = new_y;
    
    if (s->type == SHAPE_LINE) {
        s->x2 += dx;
        s->y2 += dy;
    } else if (s->type == SHAPE_TRIANGLE) {
        s->x2 += dx;
        s->y2 += dy;
        s->x3 += dx;
        s->y3 += dy;
    }
    
    printf(COLOR_SUCCESS "Shape with ID %d moved successfully.\n" COLOR_RESET, id);
}

void menu_resize_shape() {
    printf("\n" COLOR_BORDER "=== Resize Shape ===" COLOR_RESET "\n");
    list_shapes();
    if (shape_count == 0) return;
    
    int id = read_int("Enter Shape ID to resize: ", 1, next_id - 1);
    int index = -1;
    for (int i = 0; i < shape_count; i++) {
        if (shapes[i].id == id) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        printf(COLOR_ERROR "Error: Shape with ID %d not found.\n" COLOR_RESET, id);
        return;
    }
    
    Shape *s = &shapes[index];
    switch (s->type) {
        case SHAPE_LINE:
            printf("Resizing Line. Current end point is (%d, %d)\n", s->x2, s->y2);
            s->x2 = read_int("Enter new End X: ", 0, WIDTH - 1);
            s->y2 = read_int("Enter new End Y: ", 0, HEIGHT - 1);
            break;
        case SHAPE_RECTANGLE:
        case SHAPE_FILLED_RECTANGLE:
            printf("Resizing Rectangle. Current size: Width=%d, Height=%d\n", s->width, s->height);
            s->width = read_int("Enter new Width: ", 1, WIDTH);
            s->height = read_int("Enter new Height: ", 1, HEIGHT);
            break;
        case SHAPE_CIRCLE:
        case SHAPE_DIAMOND:
            printf("Resizing Circle/Diamond. Current radius: %d\n", s->radius);
            s->radius = read_int("Enter new Radius: ", 0, 30);
            break;
        case SHAPE_SQUARE:
            printf("Resizing Square. Current side length: %d\n", s->side);
            s->side = read_int("Enter new Side Length: ", 1, 20);
            break;
        case SHAPE_RIGHT_TRIANGLE:
            printf("Resizing Right Triangle. Current size: Width=%d, Height=%d\n", s->width, s->height);
            s->width = read_int("Enter new Base Width: ", 1, WIDTH);
            s->height = read_int("Enter new Height: ", 1, HEIGHT);
            break;
        case SHAPE_TRIANGLE:
            printf("Resizing Triangle. Vertices 2 & 3 are currently at (%d, %d) and (%d, %d)\n", s->x2, s->y2, s->x3, s->y3);
            s->x2 = read_int("Enter new Vertex 2 X: ", 0, WIDTH - 1);
            s->y2 = read_int("Enter new Vertex 2 Y: ", 0, HEIGHT - 1);
            s->x3 = read_int("Enter new Vertex 3 X: ", 0, WIDTH - 1);
            s->y3 = read_int("Enter new Vertex 3 Y: ", 0, HEIGHT - 1);
            break;
    }
    printf(COLOR_SUCCESS "Shape with ID %d resized successfully.\n" COLOR_RESET, id);
}

void menu_delete_shape() {
    printf("\n" COLOR_BORDER "=== Delete Shape ===" COLOR_RESET "\n");
    list_shapes();
    if (shape_count == 0) return;
    
    int id = read_int("Enter Shape ID to delete: ", 1, next_id - 1);
    if (delete_shape(id) == 0) {
        printf(COLOR_SUCCESS "Shape with ID %d deleted successfully.\n" COLOR_RESET, id);
    } else {
        printf(COLOR_ERROR "Error: Shape with ID %d not found.\n" COLOR_RESET, id);
    }
}

void menu_clear_canvas() {
    printf("\n" COLOR_BORDER "=== Clear Canvas ===" COLOR_RESET "\n");
    if (shape_count == 0) {
        printf("Canvas is already clear.\n");
        return;
    }
    printf("Are you sure you want to delete all shapes? (1 = Yes, 2 = No): ");
    int choice = read_int("", 1, 2);
    if (choice == 1) {
        shape_count = 0;
        printf(COLOR_SUCCESS "All shapes deleted. Canvas cleared successfully!\n" COLOR_RESET);
    } else {
        printf("Operation cancelled.\n");
    }
}

int main() {
    enable_ansi();
    clear_canvas();
    
    while (1) {
        printf("\n" COLOR_BORDER "==========================================" COLOR_RESET "\n");
        printf(COLOR_BORDER "      ADVANCED 2D GRAPHICS EDITOR (C)    " COLOR_RESET "\n");
        printf(COLOR_BORDER "==========================================" COLOR_RESET "\n");
        printf("1. Add a Shape\n");
        printf("2. Modify an Existing Shape\n");
        printf("3. Move a Shape\n");
        printf("4. Resize a Shape\n");
        printf("5. Delete a Shape\n");
        printf("6. Display Canvas\n");
        printf("7. Clear Canvas\n");
        printf("8. List Active Shapes\n");
        printf("9. Exit\n");
        printf(COLOR_BORDER "------------------------------------------" COLOR_RESET "\n");
        
        int choice = read_int("Select Option (1-9): ", 1, 9);
        switch (choice) {
            case 1:
                menu_add_shape();
                break;
            case 2:
                menu_modify_shape();
                break;
            case 3:
                menu_move_shape();
                break;
            case 4:
                menu_resize_shape();
                break;
            case 5:
                menu_delete_shape();
                break;
            case 6:
                printf("\nRendering current canvas:\n");
                display_canvas();
                break;
            case 7:
                menu_clear_canvas();
                break;
            case 8:
                list_shapes();
                break;
            case 9:
                printf("\nThank you for using Advanced 2D Graphics Editor!\n");
                return 0;
        }
    }
    return 0;
}
