#include <opencv2/opencv.hpp>
#include "Server.hpp"
#include "Client.hpp"
#include "base64.hpp"
Client::Client(int _id, int _socket, Server *serv)
{
  this->ID = _id;
  running = true;
  this->socketfd = _socket;
  server = serv;
  try
    {
      std::cout << "Hello how are you ? " << std::endl;
      this->backgroundReadingThread = new std::thread(&Client::readData, 
						      this,
						      _socket);
    }
  catch (std::exception & e)
    {
      std::cout << "Error = " << e.what()<<std::endl;
    }
}

Client::~Client()
{
  running = false;
  if (socketfd > 0)
    {
      close(socketfd);
    }
}

void Client::readData(int _socket)
{  
  std::cout << "Starting thread for client # " << ID << std::endl;
  unsigned char * buffer = (unsigned char*)malloc(sizeof(unsigned char) * 1);
  cv::namedWindow("win");
  std::string data = "";
  cv::Mat *img = NULL;
  if (buffer == NULL)
    {
      std::cout << "Malloc error" << std::endl;
    }
  try
    {
      int bytesRead = 1;
      int readsize = 1;
      bool gotSize = false;
      std::string imgData = "";
      while (running && bytesRead > 0)
	{
	  
	  bytesRead = read(_socket, buffer, 1);
	  if (bytesRead > 0)
	    {
	      if (buffer[0] == '\n' && gotSize == false)
		{
		  std::cout << "Done ! Got Size ! " << data << " what is up ?? "<< std::endl;
		  std::string cols = data.substr(0, data.find_first_of("x"));
		  std::string rows = data.substr(data.find_first_of("x") + 1, data.find_first_of("-"));
		  std::string matSize = data.substr(data.find_first_of("-") + 1, data.size());
		  long longRows = atol(rows.c_str());
		  long longCols = atol(cols.c_str());
		  long matrixSize = atol(matSize.c_str());
		  img = new cv::Mat(longRows, longCols, CV_32FC1);
		  std::cout << "Extracted size Cols  = " << longCols << " rows = " << longRows << std::endl;
		  data = "";
		  gotSize = true;
		}
	      else if (buffer[0] == '\n' && gotSize == true)
		{
		  std::cout << buffer[0] << std::endl;
		  std::cout << "Header got = " << data << std::endl;
		  char buffImg[4096];
		  bzero(buffImg, 4096);
		  int r = read(_socket, buffImg, 4096);
		  imgData += buffImg;
		  if (data != "4096")
		    {
		      std::cout << "Image received " << imgData.size() << std::endl;
		      std::string decoded = base64_decode(imgData);
		      std::cout << "Decoded data size = " << decoded.size() << std::endl;
		      img->data = (unsigned char*)malloc(sizeof(unsigned char)*decoded.size() + 1);
		      bzero(img->data, sizeof(unsigned char)*decoded.size() + 1);
		      for (int i = 0 ; i < decoded.size() ; ++i)
			{
			  img->data[i]  = decoded[i];
			}
		      cv::imshow("win", *img);
		      cv::waitKey(0);
		    }
		  data = "";
		}
	      else
		{
		  std::cout << "data = " << buffer[0] << std::endl;
		  data  += buffer[0];
		}
	    }
	}
    }
  catch (std::exception e)
    {
      
    }
  bzero(buffer, 1);
  free(buffer);
  running = false;
  server->stopClient();
}

bool Client::isRunning()const
{
  return running;
}

int Client::getId()const
{
  return this->ID;
}
