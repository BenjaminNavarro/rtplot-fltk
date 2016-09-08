#declare library components
declare_PID_Component(
	SHARED_LIB
	NAME rtplot-lib
	DIRECTORY rtplot
	EXPORTED COMPILER_OPTIONS -std=c++14)

declare_PID_Component_Dependency(
	COMPONENT rtplot-lib
	NATIVE fltk
	PACKAGE fltk-gui)
