#include "rtplot_layout.h"
#include <rtplot/Fl_Plot.H>
#include <cassert>

RTPlotLayout::RTPlotLayout(int x, int y, int w, int h, const char *l) : Fl_Table(x,y,w,h,l) {
	col_header(0);
	col_resize(1);
	col_header_height(0);
	row_header(0);
	row_resize(1);
	row_header_width(0);
	Fl::scrollbar_size(0);
	end();
}

RTPlotLayout::~RTPlotLayout() = default;

void RTPlotLayout::setSize(std::vector<std::shared_ptr<Fl_Plot>> widgets, size_t num_rows, size_t num_cols) {
	assert(widgets.size() <= num_rows * num_cols);
	clear();        // clear any previous widgets, if any
	rows(num_rows);
	cols(num_cols);

	col_width_all(w()/num_cols - 5);
	row_height_all(h()/num_rows - 5);

	begin();        // start adding widgets to group
	size_t row = 0, col = 0;
	int X,Y,W,H;
	for(auto widget: widgets) {
		if(static_cast<bool>(widget)) {
			find_cell(CONTEXT_TABLE, row, col, X, Y, W, H);
			widget->resize(X,Y,W,H);
		}

		if(col < num_cols - 1) {
			++col;
		}
		else {
			col = 0;
			++row;
		}
	}
	end();
}

void RTPlotLayout::draw_cell(TableContext context,
                             int R, int C, int X, int Y, int W, int H) {
	switch ( context ) {
	case CONTEXT_RC_RESIZE: {
		int X, Y, W, H;
		int index = 0;
		for ( int r = 0; r<rows(); r++ ) {
			for ( int c = 0; c<cols(); c++ ) {
				if ( index >= children() ) break;
				find_cell(CONTEXT_TABLE, r, c, X, Y, W, H);
				child(index++)->resize(X,Y,W,H);
			}
		}
		init_sizes();       // tell group children resized
		return;
	}

	default:
		return;
	}
}

void RTPlotLayout::draw() {
	color(FL_WHITE);
}
