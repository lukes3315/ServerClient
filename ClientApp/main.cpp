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

int main(int argc, char *argv[])
{
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  cv::Mat img;
  cv::VideoCapture cap("./video.MOV");
  char buffer[256];


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
  cv::namedWindow("Win", 1);
  //std::vector<unsigned char> bytes;
  unsigned char * bytes;
  int sizex = 0, sizey = 0;
  cv::Mat resized(480, 640, CV_32FC1);
  while (k != 1114089)
    {
      cap >> img;
      std::cout << "trying to capture image" << std::endl;
      if (img.empty())
	break;
      cv::resize(img, resized, resized.size(), 0, 0, CV_INTER_CUBIC);
      if (sizex == 0)
	{
	  sizex = resized.cols;
	  sizey = resized.rows;
	  std::cout << "after init" <<  "size x= " << sizex << "sizey = "<< sizey << "step = "<< img.step << std::endl;
	  bytes = (unsigned char*)malloc(sizeof(unsigned char) * (sizex * sizey));
	  bzero(bytes, strlen((const char*)bytes));
	}
      std::cout << "Before reading image" << std::endl;
      write(sockfd, "start", 4);
      for (int i = 0 ; i < sizex * sizey; ++i)
	{
	  write(sockfd, img.data[i], 1);
	  //bytes[i] = img.data[i];
	  //bytes.push_front((unsigned char)(img.data + i * img.step + j));
	}
      
      write(sockfd, "end", 3);
      //cv::imshow("Win", resized);
      k = cv::waitKey(1);
      std::cout << k << std::endl;
    }

  close(sockfd);

  printf("%s\n",buffer);
  return 0;
}
