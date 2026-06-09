#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

// Canvas constraints
#define HEIGHT 20
#define WIDTH 60
#define MAX_SHAPES 100

// ANSI Style Escape Codes for modern CLI aesthetics
#define COLOR_RESET   "\x1b[0m"
#define COLOR_DIM     "\x1b[90m" // Dark gray for background canvas grid
#define COLOR_BRIGHT  "\x1b[93m" // Bright yellow for drawings '*'
#define COLOR_BORDER  "\x1b[36m" // Cyan for editor panels and borders
#define COLOR_ERROR   "\x1b[31m" // Red for errors
#define COLOR_SUCCESS "\x1b[32m" // Green for success messages
#define COLOR_INFO    "\x1b[34m" // Blue for information listing

// Shape definitions
typedef enum {
    SHAPE_LINE = 1,
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_TRIANGLE
} ShapeType;

typedef struct {
    int x1, y1;
    int x2, y2;
} LineData;

typedef struct {
    int x, y;
    int w, h;
} RectData;

typedef struct {
    int cx, cy;
    int r;
} CircleData;

typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} TriangleData;

typedef struct {
    int id;
    ShapeType type;
    union {
        LineData line;
        RectData rect;
        CircleData circle;
        TriangleData triangle;
    } data;
} Shape;

// Editor state
Shape shapes[MAX_SHAPES];
int shape_count = 0;
int next_id = 1;
char canvas[HEIGHT][WIDTH];

// Enable virtual terminal processing on Windows for ANSI colors
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

void set_pixel(int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        canvas[y][x] = '*';
    }
}

// Bresenham's Line Algorithm
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

// Bresenham's Midpoint Circle Algorithm
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

// Rectangle Drawing Function
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

// Triangle Drawing Function
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(x1, y1, x2, y2);
    draw_line(x2, y2, x3, y3);
    draw_line(x3, y3, x1, y1);
}

// Rebuild and clear canvas from shapes database
void render_canvas() {
    clear_canvas();
    for (int i = 0; i < shape_count; i++) {
        Shape s = shapes[i];
        switch (s.type) {
            case SHAPE_LINE:
                draw_line(s.data.line.x1, s.data.line.y1, s.data.line.x2, s.data.line.y2);
                break;
            case SHAPE_RECTANGLE:
                draw_rectangle(s.data.rect.x, s.data.rect.y, s.data.rect.w, s.data.rect.h);
                break;
            case SHAPE_CIRCLE:
                draw_circle(s.data.circle.cx, s.data.circle.cy, s.data.circle.r);
                break;
            case SHAPE_TRIANGLE:
                draw_triangle(s.data.triangle.x1, s.data.triangle.y1, 
                              s.data.triangle.x2, s.data.triangle.y2, 
                              s.data.triangle.x3, s.data.triangle.y3);
                break;
        }
    }
}

// Display canvas to stdout
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
        printf("  (None)\n");
        return;
    }
    for (int i = 0; i < shape_count; i++) {
        Shape s = shapes[i];
        switch (s.type) {
            case SHAPE_LINE:
                printf("  [%d] Line: Start (%d, %d) to End (%d, %d)\n", 
                       s.id, s.data.line.x1, s.data.line.y1, s.data.line.x2, s.data.line.y2);
                break;
            case SHAPE_RECTANGLE:
                printf("  [%d] Rectangle: Top-Left (%d, %d), Width %d, Height %d\n", 
                       s.id, s.data.rect.x, s.data.rect.y, s.data.rect.w, s.data.rect.h);
                break;
            case SHAPE_CIRCLE:
                printf("  [%d] Circle: Center (%d, %d), Radius %d\n", 
                       s.id, s.data.circle.cx, s.data.circle.cy, s.data.circle.r);
                break;
            case SHAPE_TRIANGLE:
                printf("  [%d] Triangle: Vertices (%d, %d), (%d, %d), (%d, %d)\n", 
                       s.id, s.data.triangle.x1, s.data.triangle.y1, 
                       s.data.triangle.x2, s.data.triangle.y2, 
                       s.data.triangle.x3, s.data.triangle.y3);
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
        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = '\0';
        
        // Skip empty lines
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
    s.data.line.x1 = read_int("Enter Start X (0 to 59): ", 0, WIDTH - 1);
    s.data.line.y1 = read_int("Enter Start Y (0 to 19): ", 0, HEIGHT - 1);
    s.data.line.x2 = read_int("Enter End X (0 to 59): ", 0, WIDTH - 1);
    s.data.line.y2 = read_int("Enter End Y (0 to 19): ", 0, HEIGHT - 1);
    return s;
}

Shape prompt_rectangle() {
    Shape s;
    s.type = SHAPE_RECTANGLE;
    printf("\n--- Draw Rectangle ---\n");
    s.data.rect.x = read_int("Enter Top-Left X (0 to 59): ", 0, WIDTH - 1);
    s.data.rect.y = read_int("Enter Top-Left Y (0 to 19): ", 0, HEIGHT - 1);
    s.data.rect.w = read_int("Enter Width (1 to 60): ", 1, WIDTH);
    s.data.rect.h = read_int("Enter Height (1 to 20): ", 1, HEIGHT);
    return s;
}

Shape prompt_circle() {
    Shape s;
    s.type = SHAPE_CIRCLE;
    printf("\n--- Draw Circle ---\n");
    s.data.circle.cx = read_int("Enter Center X (0 to 59): ", 0, WIDTH - 1);
    s.data.circle.cy = read_int("Enter Center Y (0 to 19): ", 0, HEIGHT - 1);
    s.data.circle.r = read_int("Enter Radius (0 to 40): ", 0, 40);
    return s;
}

Shape prompt_triangle() {
    Shape s;
    s.type = SHAPE_TRIANGLE;
    printf("\n--- Draw Triangle ---\n");
    s.data.triangle.x1 = read_int("Enter Vertex 1 X (0 to 59): ", 0, WIDTH - 1);
    s.data.triangle.y1 = read_int("Enter Vertex 1 Y (0 to 19): ", 0, HEIGHT - 1);
    s.data.triangle.x2 = read_int("Enter Vertex 2 X (0 to 59): ", 0, WIDTH - 1);
    s.data.triangle.y2 = read_int("Enter Vertex 2 Y (0 to 19): ", 0, HEIGHT - 1);
    s.data.triangle.x3 = read_int("Enter Vertex 3 X (0 to 59): ", 0, WIDTH - 1);
    s.data.triangle.y3 = read_int("Enter Vertex 3 Y (0 to 19): ", 0, HEIGHT - 1);
    return s;
}

void menu_add_shape() {
    printf("\n" COLOR_BORDER "=== Add Shape ===" COLOR_RESET "\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");
    printf("5. Back to Main Menu\n");
    int choice = read_int("Select Shape Type: ", 1, 5);
    
    Shape s;
    switch (choice) {
        case 1: s = prompt_line(); break;
        case 2: s = prompt_rectangle(); break;
        case 3: s = prompt_circle(); break;
        case 4: s = prompt_triangle(); break;
        case 5: return;
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
    }
    
    modify_shape(id, updated);
    printf(COLOR_SUCCESS "Shape with ID %d updated successfully.\n" COLOR_RESET, id);
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

int main() {
    enable_ansi();
    clear_canvas();
    
    while (1) {
        printf("\n" COLOR_BORDER "==================================" COLOR_RESET "\n");
        printf(COLOR_BORDER "      ASCII 2D GRAPHICS EDITOR    " COLOR_RESET "\n");
        printf(COLOR_BORDER "==================================" COLOR_RESET "\n");
        printf("1. Add a Shape\n");
        printf("2. Modify an Existing Shape\n");
        printf("3. Delete a Shape\n");
        printf("4. Display Canvas\n");
        printf("5. List Active Shapes\n");
        printf("6. Exit\n");
        printf(COLOR_BORDER "----------------------------------" COLOR_RESET "\n");
        
        int choice = read_int("Select Option (1-6): ", 1, 6);
        switch (choice) {
            case 1:
                menu_add_shape();
                break;
            case 2:
                menu_modify_shape();
                break;
            case 3:
                menu_delete_shape();
                break;
            case 4:
                printf("\nRendering current canvas:\n");
                display_canvas();
                break;
            case 5:
                list_shapes();
                break;
            case 6:
                printf("\nThank you for using ASCII 2D Graphics Editor!\n");
                return 0;
        }
    }
    return 0;
}
