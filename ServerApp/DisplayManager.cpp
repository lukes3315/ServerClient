#include "DisplayManager.hpp"

DisplayManager::DisplayManager()
{
  running = true;
  update = false;
  matrix = NULL;
  try 
    {
      graphicLoop = new std::thread(&DisplayManager::displayGraphics, this);
    }
  catch (std::exception & e)
    {
      std::cout << "[DisplayManager constructor] Error = " << e.what() << std::endl;
    }
}

DisplayManager::~DisplayManager()
{
  delete MainWindow;
  if (matrix != NULL)
    delete matrix;
}

void DisplayManager::displayGraphics()
{
  MainWindow = new sf::RenderWindow(sf::VideoMode(640, 480, 32), "Client #");
  sf::Event event;
  sf::Image * image = NULL;
  sf::Sprite sprite;
  unsigned char rgb[3];
  cv::Vec4b color;
  while (running)
    {
      if (update == true && matrix != NULL)
	{
	  if (image == NULL)
	    {
	      image = new sf::Image(matrix->cols, matrix->rows, sf::Color(0, 0, 0));
	    }
	  std::cout << matrix->cols << " image = " << image->GetWidth() << std::endl;
	  for (int x = 0 ; x < image->GetWidth() ; ++x)
	    {
	      for (int y = 0 ; y < image->GetHeight() ; ++y)
		{
		  color = matrix->at<cv::Vec4b>(y, x);
		  image->SetPixel(x, y, sf::Color(color.val[0], color.val[1], color.val[2]));
		}
	    }
	  
	  /* for (int x = 0 ; x < matrix->cols ; ++x)
	    {
	      for ( int y = 0 ; y < matrix->rows ; ++y)
		{
		  color = matrix->at<cv::Vec3b>(cv::Point(x, y));
		  
		  //rgb[0] = matrix->data[y * matrix->cols * matrix->channels() + x * matrix->channels() + 0];
		  //rgb[1] = matrix->data[y * matrix->cols * matrix->channels() + x * matrix->channels() + 1];
		  //rgb[2] = matrix->data[y * matrix->cols * matrix->channels() + x * matrix->channels() + 2];
		  image->SetPixel(x, y, sf::Color(color.val[0], color.val[1], color.val[2]));
		}
		}*/
	  std::cout << "Channels = " <<  matrix->channels() << std::endl;
	  if (image != NULL)
	    {
	      sprite.SetImage(*image);
	      MainWindow->Draw(sprite);
	    }
	  update = false;
	}
      while (MainWindow->GetEvent(event))
	{
	  if (event.Type == sf::Event::Closed)
	    running = false;
	  if ((event.Type == sf::Event::KeyPressed) && event.Key.Code == sf::Key::Escape)
	    running = false;
	  std::cout << "Get Events !" << std::endl;
	}
	MainWindow->Display();
    }
  MainWindow->Close();
}


void DisplayManager::setRunning(bool _running)
{
  running = _running;
}

void DisplayManager::setUpdate(bool _update)
{
  update = _update;
}

void DisplayManager::updateMatrix(cv::Mat * _matrix, bool toUpdate)
{
  if (matrix == NULL)
    {
      matrix = new cv::Mat(_matrix->cols, _matrix->rows, _matrix->type());
    }
  if (update == false)
    {
      for (int i = 0 ; i < _matrix->total() * _matrix->elemSize() ; ++i)
	{
	  matrix->data[i] = _matrix->data[i];
	}
      if (toUpdate == true)
	update = true;
    }
}
