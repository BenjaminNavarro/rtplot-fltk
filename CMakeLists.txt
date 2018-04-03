CMAKE_MINIMUM_REQUIRED(VERSION 3.0.2)
set(WORKSPACE_DIR ${CMAKE_SOURCE_DIR}/../.. CACHE PATH "root of the packages workspace directory")
list(APPEND CMAKE_MODULE_PATH ${WORKSPACE_DIR}/share/cmake/system) # using generic scripts/modules of the workspace
include(Package_Definition NO_POLICY_SCOPE)

PROJECT(rtplot-fltk)

declare_PID_Package(
			AUTHOR 			Benjamin Navarro
			INSTITUTION		LIRMM
			MAIL			navarro@lirmm.fr
			ADDRESS 		git@github.com:BenjaminNavarro/rtplot-fltk.git
			PUBLIC_ADDRESS 	https://github.com/BenjaminNavarro/rtplot-fltk.git
			YEAR 			2018
			LICENSE 		CeCILL
			DESCRIPTION 	"Real time plotting library and application based on FLTK"
		)


add_PID_Package_Author(AUTHOR Robin Passama INSTITUTION LIRMM) #debugging and adapting to PID API changes
set_PID_Package_Version(1 1 0)

#now finding packages
declare_PID_Package_Dependency(PACKAGE rtplot-core NATIVE VERSION 0.1)
declare_PID_Package_Dependency(PACKAGE fltk-gui NATIVE VERSION 0.4.1)

build_PID_Package()
