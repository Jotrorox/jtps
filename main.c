#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

// Default values
#define DEFAULT_WIDTH 40
#define DEFAULT_HEIGHT 20
#define BALL_SPEED 30.0
#define FPS_UPDATE_INTERVAL 0.5

typedef struct {
  double x;
  double y;
  double dx;
  double dy;
} Ball;

typedef struct {
  int width;
  int height;
  int max_fps;
} Config;

// Global flag for cleanup
static volatile int running = 1;
double highestFPS = 1.0;

// Function to hide cursor
void hide_cursor() {
  printf("\033[?25l");  // ANSI escape code to hide cursor
  fflush(stdout);
}

// Function to show cursor
void show_cursor() {
  printf("\033[?25h");  // ANSI escape code to show cursor
  fflush(stdout);
}

// Signal handler for clean exit
void handle_signal(int sig) {
  running = 0;
}

// Cleanup function
void cleanup() {
  show_cursor();
  printf("\033[H\033[J");  // Clear screen
  printf("\033[H");        // Move to home position

  printf("The highest FPS reached were: %f", highestFPS);
}

double get_time() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + ts.tv_nsec / 1e9;
}

void clear_screen() {
  printf("\033[H\033[J");
  fflush(stdout);
}

void update_position(Ball* ball, double delta_time, const Config* config) {
  ball->x += ball->dx * BALL_SPEED * delta_time;
  ball->y += ball->dy * BALL_SPEED * delta_time;

  if (ball->x <= 0 || ball->x >= config->width - 1) {
    ball->dx = -ball->dx;
    ball->x = ball->x <= 0 ? 0 : config->width - 1;
  }

  if (ball->y <= 0 || ball->y >= config->height - 1) {
    ball->dy = -ball->dy;
    ball->y = ball->y <= 0 ? 0 : config->height - 1;
  }
}

void draw_frame(Ball* ball, double fps, const Config* config) {
  char** screen = malloc(config->height * sizeof(char*));
  for (int i = 0; i < config->height; i++) {
    screen[i] = malloc(config->width * sizeof(char));
    memset(screen[i], ' ', config->width);
  }

  char fps_text[32];
  if (config->max_fps > 0) {
    snprintf(fps_text, sizeof(fps_text), "%.1f/%d FPS", fps, config->max_fps);
  } else {
    snprintf(fps_text, sizeof(fps_text), "%.1f FPS", fps);
  }
  int fps_len = strlen(fps_text);
  for (int i = 0; i < fps_len && i < config->width; i++) {
    screen[0][config->width - fps_len + i] = fps_text[i];
  }

  if (ball->x >= 0 && ball->x < config->width && 
    ball->y >= 0 && ball->y < config->height) {
    screen[(int)ball->y][(int)ball->x] = 'O';
  }

  for (int i = 0; i < config->width; i++) {
    screen[0][i] = '-';
    screen[config->height - 1][i] = '-';
  }
  for (int i = 0; i < config->height; i++) {
    screen[i][0] = '|';
    screen[i][config->width - 1] = '|';
  }

  printf("\033[H");  // Move cursor to home position
  for (int i = 0; i < config->height; i++) {
    fwrite(screen[i], 1, config->width, stdout);
    putchar('\n');
  }
  fflush(stdout);

  for (int i = 0; i < config->height; i++) {
    free(screen[i]);
  }
  free(screen);
}

void print_usage(const char* program_name) {
  printf("Usage: %s [options]\n", program_name);
  printf("Options:\n");
  printf("  -w WIDTH    Set box width (default: %d)\n", DEFAULT_WIDTH);
  printf("  -h HEIGHT   Set box height (default: %d)\n", DEFAULT_HEIGHT);
  printf("  -f FPS      Set maximum FPS (default: unlimited)\n");
  printf("  --help      Show this help message\n\n");
  printf("Examples:\n");
  printf("  %s                  # Run with default size\n", program_name);
  printf("  %s -w 80 -h 40      # Run with 80x40 box\n", program_name);
  printf("  %s -f 60            # Run with 60 FPS limit\n", program_name);
  printf("  %s -w 100 -h 30 -f 30 # Run with custom size and FPS\n", program_name);
}

int parse_positive_int(const char* str, const char* param_name) {
  char* endptr;
  int value = (int)strtol(str, &endptr, 10);
  if (*endptr != '\0' || value <= 0) {
    printf("Error: Invalid %s value '%s'. Must be a positive integer.\n", param_name, str);
    exit(1);
  }
  return value;
}

Config parse_arguments(int argc, char* argv[]) {
  Config config = {
    .width = DEFAULT_WIDTH,
    .height = DEFAULT_HEIGHT,
    .max_fps = 0
  };

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--help") == 0) {
      print_usage(argv[0]);
      exit(0);
    } else if (strcmp(argv[i], "-w") == 0 && i + 1 < argc) {
      config.width = parse_positive_int(argv[++i], "width");
    } else if (strcmp(argv[i], "-h") == 0 && i + 1 < argc) {
      config.height = parse_positive_int(argv[++i], "height");
    } else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
      config.max_fps = parse_positive_int(argv[++i], "FPS");
    } else {
      printf("Error: Unknown option or missing argument: %s\n", argv[i]);
      print_usage(argv[0]);
      exit(1);
    }
  }

  return config;
}

int main(int argc, char* argv[]) {
  // Set up signal handlers for clean exit
  signal(SIGINT, handle_signal);
  signal(SIGTERM, handle_signal);

  // Register cleanup function to run at exit
  atexit(cleanup);

  Config config = parse_arguments(argc, argv);

  Ball ball = {
    .x = config.width * 0.125,
    .y = config.height * 0.25,
    .dx = 1.0,
    .dy = 0.5
  };

  double last_time = get_time();
  double last_fps_update = last_time;
  int frame_count = 0;
  double current_fps = 0.0;
  double target_frame_time = config.max_fps > 0 ? 1.0 / config.max_fps : 0.0;

  // Hide cursor before starting
  hide_cursor();

  // Main game loop
  while (running) {
    double frame_start_time = get_time();

    double current_time = frame_start_time;
    double delta_time = current_time - last_time;
    last_time = current_time;

    frame_count++;
    if (current_time - last_fps_update >= FPS_UPDATE_INTERVAL) {
      current_fps = frame_count / (current_time - last_fps_update);
      frame_count = 0;
      last_fps_update = current_time;
    }

    update_position(&ball, delta_time, &config);
    draw_frame(&ball, current_fps, &config);

    if (current_fps > highestFPS) {
      highestFPS = current_fps;
    }

    if (config.max_fps > 0) {
      double frame_end_time = get_time();
      double frame_time = frame_end_time - frame_start_time;
      if (frame_time < target_frame_time) {
        usleep((target_frame_time - frame_time) * 1000000);
      }
    } 
  }

  return 0;
}
