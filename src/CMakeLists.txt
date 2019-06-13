PID_Component(
    SHARED
    NAME rtplot-lib 
    DIRECTORY rtplot 
    CXX_STANDARD 14
    DEPEND fltk-gui/fltk
    EXPORT rtplot-core/rtplot-core
)
