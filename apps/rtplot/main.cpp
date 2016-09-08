#include <rtplot.h>

int main(int argc, char *argv[])
{
	Fl::lock();
	XInitThreads();

    RTPlot plot(argc, argv);

	Fl::run();

    return 0;
}
