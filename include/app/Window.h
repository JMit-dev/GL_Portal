#ifndef WINDOW_H
#define WINDOW_H

struct GLFWwindow;
struct GLFWmonitor;

class Window {
protected:
  Window(int width, int height, const char *title);
  virtual ~Window() noexcept;

  Window(const Window &) = delete;
  Window(Window &&) = delete;
  Window &operator=(const Window &) = delete;
  Window &operator=(Window &&) = delete;

protected:
  GLFWwindow *pWindow{nullptr};
};
#endif
