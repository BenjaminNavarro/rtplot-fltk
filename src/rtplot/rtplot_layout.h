#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Table.H>
#include <vector>
#include <memory>

class Fl_Plot;

class RTPlotLayout : public Fl_Table {
public:
	RTPlotLayout(int x, int y, int w, int h, const char *l=0);
	~RTPlotLayout();

	void setSize(std::vector<std::shared_ptr<Fl_Plot>> widgets, size_t num_rows, size_t num_cols);

protected:
	void draw_cell(TableContext context,        // table cell drawing
	               int R=0, int C=0, int X=0, int Y=0, int W=0, int H=0);

	void draw();
};
