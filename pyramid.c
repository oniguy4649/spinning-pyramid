#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define WIDTH 150
#define HEIGHT 50
#define DISTANCE_FROM_CAM 100
#define HORIZONTAL_OFFSET 0
#define BACKGROUND_SYMBOL ' '
#define PROJECTION_FACTOR 20
#define ROTATION_SPEED_X 0.05
#define ROTATION_SPEED_Y 0.05
#define ROTATION_SPEED_Z 0.01
#define INCREMENT_SPEED 0.6

#define BASE1_X -20
#define BASE1_Y -20
#define BASE1_Z -50
#define BASE2_X 20
#define BASE2_Y -20
#define BASE2_Z -50
#define BASE3_X 20
#define BASE3_Y 20
#define BASE3_Z -50
#define BASE4_X -20
#define BASE4_Y 20
#define BASE4_Z -50
#define APEX_X 0
#define APEX_Y 0
#define APEX_Z -20

// Global pariables
float rotation_x = 0.0f;
float rotation_y = 0.0f;
float rotation_z = 0.0f;

float z_buffer[WIDTH * HEIGHT];
char screen_buffer[WIDTH * HEIGHT];

// Function prototypes
float calculate_x(float i, float j, float k);
float calculate_y(float i, float j, float k);
float calculate_z(float i, float j, float k);
void calculate_for_surface(float vertex_x, float vertex_y, float vertex_z, char ch);
void draw_triangle(float v1[3], float v2[3], float v3[3], char ch);

int main() {
    printf("\x1b[2J"); // Clear screen

    while (1) {
        // Clear buffers
        memset(screen_buffer, BACKGROUND_SYMBOL, WIDTH * HEIGHT);
        memset(z_buffer, 0, sizeof(z_buffer));

        // Define pyramid vertices
        float base1[3] = {BASE1_X, BASE1_Y, BASE1_Z};
        float base2[3] = {BASE2_X, BASE2_Y, BASE2_Z};
        float base3[3] = {BASE3_X, BASE3_Y, BASE3_Z};
        float base4[3] = {BASE4_X, BASE4_Y, BASE4_Z};
        float apex[3] = {APEX_X, APEX_Y, APEX_Z};

        // Draw pyramid faces
        draw_triangle(base1, base2, apex, '@'); // Front face
        draw_triangle(base2, base3, apex, '#'); // Right face
        draw_triangle(base3, base4, apex, '+'); // Back face
        draw_triangle(base4, base1, apex, '~'); // Left face

        // Draw pyramid base
        draw_triangle(base1, base2, base3, '.');
        draw_triangle(base1, base3, base4, '.');

        // Display screen buffer
        printf("\x1b[H"); // Reset cursor
        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            putchar(i % WIDTH ? screen_buffer[i] : '\n');
        }

        // Update rotations
        rotation_x += ROTATION_SPEED_X;
        rotation_y += ROTATION_SPEED_Y;
        rotation_z += ROTATION_SPEED_Z;

        usleep(16000); // 60 FPS
    }

    return 0;
}

float calculate_x(float i, float j, float k) {
    return j * sin(rotation_x) * sin(rotation_y) * cos(rotation_z) -
           k * cos(rotation_x) * sin(rotation_y) * cos(rotation_z) +
           j * cos(rotation_x) * sin(rotation_z) +
           k * sin(rotation_x) * sin(rotation_z) +
           i * cos(rotation_y) * cos(rotation_z);
}

float calculate_y(float i, float j, float k) {
    return j * cos(rotation_x) * cos(rotation_z) +
           k * sin(rotation_x) * cos(rotation_z) -
           j * sin(rotation_x) * sin(rotation_y) * sin(rotation_z) +
           k * cos(rotation_x) * sin(rotation_y) * sin(rotation_z) -
           i * cos(rotation_y) * sin(rotation_z);
}

float calculate_z(float i, float j, float k) {
    return k * cos(rotation_x) * cos(rotation_y) -
           j * sin(rotation_x) * cos(rotation_y) +
           i * sin(rotation_y);
}

void calculate_for_surface(float vertex_x, float vertex_y, float vertex_z, char ch) {
    float x = calculate_x(vertex_x, vertex_y, vertex_z);
    float y = calculate_y(vertex_x, vertex_y, vertex_z);
    float z = calculate_z(vertex_x, vertex_y, vertex_z) + DISTANCE_FROM_CAM;

    float ooz = 1 / z;
    int xp = (int)(WIDTH / 2 + HORIZONTAL_OFFSET + PROJECTION_FACTOR * ooz * x * 2);
    int yp = (int)(HEIGHT / 2 + PROJECTION_FACTOR * ooz * y);

    int idx = xp + yp * WIDTH;
    if (idx >= 0 && idx < WIDTH * HEIGHT) {
        if (ooz > z_buffer[idx]) {
            z_buffer[idx] = ooz;
            screen_buffer[idx] = ch;
        }
    }
}

void draw_triangle(float v1[3], float v2[3], float v3[3], char ch) {
    for (float t = 0; t <= 1; t += INCREMENT_SPEED / 20) {
        float x1 = v1[0] + t * (v2[0] - v1[0]);
        float y1 = v1[1] + t * (v2[1] - v1[1]);
        float z1 = v1[2] + t * (v2[2] - v1[2]);

        float x2 = v1[0] + t * (v3[0] - v1[0]);
        float y2 = v1[1] + t * (v3[1] - v1[1]);
        float z2 = v1[2] + t * (v3[2] - v1[2]);

        for (float t2 = 0; t2 <= 1; t2 += INCREMENT_SPEED / 20) {
            float x = x1 + t2 * (x2 - x1);
            float y = y1 + t2 * (y2 - y1);
            float z = z1 + t2 * (z2 - z1);
            calculate_for_surface(x, y, z, ch);
        }
    }
}
