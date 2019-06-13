# TODO remove the app until the new nanomsg interface is ready
# declare_PID_Component(
# 	EXAMPLE_APPLICATION
# 	NAME rtplot
# 	DIRECTORY rtplot)
#
# declare_PID_Component_Dependency(
# 	COMPONENT rtplot
# 	NATIVE rtplot-lib)
#
# declare_PID_Component_Dependency(
# 	COMPONENT  rtplot
# 	NATIVE      fltk
# 	PACKAGE     fltk-gui)

PID_Component(
	EXAMPLE
	NAME rtplot-test
	DIRECTORY test_rtplot
    DEPEND rtplot-lib)
