#include <pid/rpath.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <unistd.h>
#include <cmath>
#include <semaphore.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <string.h>

using namespace std;

int main(int argc, char* argv[]) {
	PID_EXE(argv[0]);

	if(argc <= 1)
		return -1;
	
	FILE* pipehandle = popen(PID_PATH("rtplot").c_str(),"w");

	if(pipehandle == NULL){
		cout << "RTPlot executable not found" << endl;
		return (false);
	}

	cout << "Pipe opened" << endl;

	fprintf(pipehandle, "auto_refresh on\n");
	fflush(pipehandle);

	usleep(1000000);

	fprintf(pipehandle, "yrange 5 10\n");
	fflush(pipehandle);

	if(strcmp(argv[1], "rtsine") == 0) {
		cout << "Sending a sine wave" << endl;
		
		fprintf(pipehandle, "yrange -600 600\n");
		fflush(pipehandle);

		fprintf(pipehandle, "xrange 0 1000\n");
		fflush(pipehandle);
		/*		
		fprintf(pipehandle, "auto_x_range\n");
		fflush(pipehandle);
	
		fprintf(pipehandle, "auto_y_range\n");
		fflush(pipehandle);
		*/
		for (int i = 0; i < 10000; ++i)
		{
			double x,y1,y2;
			x = i/10.0;
			y1 = 5.0 + (5.0 + 5.0*double(i)/100.0)*sin(x);
			y2 = 5.0 + (5.0 + 5.0*double(i)/100.0)*cos(x);

			fprintf(pipehandle, "plot 0 %f %f\n", x, y1);
			fflush(pipehandle);
	
			fprintf(pipehandle, "plot 1 %f %f\n", x, y2);
			fflush(pipehandle);
			
			if(x >= 100.0) {
				fprintf(pipehandle, "xrange %f %f\n", x-100., x);
				fflush(pipehandle);
				/*
				fprintf(pipehandle, "remove_point 0\n");
				fflush(pipehandle);
			
				fprintf(pipehandle, "remove_point 1\n");
				fflush(pipehandle);
				*/
			}

			usleep(1000);
		}
		string tmp;
		cin >> tmp;
		cout << "Closing RTPlot" << endl;
		fprintf(pipehandle, "quit");
		fflush(pipehandle);
	}
	else if(strcmp(argv[1], "sine") == 0) {
		cout << "Sending a sine wave" << endl;
		
		for (int i = 0; i <= 100; ++i)
		{
			double x,y;
			x = i/10.0;
			y = 5.0 + 5.0*sin(x);

			cout << "Plotting point " << x << ","  << y << endl;

			fprintf(pipehandle, "plot %f %f\n", x, y);
			fflush(pipehandle);	
			usleep(1000);
		}

		string tmp;
		cin >> tmp;
		cout << "Closing RTPlot" << endl;
		fprintf(pipehandle, "quit");
		fflush(pipehandle);
	}


	pclose(pipehandle);	
	cout << "Pipe closed" << endl;

	return 0;
}
