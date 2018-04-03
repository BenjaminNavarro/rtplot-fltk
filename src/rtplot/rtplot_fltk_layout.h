#pragma once

#include <rtplot/internal/rtplot_layout.h>
#include <FL/Fl.H>
#include <FL/Fl_Table.H>
#include <vector>
#include <memory>

namespace rtp {

class RTPlotCore;

class RTPlotFLTKLayout : virtual public Fl_Table, virtual public RTPlotLayout {
public:
	RTPlotFLTKLayout(int x, int y, int w, int h, const char *l=0);
	virtual ~RTPlotFLTKLayout();

	virtual void setPlots(std::vector<std::shared_ptr<RTPlotCore>> widgets, size_t num_rows, size_t num_cols) override;

protected:
	virtual void draw_cell(TableContext context,        // table cell drawing
	                       int R=0, int C=0, int X=0, int Y=0, int W=0, int H=0) override;

	virtual void draw() override;
};

}
