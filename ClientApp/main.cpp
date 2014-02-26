#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "base64.hpp"


#define PACKET_SIZE 18000

int main(int argc, char *argv[])
{
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  cv::Mat img;
  cv::VideoCapture cap("../../video.mp4");
  //cv::VideoCapture cap(0);

  if (!cap.isOpened())
    {
      std::cout << "Error opening file" << std::endl;
      return -1;
    }
  
  if (argc < 3) {
    fprintf(stderr,"usage %s hostname port\n", argv[0]);
    exit(0);
  }
  portno = atoi(argv[2]);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
    {
      perror("ERROR opening socket");
      exit(1);
    }
  server = gethostbyname(argv[1]);
  if (server == NULL) {
    fprintf(stderr,"ERROR, no such host\n");
    exit(0);
  }
  
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  bcopy((char *)server->h_addr, 
	(char *)&serv_addr.sin_addr.s_addr,
	server->h_length);
  serv_addr.sin_port = htons(portno);
  
  if (connect(sockfd,(const sockaddr*)&serv_addr,sizeof(serv_addr)) < 0) 
    {
      perror("ERROR connecting");
      exit(1);
    }
  
  int k = -1;
  cv::Mat resized(480, 640, img.type());  
  unsigned char * bytes;
  int sizex = 0, sizey = 0;
  cap >> img;
  
  cv::resize(img, resized, cv::Size(640, 480));
  std::stringstream ss;
  ss << resized.cols;
  ss << "x";
  ss << resized.rows;
  std::string size_string = "";
  size_string = ss.str();
  std::cout << "[Client sending message = " << size_string.c_str() << std::endl;
  for (int i = 0; i < size_string.size() ; ++i)
    {
      std::cout << "Sending = " << size_string[i] << std::endl;
      sendto(sockfd, &size_string[i], 1, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    }
  write(sockfd, "-", 1);
  ss.str("");
  ss.clear();

  while (1)
    {
      cap >> img;
      if (img.empty())
	break;
      cv::resize(img, resized, cv::Size(640, 480));
      std::basic_string<char> encoded = base64_encode(resized.data, resized.total() * resized.elemSize());
      std::string str = "";
      
      ss.str("");
      ss.clear();
      ss << encoded.size();
      str = ss.str() + "\n";
      //std::cout << "Sending " << count << std::endl;
      sendto(sockfd, str.c_str(), str.size(), 0,(struct sockaddr*)&serv_addr, sizeof(serv_addr));
      std::cout << "Sending header = "<< str << std::endl;
      sendto(sockfd, encoded.c_str(), encoded.size(), 0,(struct sockaddr*)&serv_addr, sizeof(serv_addr));
      //std::cout << "Sending bytes = "<< encoded.size() << std::endl;
      cv::imshow("img sent", img);
      cv::waitKey(1);
    }
  close(sockfd);
  //printf("%s\n",buffer);
  return 0;
}
