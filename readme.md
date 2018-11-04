Spelling Server
Design:
   Upon running the server, the dictionary file is processed, the bounded
   buffer is created using a monitor, the listener socket is instantiated,
   and the worker threads are initialized and dispatched to use the monitor
   and the dictionarySearch function. The server then loops indefinitely,
   listening on the listener socket and connecting to new clients. As it
   connects to a client, the client file descriptor is added to the monitor's
   bounded buffer. The worker threads are notified of the full slot and remove
   the client. The worker thread reads words from the client until done is read
   or the client using a keyboard interrupt. Each word is looked up in the
   dictionary and the result is sent to the client. The logfile is then locked
   by the thread and the word and whether it exists or not is logged. The
   logfile is then unlocked. When a client finishes entering all words, the
   worker thread closes the client connection.

Running:
   Server:
      The server is meant to run indefinately. As such, a keyboard interrupt
      is the only way to end the server program. The sever is defaulted to
      hold 2 clients in the bounded buffer. The server is defaulted to have
      2 worker threads to process clients.
      ./spellCheck:
          Defaults to port 1111 and the dictionary file words.txt.
      ./spellCheck dictfile.txt portNumber
          Switches dictionary to desired dictionary and port to desired
	  port number.
      ./spellCheck dictfile.txt
          Switches dictionary to desired dictionary file.
      Words within the dictionary file must be newline delimited.

   Client:
      Testing was done using netcat. Clients can disconnect by typing done.
      Clients can also disconnect with a keyboard interrupt without harming
      the server.

Scripts:
   main.c:
      main:
         Initially, the dictionary functions are called and the dictionary is
	 read and stored in a variable. Also, the monitor functions are called
	 using functions from server.c to set up the monitor. A struct is also
	 initialized to pass the monitor and dictionary through individual
      	 threads. After setup, the main function accepts connections from
	 clients and adds them to the monitor's queue indefinitely. 

      dictionarySearch:
	 The threads loop indefinitely within this function. They first grab a
	 connection from the monitor. Next, they read a word from the client
	 until "done" is sent by the client or the client disconnects. After
	 obtaining a word, they look the word up in the dictionary. The client
	 is notified if their word exists. The logfile is then locked by a
	 global pthread lock, the thread records the word along with its
	 success, and then the logfile is unlocked.
	 
   dictionary.c
      readDict:
	reads a dictionary file and returns a char** array representation of
	the dictionary. The returned dictionary is in all caps and unsorted.

      checkDict:
	Checks to see whether a word exists in a dictionary. Returns 1 if it
	does exist and 0 if it does not

      printDict:
	prints a dictionary to the terminal. This was used for debugging
	purposes
   
   utility.c
      sbuf_init:
	initializes values within monitor to a desired amount.

      sbuf_insert:
	Locks the monitor and waits on the monitor's empty_slot condition
	variable. Then, inserts a client file descriptor within the monitor's
	queue. Signals the monitor's full slot condition variable. Unlocks the
	monitor

      sbuf_remove:
	Locks the monitor and waits on the monitor's full_slot condition
	variable. Then, removes a client file descriptor from the monitor's
	queue. Signals the monitor's full slot condition variable. Unlocks the
	monitor

      sbuf_free:
	frees the bounded buffer of the monitor

   server.c
      getlistenfd:
	creates a socket for the server to listen through. returns this file
	descriptor to the calling function.