#include <rtplot.h>

#include <iostream>
#include <unistd.h>
#include <cmath>
#include <string.h>
#include <sstream>
#include <iomanip>
#include <X11/Xlib.h>

using namespace std;

int main(int argc, char* argv[]) {

	Fl::lock();
	XInitThreads();

	RTPlot plot(1, argv);

	plot.autoRefresh(true, 20);

	usleep(1000000);

	if(argc == 1) {
		plot.setXRange(-10, 10);
		plot.setYRange(-10, 10);

		Fl::run();
		plot.quit();
	}
	else if(strcmp(argv[1], "rtsine") == 0) {
		cout << "Sending 6 sine waves" << endl;

		plot.autoXRange();
		plot.autoYRange();

		auto sine_wave = [](float x, float amplitude, float freq, float offset) -> float {
							 return offset + amplitude*sinf(x*freq);
						 };

		const int points = 10000;
		float amplitude = 0.f;
		float freq[6] = {0.02f, 0.05f, 0.1f, 0.5f, 1.f, 2.f};
		float amp[6] = {100.f, 200.f, 50.f, 75.f, 20.f, 30.f};
		float offset[6] = {500.f, 200.f, 0.f, -100.f, -200.f, -300.f};

		for (size_t i = 0; i < 6; i++) {
			std::ostringstream name;
			name << "Sine " << setprecision(3) << freq[i] << " " << amp[i] << " " << offset[i];
			plot.setCurveName(i, name.str());
		}

		for (int i = 0; i < points; ++i)
		{
			float x,y1,y2;
			x = i/10.f;
			i > points/2 ? amplitude -= 1.f : amplitude += 1.f;

			for (size_t c = 0; c < 6; c++) {
				plot.newPoint(c, x, sine_wave(x, amp[c], freq[c], offset[c]));
				if(i > points/5) {
					plot.removeFirstPoint(c);
				}
			}

			if(not i%10)
				Fl::check();
			usleep(1000);
		}
		// Fl::run();
		cout << "Closing RTPlot" << endl;
		plot.quit();
	}
	else if(strcmp(argv[1], "sine") == 0) {
		cout << "Sending a sine wave" << endl;

		for (int i = 0; i <= 100; ++i)
		{
			float x,y;
			x = i/10.f;
			y = 5.f + 5.f*sinf(x);

			plot.newPoint(0, x, y);

			Fl::check();
			usleep(1000);
		}

		string tmp;
		cin >> tmp;
		cout << "Closing RTPlot" << endl;
		plot.quit();
	}


	return 0;
}
