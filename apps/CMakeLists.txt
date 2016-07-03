declare_PID_Component(
	APPLICATION 
	NAME rtplot 
	DIRECTORY rtplot)

declare_PID_Component_Dependency(
	COMPONENT rtplot 
	NATIVE rtplot-lib)

declare_PID_Component_Dependency(
	COMPONENT  rtplot   
	NATIVE      fltk
	PACKAGE     fltk-gui)

declare_PID_Component(
	APPLICATION 
	NAME rtplot-test 
	DIRECTORY test_rtplot)

declare_PID_Component_Dependency(
	COMPONENT rtplot-test
	NATIVE rtplot-lib)
	

declare_PID_Component_Dependency(
	COMPONENT  rtplot-test   
	NATIVE      fltk
	PACKAGE     fltk-gui)

