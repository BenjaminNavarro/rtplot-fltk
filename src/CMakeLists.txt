

#declare library components
declare_PID_Component(SHARED_LIB NAME rtplot-lib DIRECTORY rtplot CXX_STANDARD 14)
declare_PID_Component_Dependency(COMPONENT rtplot-lib EXPORT NATIVE rtplot-core PACKAGE rtplot-core)
declare_PID_Component_Dependency(COMPONENT rtplot-lib NATIVE fltk PACKAGE fltk-gui)
