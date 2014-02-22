#include "DisplayManager.hpp"
#include <opencv2/opencv.hpp>

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
  MainWindow = new sf::RenderWindow(sf::VideoMode(640, 480, 16), "Client #");
  sf::Event event;
  sf::Image * image = NULL;
  sf::Sprite sprite;
  short rgb[3];
  sf::Uint8 * rgbData = NULL;

  while (running)
    {
      if (update == true && matrix != NULL)
	{
	  if (image == NULL)
	    {
	      std::cout << "MATRIX X + " << matrix->cols << std::endl;
	      image = new sf::Image(matrix->cols, matrix->rows, sf::Color(0, 0, 0));
	      std::cout <<" Width =- " << image->GetWidth() << " HEIGHT = " << image->GetHeight() << std::endl;
	      rgbData = (sf::Uint8*)malloc(sizeof(sf::Uint8) * ((matrix->cols * matrix->rows) * 4));
	      for (int i = 0 ; i < (matrix->rows * matrix->cols) * 4; ++i)
		{
		  rgbData[i] = 255;
		}
	    }
	  else
	    {
	      //std::cout << matrix->type() << std::endl;
	      cv::imshow("DATA", *matrix);
	      cv::waitKey(1);
	      /*std::cout << matrix->cols << " image = " << image->GetWidth() << std::endl;
		for (int x = 0 ; x < image->GetWidth() ; ++x)
		{
		for (int y = 0 ; y < image->GetHeight() ; ++y)
		{
		color = matrix->at<cv::Vec4b>(y, x);
		image->SetPixel(x, y, sf::Color(color.val[2], color.val[1], color.val[0]));
		}
		}*/
	      
	      /*
	      for (int y = 0 ; y < matrix->rows ; ++y)
		 {
		   for ( int x = 0 ; x < matrix->cols ; ++x)
		     {
		       rgbData[(x * 4) * (matrix->rows) + (y * 4) + 2] = matrix->data[x * (matrix->rows * matrix->channels()) + (y *matrix->channels()) + 0];
		       rgbData[(x * 4) * (matrix->rows) + (y * 4) + 1] = matrix->data[x * (matrix->rows * matrix->channels()) + (y * matrix->channels()) + 1];
		       rgbData[(x * 4) * (matrix->rows) + (y * 4)] = matrix->data[x * (matrix->rows * matrix->channels()) + (y * matrix->channels()) + 2];
		     }
		 }
	      */
	      int j = matrix->cols * matrix->rows;
	      for (int i = 0 ; i < (matrix->cols * matrix->rows); ++i)
		{
		  rgbData[(i * 4)] = matrix->data[(j * matrix->channels())];
		  rgbData[(i * 4) + 1] = matrix->data[(j * matrix->channels()) + 1];
		  rgbData[(i * 4) + 2] = matrix->data[(j * matrix->channels()) + 2];
		  --j;
		}
	      image->LoadFromPixels(matrix->cols, matrix->rows, (const sf::Uint8*)rgbData);
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
  free(rgbData);
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
      matrix = new cv::Mat(_matrix->rows, _matrix->cols, _matrix->type());
    }
  if (update == false)
    {
      for (int i = 0 ; i < (_matrix->cols * _matrix->rows) * _matrix->channels() ; ++i)
	{
	  matrix->data[i] = _matrix->data[i];
	}
      if (toUpdate == true)
	update = true;
    }
}
