#include <rtplot/rtplot_fltk.h>

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <unistd.h>

int main(int argc, char const* argv[]) {
    rtp::RTPlotFLTK rtplot;

    rtplot.enableAutoRefresh(50);
    rtplot.setGridSize(2, 2);

    for (size_t i = 0; i < 4; ++i) {
        rtplot.setPlotName(i, "Graph #" + std::to_string(i + 1));
    }

    rtplot.enableFastPlotting();

    auto sine_wave = [](float x, float amplitude, float freq,
                        float offset) -> float {
        return offset + amplitude * std::sin(x * freq);
    };

    const int points = 10000;
    float amplitude = 0.f;
    float freq[6] = {0.02f, 0.05f, 0.1f, 0.5f, 1.f, 2.f};
    float amp[6] = {100.f, 200.f, 50.f, 75.f, 20.f, 30.f};
    float offset[6] = {500.f, 200.f, 0.f, -100.f, -200.f, -300.f};

    for (size_t i = 0; i < 6; i++) {
        std::ostringstream name;
        name << "Sine " << std::setprecision(3) << freq[i] << " " << amp[i]
             << " " << offset[i];
        rtplot.setCurveLabel(0, i, name.str());
        rtplot.setXLabel(0, "Time (s)");
        rtplot.setYLabel(0, "Amplitude");
        rtplot.setCurveLabel(3, i, name.str());
        rtplot.setXLabel(3, "Time (s)");
        rtplot.setYLabel(3, "Amplitude");
    }
    rtplot.setCurveLabel(1, 0, "line");
    rtplot.setCurveLabel(2, 0, "line");

    for (size_t i = 0; i < 4; ++i) {
        rtplot.autoXRange(i);
        rtplot.autoYRange(i);
    }
    rtplot.setMaxPoints(0, points / 5);
    rtplot.setMaxPoints(3, points / 5);

    size_t total_duration = 0;
    for (int i = 0; i < points; ++i) {
        using namespace std::chrono;

        auto t_start = high_resolution_clock::now();
        float x;
        x = i / 10.f;
        i > points / 2 ? amplitude -= 1.f : amplitude += 1.f;

        {
            using namespace std;
            using namespace std::chrono;
            high_resolution_clock::time_point t1 = high_resolution_clock::now();

            for (size_t c = 0; c < 6; c++) {
                rtplot.addPoint(0, c, x,
                                sine_wave(x, amp[c], freq[c], offset[c]));
                rtplot.addPoint(3, c, x,
                                sine_wave(x, amp[c], freq[c], offset[c]));
            }

            rtplot.addPoint(1, 0, i, i);
            rtplot.addPoint(2, 0, i, i);

            high_resolution_clock::time_point t2 = high_resolution_clock::now();
            total_duration += duration_cast<nanoseconds>(t2 - t1).count() /
                              14; // 14 points per loop
        }

        std::this_thread::sleep_until(t_start + milliseconds(1));
    }

    std::cout << "Plotting done\n";
    std::cout << "It took " << total_duration / points
              << " ns in avg to add a point\n";

    rtplot.run();

    return 0;
}
