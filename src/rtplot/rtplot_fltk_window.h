#pragma once

#include <rtplot/internal/rtplot_window.h>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

namespace rtp {

class RTPlotFLTKWindow : virtual public Fl_Double_Window,
                         virtual public RTPlotWindow {
public:
    RTPlotFLTKWindow(int width, int height, const char* name);
    ~RTPlotFLTKWindow();

    virtual void show() override;
    virtual void hide() override;
    virtual void setMinimumSize(size_t width, size_t height) override;
    virtual void redraw() override;

protected:
    virtual void draw() override;
};

} // namespace rtp
