#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Widget.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>

#include <rtplot/rtplot_core.h>

namespace rtp {

class Fl_Plot : virtual public Fl_Widget, virtual public rtp::RTPlotCore {
public:
	Fl_Plot(int x = 0, int y = 0, int w = 0, int h = 0, const char *label = 0);
	virtual ~Fl_Plot();

	virtual void refresh() override;

	static void display_labels_btn_clb(Fl_Widget *w, void *obj);

protected:
	virtual void draw() override;
	virtual int handle(int event) override;

	virtual size_t getWidth() override;
	virtual size_t getHeight() override;
	virtual int getXPosition() override;
	virtual int getYPosition() override;
	void setSize(const Pairf& size) override;
	void setPosition(const PointXY& position) override;
	virtual void pushClip(const PointXY& start, const Pairf& size) override;
	virtual void popClip() override;
	virtual void startLine() override;
	virtual void drawLine(const PointXY& start, const PointXY& end) override;
	virtual void endLine() override;
	virtual void setLineStyle(LineStyle style) override;
	virtual void drawText(const std::string& text, const PointXY& position, int angle = 0) override;
	virtual Pairf measureText(const std::string& text) override;
	virtual void setColor(size_t palette_idx) override;
	virtual void setColor(Colors color) override;
	virtual void saveColor() override;
	virtual void restoreColor() override;

private:
	void toggleLabelsProcess();
	Fl_Button* display_labels_btn_;
	uint32_t saved_color_;
};

}
