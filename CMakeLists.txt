CMAKE_MINIMUM_REQUIRED(VERSION 3.0.2)
set(WORKSPACE_DIR ${CMAKE_SOURCE_DIR}/../.. CACHE PATH "root of the packages workspace directory")
list(APPEND CMAKE_MODULE_PATH ${WORKSPACE_DIR}/share/cmake/system) # using generic scripts/modules of the workspace
include(Package_Definition NO_POLICY_SCOPE)

PROJECT(rtplot-fltk)

PID_Package(
			AUTHOR 			Benjamin Navarro
			INSTITUTION		LIRMM
			MAIL			navarro@lirmm.fr
			ADDRESS 		git@github.com:BenjaminNavarro/rtplot-fltk.git
			PUBLIC_ADDRESS 	https://github.com/BenjaminNavarro/rtplot-fltk.git
			YEAR 			2018
			LICENSE 		CeCILL
			DESCRIPTION 	"Real time plotting library and application based on FLTK"
            VERSION         1.1.0
		)


add_PID_Package_Author(AUTHOR Robin Passama INSTITUTION LIRMM) #debugging and adapting to PID API changes

PID_Dependency(rtplot-core VERSION 0.1)
PID_Dependency(fltk-gui VERSION 0.4.4)

build_PID_Package()
