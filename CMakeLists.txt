CMAKE_MINIMUM_REQUIRED(VERSION 3.0.2)
set(WORKSPACE_DIR ${CMAKE_SOURCE_DIR}/../.. CACHE PATH "root of the packages workspace directory")
list(APPEND CMAKE_MODULE_PATH ${WORKSPACE_DIR}/share/cmake/system) # using generic scripts/modules of the workspace
include(Package_Definition NO_POLICY_SCOPE)

PROJECT(rtplot-fltk)

declare_PID_Package(	
			AUTHOR 		Benjamin Navarro
			INSTITUTION	LIRMM
			MAIL		navarro@lirmm.fr
			ADDRESS 	git@gite.lirmm.fr:pid/rtplot-fltk.git
			YEAR 		2016 
			LICENSE 	CeCILL
			DESCRIPTION 	"Real time plotting library and application based on FLTK"
		)

set_PID_Package_Version(0 2 0)

#now finding packages
find_Package(fltk-gui 0.4 REQUIRED)
declare_PID_Package_Dependency(
	PACKAGE fltk-gui
	NATIVE VERSION 0.4)

find_package (pid-rpath 2.0 REQUIRED)
declare_PID_Package_Dependency(
	PACKAGE pid-rpath 
	NATIVE VERSION 2.0)

build_PID_Package()

