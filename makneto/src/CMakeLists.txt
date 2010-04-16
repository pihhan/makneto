cmake_minimum_required(VERSION 2.6)

project(makneto)

SET(CMAKE_BUILD_TYPE Debug)

find_package(Qt4 REQUIRED)

set(KDE4_FIND_QUIETLY false)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_HOME_DIRECTORY}/CMakeMod")
list(APPEND CMAKE_MODULE_PATH "../CMakeMod")

find_package(KDE4 REQUIRED)
find_package(QCA2 REQUIRED)

#set(QT_USE_QT3SUPPORT TRUE)
include(${QT_USE_FILE})
include (KDE4Defaults)

message(STATUS "Module path: ${CMAKE_MODULE_PATH}")
#set(Iris_DIR "../iris")
find_package(Iris REQUIRED)


include_directories( ${KDE4_INCLUDES} ${KDE4_PHONON_INCLUDES}  ${KDE4_KDECORE_INCLUDES} ${QT_INCLUDES} ${QCA2_INCLUDE_DIR} ${IRIS_INCLUDES} 
#cutestuff/util cutestuff/network cutestuff/legacy 
)


link_directories( 
#    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/makneto.dir/cutestuff/network  
#    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/makneto.dir/cutestuff/util
#    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/makneto.dir/cutestuff/legacy
    ${IRIS_LIB_PATH}
)

link_libraries( 
    ${IRIS_LIBS}
)


message("BINARY DIR: "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/makneto.dir/iris)

set(makneto_SRCS
	main.cpp
	maknetomainwindow.cpp
	maknetoview.cpp
	sidebarwidget.cpp
	connectionview.cpp
	roasterview.cpp
	connection.cpp
	makneto.cpp
	maknetocontactlist.cpp
	maknetocontact.cpp
	wbwidget.cpp
	wbscene.cpp
	wbitems.cpp
	sessionview.cpp
	sessiontabmanager.cpp
	maknetohtmlbrowser.cpp
	mediaplayer.cpp
	ftstream.cpp
	addcontactdialog.cpp
  contactdetaildialog.cpp
  mucview.cpp
  muccontrol.cpp
	chatinput.cpp
  chatoutput.cpp
  palettewidget.cpp
  wbforeign.cpp
  qgraphicswidgetitem.cpp

#plugins
  plugins/plugin.cpp
  plugins/pollplugin.cpp
  plugins/pollplugincreate.cpp

#contactlist
	contactlist/contactlistalphacomparator.cpp
	contactlist/contactlist.cpp
	contactlist/contactlistgroupitem.cpp
	contactlist/contactlistmodel.cpp
	contactlist/contactlistcontact.cpp
	contactlist/contactlistgroup.cpp
	contactlist/contactlistitem.cpp
	contactlist/contactlistview.cpp
	contactlist/status.h

# Petr Mensik a jeho tvorba
    contactlist/contactlistgroupedcontact.cpp
        featurelist.h
        featurelist.cpp
        discorequest.h
        discorequest.cpp
        maknetocontactresource.cpp

 )

kde4_moc_headers(makneto

)

kde4_add_ui_files(makneto_SRCS maknetoview_base.ui prefs_base.ui add_contact.ui contact_detail.ui)
kde4_add_kcfg_files(makneto_SRCS settings.kcfgc )

kde4_add_executable(makneto ${makneto_SRCS})

SET_TARGET_PROPERTIES(makneto PROPERTIES COMPILE_FLAGS "-g")

target_link_libraries(makneto ${KDE4_KDEUI_LIBS} ${QT_LIBRARIES} ${QCA2_LIBRARIES} ${KDE4_KHTML_LIBS} ${KDE4_PHONON_LIBS} ${IRIS_LIBRARIES})

install(TARGETS makneto DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES makneto.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES makneto.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES maknetoui.rc  DESTINATION  ${DATA_INSTALL_DIR}/makneto )

add_subdirectory( icons )
