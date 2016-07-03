declare_PID_Component(TEST_APPLICATION NAME test-rtplot DIRECTORY test_rtplot)

declare_PID_Component_Dependency(
	COMPONENT test-rtplot
	NATIVE rtplot)

declare_PID_Component_Dependency(
	COMPONENT test-rtplot
	NATIVE rpathlib
	PACKAGE pid-rpath)

#run_PID_Test(
#	NAME sine-plot
#	COMPONENT test-rtplot
#	ARGUMENTS "rtsine")

