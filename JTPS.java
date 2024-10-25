public class JTPS {
  private static final int WIDTH = 40;
  private static final int HEIGHT = 20;
  private static final double BALL_SPEED = 30.0;

  private static double x = 5;
  private static double y = 5;
  private static double dx = 1;
  private static double dy = 0.5;

  private static final int FPS_UPDATE_INTERVAL = 500;
  private static long lastFpsUpdate = 0;
  private static int frameCount = 0;
  private static double currentFps = 0;

  public static void main(String[] args) {
    long lastFrameTime = System.nanoTime();

    while (true) {
      long currentTime = System.nanoTime();
      double deltaTime = (currentTime - lastFrameTime) / 1_000_000_000.0;
      lastFrameTime = currentTime;

      updateFps(currentTime);

      drawFrame();

      clearScreen();
    }
  }

  private static void updateFps(long currentTime) {
    frameCount++;

    if (currentTime - lastFpsUpdate >= FPS_UPDATE_INTERVAL * 1_000_000) {
      currentFps = frameCount / ((currentTime - lastFpsUpdate) / 1_000_000_000.0);
      frameCount = 0;
      lastFpsUpdate = currentTime;
    }
  }

  private static void drawFrame() {
    char[][] screen = new char[HEIGHT][WIDTH];

    for (int i = 0; i < HEIGHT; i++) for (int j = 0; j < WIDTH; j++) screen[i][j] = ' ';

    String fpsText = String.format("%.1f FPS", currentFps);
    drawText(screen, fpsText, 0, WIDTH - fpsText.length());

    screen[(int) y][(int) x] = 'O';

    for (int i = 0; i < WIDTH; i++) {
      screen[0][i] = '-';
      screen[HEIGHT-1][i] = '-';
    }

    for (int i = 0; i < HEIGHT; i++) {
      screen[i][0] = '|';
      screen[i][WIDTH-1] = '|';
    }

    for (int i = 0; i < HEIGHT; i++) System.out.println(new String(screen[i]));
  }

  private static void drawText(char[][] screen, String text, int row, int col) {
    for (int i = 0; i < text.length(); i++)
      if (col + i < WIDTH) screen[row][col + i] = text.charAt(i);
  }

  private static void clearScreen() {
    System.out.print("\033[H\033[2J]]");
    System.out.flush();
  }
}
