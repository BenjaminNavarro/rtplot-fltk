#include <rtplot/rtplot.h>
#include <FL/Fl.H>

#include <vector>
#include <cassert>
#include <iostream>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <chrono>


int main(int argc, char const *argv[]) {
	RTPlot rtplot;

	rtplot.autoRefresh(true, 50);
	rtplot.setGridSize(2, 2);

	for (size_t i = 0; i < 4; ++i) {
		rtplot.setPlotName(i, "Graph #" + std::to_string(i+1));
	}

	auto sine_wave = [] (float x, float amplitude, float freq, float offset) -> float {
						 return offset + amplitude*std::sin(x*freq);
					 };

	const int points = 10000;
	float amplitude = 0.f;
	float freq[6] = {0.02f, 0.05f, 0.1f, 0.5f, 1.f, 2.f};
	float amp[6] = {100.f, 200.f, 50.f, 75.f, 20.f, 30.f};
	float offset[6] = {500.f, 200.f, 0.f, -100.f, -200.f, -300.f};

	for (size_t i = 0; i < 6; i++) {
		std::ostringstream name;
		name << "Sine " << std::setprecision(3) << freq[i] << " " << amp[i] << " " << offset[i];
		rtplot.setCurveName(0, i, name.str());
		rtplot.setXLabel(0, "Time (s)");
		rtplot.setYLabel(0, "Amplitude");
		rtplot.setCurveName(3, i, name.str());
		rtplot.setXLabel(3, "Time (s)");
		rtplot.setYLabel(3, "Amplitude");
	}
	rtplot.setCurveName(1, 0, "line");
	rtplot.setCurveName(2, 0, "line");

	for(size_t i=0; i<4; ++i) {
		rtplot.autoXRange(i);
		rtplot.autoYRange(i);
	}
	rtplot.setMaxPoints(0, points/5);
	rtplot.setMaxPoints(3, points/5);

	size_t total_duration = 0;
	for (int i = 0; i < points; ++i)
	{
		float x;
		x = i/10.f;
		i > points/2 ? amplitude -= 1.f : amplitude += 1.f;

		for (size_t c = 0; c < 6; c++) {
			rtplot.newPoint(0, c, x, sine_wave(x, amp[c], freq[c], offset[c]));
			rtplot.newPoint(3, c, x, sine_wave(x, amp[c], freq[c], offset[c]));
		}

		{
			using namespace std;
			using namespace std::chrono;
			high_resolution_clock::time_point t1 = high_resolution_clock::now();
			rtplot.newPoint(1, 0, i, i);
			high_resolution_clock::time_point t2 = high_resolution_clock::now();
			total_duration += duration_cast<nanoseconds>( t2 - t1 ).count();
		}
		rtplot.newPoint(2, 0, i, i);

		usleep(1000);
	}

	std::cout << "Plotting done\n";
	std::cout << "It took " << total_duration/points << " ns in avg to add a point\n";

	rtplot.run();

	return 0;
}
