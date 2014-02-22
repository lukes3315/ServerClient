#include <opencv2/opencv.hpp>
#include "Server.hpp"
#include "Client.hpp"
#include "base64.hpp"

Client::Client(int _id, int _socket, Server *serv)
{
  this->ID = _id;
  running = true;
  this->socketfd = _socket;
  dispManager = new DisplayManager();
  server = serv;
  try
    {
      this->backgroundReadingThread = new std::thread(&Client::readData, 
						      this,
						      _socket);
    }
  catch (std::exception & e)
    {
      std::cout << "[Client constructor] Client #" << _id << " Error : " << e.what()<<std::endl;
    }
}

Client::~Client()
{
  running = false;
  dispManager->setRunning(false);
  if (socketfd > 0)
    {
      close(socketfd);
    }
  delete dispManager;
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
      std::string imgData;
      char buffImg[4096];
      bzero(buffImg, 4096);
      while (running && bytesRead > 0)
	{
	  bytesRead = read(_socket, buffer, 1);
	  if (bytesRead > 0)
	    {
	      if (buffer[0] == '\n' && gotSize == false)
		{
		  std::string cols = data.substr(0, data.find_first_of("x"));
		  std::string rows = data.substr(data.find_first_of("x") + 1, data.find_first_of("-"));
		  std::string matSize = data.substr(data.find_first_of("-") + 1, data.find_last_of("-"));
		  std::string imgType = data.substr(data.find_last_of("-") + 1, data.size());
		  long longRows = atol(rows.c_str());
		  long longCols = atol(cols.c_str());
		  long matrixSize = atol(matSize.c_str());
		  int imageType = atoi(imgType.c_str());
		  img = new cv::Mat(longRows, longCols, imageType);
		  std::cout << imgType << std::endl;
		  data = "";
		  img->data = (unsigned char*)malloc(sizeof(unsigned char) * matrixSize);
		  gotSize = true;
		}
	      else if (buffer[0] == '\n' && gotSize == true)
		{
		  bzero(buffImg, 4096);
		  int r = read(_socket, buffImg, 4096);
		  imgData += buffImg;
		  if (data != "4096")
		    {
		      //std::cout << "Image received " << imgData.size() << std::endl;
		      std::string decoded = base64_decode(imgData);
		      //std::cout << "Decoded data size = " << decoded.size() << std::endl;
		      bzero(img->data, sizeof(unsigned char)*decoded.size() + 1);
		      int j = decoded.size() - 1;
		      for (int i = 0 ; i < decoded.size() ; ++i)
			{
			  img->data[i]  = decoded[j];
			  --j;
			}
		      dispManager->updateMatrix(img, true);
		      imgData = "";
		      imgData.shrink_to_fit();
		      //std::cout << "Refresh " << std::endl;
		      //free(img->data);
		    }
		  data = "";
		}
	      else
		{
		  data  += buffer[0];
		}
	    }
	}
    }
  catch (std::exception e)
    {
      std::cout << "Exception in Client #" << this->getId() << " Message = " << e.what() << std::endl;
    } 
  std::cout << "Freeing image->data" << std::endl;
  free(img->data);
  std::cout << "Freeing buffer" << std::endl;
  free(buffer);
  delete img;
  running = false;
  dispManager->setRunning(false);
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
