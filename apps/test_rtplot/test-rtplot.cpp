#include <rtplot.h>

#include <FL/Fl.H>

#include <iostream>
#include <unistd.h>
#include <cmath>
#include <string.h>
#include <sstream>
#include <iomanip>
#include <X11/Xlib.h>

using namespace std;

int main(int argc, char* argv[]) {

	RTPlot plot(false);
	RTPlot plot2(false);

	plot.autoRefresh(true, 20);
	plot2.autoRefresh(true, 50);

	usleep(1000000);

	cout << "Sending 6 sine waves" << endl;

	plot.autoXRange();
	plot.autoYRange();
	plot2.autoXRange();
	plot2.autoYRange();

	auto sine_wave = [] (float x, float amplitude, float freq, float offset)->float {
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
		float x;
		x = i/10.f;
		i > points/2 ? amplitude -= 1.f : amplitude += 1.f;

		for (size_t c = 0; c < 6; c++) {
			plot.newPoint(c, x, sine_wave(x, amp[c], freq[c], offset[c]));
			if(i > points/5) {
				plot.removeFirstPoint(c);
			}
		}

		plot2.newPoint(0, i, i);

		usleep(1000);
	}
	cout << "Enter something to close the plots" << endl;
	string tmp;
	cin >> tmp;
	cout << "Closing RTPlot" << endl;
	plot.quit();
	plot2.quit();
	cout << "Goodbye" << endl;

	return 0;
}
