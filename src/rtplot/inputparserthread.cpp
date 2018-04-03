#include <rtplot/inputparserthread.h>

#include <rtplot/rtplot.h>

#include <iostream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>

#include <semaphore.h>
#include <sys/sem.h>
#include <fcntl.h>

using namespace std;

InputParserThread::InputParserThread(RTPlot* mw) : stop_(false), mw_(mw)
{

}

InputParserThread::~InputParserThread()
{

}

void InputParserThread::run() {
	stop_ = false;
	th_ = thread(&InputParserThread::process, this);
}

void InputParserThread::process() {
	// TODO update
	// while(!stop_) {
	//  if(peek_stdin(5) != 0) { // wait for 5s before checking if we have to exit or not (stop_ flag)
	//      string input;
	//      getline(cin, input);
	//      vector<string> args;
	//      istringstream iss(input);
	//      ostringstream params;
	//      string s;
	//      while (getline(iss, s, ' '))
	//          args.push_back(s);
	//
	//      if(args.size() == 0)
	//          continue;
	//
	//      string cmd = args[0];
	//
	//      // Handle names with spaces
	//      if(cmd == "xlabel") {
	//          copy(args.begin()+1, args.end(), ostream_iterator<string>(params, " "));
	//          mw_->setXLabel(params.str());
	//      }
	//      else if(cmd == "ylabel") {
	//          copy(args.begin()+1, args.end(), ostream_iterator<string>(params, " "));
	//          mw_->setXLabel(params.str());
	//      }
	//      else if(cmd == "yname") {
	//          int curve;
	//          if(args.size() > 2) {
	//              curve = stoi(args[1]);
	//          }
	//          else
	//              curve = -1;
	//
	//          copy(args.begin()+2, args.end(), ostream_iterator<string>(params, " "));
	//          mw_->setCurveName(curve, params.str());
	//      }
	//      else {
	//          switch(args.size()) {
	//          case 1:
	//              if(cmd == "remove_point")
	//                  mw_->removeFirstPoint(0);
	//              else if(cmd == "refresh")
	//                  mw_->refresh();
	//              else if(cmd == "auto_x_range")
	//                  mw_->autoXRange();
	//              else if(cmd == "auto_y_range")
	//                  mw_->autoYRange();
	//              else if(cmd == "quit") {
	//                  stop_ = true;
	//                  mw_->quit();
	//              }
	//              break;
	//          case 2:
	//              if(cmd == "remove_point")
	//                  mw_->removeFirstPoint(stoi(args[1]));
	//              else if(cmd == "auto_refresh")
	//                  mw_->autoRefresh(args[1] == "on", 100);
	//              else if(cmd == "sem_name") {
	//                  sem_t * mutex;
	//                  if ((mutex = sem_open(args[1].c_str(), O_CREAT, 0644, 0)) == SEM_FAILED) {
	//                      cerr << "semaphore " + args[1] + "failed to initialized" << endl;
	//                  }
	//                  else {
	//                      sem_post(mutex);
	//                  }
	//              }
	//              break;
	//          case 3:
	//              if(cmd == "plot")
	//                  mw_->newPoint(0, stof(args[1]), stof(args[2]));
	//              else if(cmd == "auto_refresh")
	//                  mw_->autoRefresh(args[1] == "on", stoi(args[2]));
	//              else if(cmd == "xrange")
	//                  mw_->setXRange(stof(args[1]), stof(args[2]));
	//              else if(cmd == "yrange")
	//                  mw_->setYRange(stof(args[1]), stof(args[2]));
	//              break;
	//          case 4:
	//              if(cmd == "plot")
	//                  mw_->newPoint(stoi(args[1]), stof(args[2]), stof(args[3]));
	//              break;
	//          default:
	//              break;
	//          }
	//      }
	//  }
	// }
}

int InputParserThread::peek_stdin(unsigned int secs) {
	// timeout structure passed into select
	struct timeval tv;
	// fd_set passed into select
	fd_set fds;
	// Set up the timeout.  here we can wait for 1 second
	tv.tv_sec = secs;
	tv.tv_usec = 0;

	// Zero out the fd_set - make sure it's pristine
	FD_ZERO(&fds);
	// Set the FD that we want to read
	FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
	// select takes the last file descriptor value + 1 in the fdset to check,
	// the fdset for reads, writes, and errors.  We are only passing in reads.
	// the last parameter is the timeout.  select will return if an FD is ready or
	// the timeout has occurred
	select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
	// return 0 if STDIN is not ready to be read.
	return FD_ISSET(STDIN_FILENO, &fds);
}

void InputParserThread::stop() {
	if(not stop_) {
		stop_ = true;
		th_.join();
	}
}

void InputParserThread::join() {
	th_.join();
}
