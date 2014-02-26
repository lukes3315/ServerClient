#ifndef DISPLAY_MANAGER
#define DISPLAY_MANAGER

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <thread>
#include <iostream>
#include <opencv2/opencv.hpp>
class DisplayManager
{
  friend class Client;
public :
  DisplayManager();
  ~DisplayManager();
  void displayGraphics();
  void setRunning(bool);
  void setUpdate(bool);
  void setWindowSize(int, int);
  void updateMatrix(cv::Mat *, bool = false);
private :
  std::thread * graphicLoop;
  bool running;
  bool update;
  cv::Mat * matrix;
  sf::RenderWindow * MainWindow;
};

#endif
