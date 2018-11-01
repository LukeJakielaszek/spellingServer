#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>

// custom dictionaries
#include "dictionary.h"
#include "utility.h"
#include "server.h"

#define DEFAULT_DICTIONARY "words.txt" // defaults dictionary file
#define DEFAULT_PORT "1111" // default port

#define BUFFSIZE 50
#define SERVICE_CAPACITY 2
#define THREAD_CAPACITY 1

pthread_mutex_t fileMutex;
char fileName[BUFFSIZE] = "log.txt";

typedef struct dictThread{
  char ** dict;
  int dictLen;
  sbuf_monitor_t *monitor;
}dictThread;

void *dictionarySearch(void * dictStruct);

int main(int argc, char ** argv){
  // default locations
  char *dictFile = DEFAULT_DICTIONARY;
  char *port = DEFAULT_PORT;

  // determines if other dict or ports are desired
  if(argc == 2){
    dictFile = argv[1];
  }else if(argc == 3){
    dictFile = argv[1];
    port = argv[2];
  }else if(argc == 1){
    ; // do nothing, default is desired
  }else{
    printf("ERROR: Invalid arguments.\n");
    exit(EXIT_FAILURE);
  }

  // # of words in dictionary
  int dictLen = -1;

  // gets dictionary
  char ** dict = readDict(dictFile, &dictLen);

  // server setup
  int listenfd, // listener socket
    connectedfd; // connected socket

  struct sockaddr_storage client_address;
  socklen_t client_size;
  char line[BUFFSIZE];
  ssize_t bytes_read;
  char clientName[BUFFSIZE];
  char clientPort[BUFFSIZE];

  // creates a passive socket to be used for accepting connections
  listenfd = getlistenfd(port);

  sbuf_monitor_t *serverToLook = (sbuf_monitor_t*)malloc(sizeof(sbuf_monitor_t));

  if(serverToLook == NULL){
    printf("ERROR: Failed to Malloc monitor");
    exit(EXIT_FAILURE);
  }
  
  sbuf_init(serverToLook, SERVICE_CAPACITY);

  // struct to hold information neccesary for dictionary threads
  dictThread *dictInfo = (dictThread*)malloc(sizeof(dictThread));

  if(dictInfo == NULL){
    printf("ERROR: Failed to Malloc dictionary");
    exit(EXIT_FAILURE);
  }
  
  dictInfo->dict = dict;
  dictInfo->dictLen = dictLen;
  dictInfo->monitor = serverToLook;

  // array of searcher threads
  pthread_t searchers[THREAD_CAPACITY];

  int i;
  for(i = 0; i < THREAD_CAPACITY; i++){
    pthread_create(&searchers[i], NULL, &dictionarySearch, (void*)dictInfo);
  }

  pthread_mutex_init(&fileMutex, NULL);
  
  // infinite loop to get connections
  while(1){
    // get size of client
    client_size = sizeof(client_address);

    printf("Attempting to connect...\n");
    // attempt to connect to a client
    connectedfd = accept(listenfd, (struct sockaddr*)&client_address,
			 &client_size);

    // check for success
    if(connectedfd == -1){
      printf("Unable to connect to client\n");
      continue;
    }

    printf("Connected\n");

    // gets information about client
    if (getnameinfo((struct sockaddr*)&client_address, client_size,
		    clientName, BUFFSIZE, clientPort, BUFFSIZE, 0)!=0) {
      fprintf(stderr, "error getting name information about client\n");
    } else {
      printf("accepted connection from %s:%s\n", clientName, clientPort);
    }

    printf("Added client to queue.\n");
    // inserts client connection into buffer
    sbuf_insert(serverToLook, connectedfd);
  }
}

// thread function to search a dictionary for client word
void *dictionarySearch(void * dictStruct){
  // continuous thread loop
  while(1){
    int flag = 0;
    printf("Running dictionary thread\n");

    // cast struct to struct reference
    dictThread *dictInfo = (dictThread*)dictStruct;
    
    printf("Getting client descriptor\n");
    
    // get client file descriptor
    int clientFd = sbuf_remove(dictInfo->monitor);
    
    // buffer
    char buffer[BUFFSIZE] = "Not a boy";
    
    while(strcmp(buffer, "DONE") != 0 && flag == 0){
      //      write(clientFd, "Enter a word (DONE to quit)\n", 29);
      
      printf("Attempting to read from client\n");
      
      int numRead = read(clientFd, buffer, BUFFSIZE);
      
      // get word to check from client
      if(numRead == 0){
	close(clientFd);
	flag = 1;
	printf("Client disconnected\n");
	continue;
      }else if(numRead < 0){
	printf("ERROR: Failed to read from client.\n");
	exit(EXIT_FAILURE);
      }
      
      // replace newline character in client word
      buffer[numRead-1] = '\0';

      // search dict for client word
      int isWord = checkDict(dictInfo->dict, buffer, dictInfo->dictLen);
      
      if(isWord){
	if(write(clientFd, "OK\n", 3) == -1){
	  close(clientFd);
	  continue;
	}
	printf("[%s] is a word\n", buffer);
      }else{
	if(write(clientFd, "MISSPELLED\n", 11) == -1){
	  close(clientFd);
	  continue;
	}
	printf("[%s] is not a word\n", buffer);
      }
      
      if(pthread_mutex_lock(&fileMutex) != 0){
	printf("ERROR: Unable to lock file mutex.\n");
	exit(EXIT_FAILURE);
      }

      FILE * lfile = fopen(fileName, "a");

      fprintf(lfile, "[%s] : %d\n", buffer, isWord);
      
      fclose(lfile);
      
      // unlocks monitor
      if(pthread_mutex_unlock(&fileMutex)!=0){
	printf("ERROR: Unable to unlock file mutex.\n");
	exit(EXIT_FAILURE);
      }
    }
    
  }
}
