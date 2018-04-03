#pragma once

#include <rtplot/rtplot.h>

namespace rtp {

class RTPlotFLTK : virtual public rtp::RTPlot
{
public:
	RTPlotFLTK();
	~RTPlotFLTK();

	virtual void refresh() override;
	virtual void run() override;
	virtual bool check() override;

protected:
	virtual void create() override;
	virtual std::shared_ptr<rtp::RTPlotCore> makePlot() override;
};

}
