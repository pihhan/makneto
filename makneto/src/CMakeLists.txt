project(makneto)

SET(CMAKE_BUILD_TYPE Debug)

find_package(KDE4 REQUIRED)
find_package(QCA2 REQUIRED)
find_package(Qt4 REQUIRED)

set(QT_USE_QT3SUPPORT TRUE)
include(${QT_USE_FILE})

include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} ${QT_INCLUDES} ${QCA2_INCLUDE_DIR} iris/include iris/libidn iris/xmpp-core iris/xmpp-im 
iris/jabber cutestuff/util cutestuff/network cutestuff/legacy )

link_directories( 
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/makneto.dir/iris/libidn  
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/makneto.dir/iris/xmpp-core
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/makneto.dir/iris/xmpp-im
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/makneto.dir/iris/jabber
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/makneto.dir/cutestuff/network  
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/makneto.dir/cutestuff/util
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/makneto.dir/cutestuff/legacy
)

link_libraries( 
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/makneto.dir/iris/libidn  
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/makneto.dir/iris/xmpp-core
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/makneto.dir/iris/xmpp-im
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/makneto.dir/iris/jabber
    ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/makneto.dir/cutestuff/util
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

#libidn
	iris/libidn/profiles.c
	iris/libidn/rfc3454.c
	iris/libidn/nfkc.c
	iris/libidn/stringprep.c
#iris
	iris/xmpp-core/connector.cpp
	iris/xmpp-core/tlshandler.cpp
	iris/xmpp-core/jid.cpp
	iris/xmpp-core/securestream.cpp
	iris/xmpp-core/parser.cpp
	iris/xmpp-core/xmlprotocol.cpp
	iris/xmpp-core/protocol.cpp
	iris/xmpp-core/compress.cpp
	iris/xmpp-core/compressionhandler.cpp
	iris/xmpp-core/stream.cpp
	iris/xmpp-core/simplesasl.cpp
	iris/xmpp-core/xmpp_stanza.cpp
	iris/xmpp-im/types.cpp
	iris/xmpp-im/client.cpp
	iris/xmpp-im/xmpp_features.cpp
	iris/xmpp-im/xmpp_discoitem.cpp
	iris/xmpp-im/xmpp_discoinfotask.cpp
	iris/xmpp-im/xmpp_xdata.cpp
	iris/xmpp-im/xmpp_task.cpp
	iris/xmpp-im/xmpp_tasks.cpp
	iris/xmpp-im/xmpp_xmlcommon.cpp
	iris/xmpp-im/xmpp_vcard.cpp
	iris/jabber/s5b.cpp
	iris/jabber/xmpp_ibb.cpp
	iris/jabber/filetransfer.cpp
	
#cutestuff
	cutestuff/util/bytestream.cpp
	
        cutestuff/network/bsocket.cpp
        cutestuff/network/httpconnect.cpp
        cutestuff/network/httppoll.cpp
        cutestuff/network/socks.cpp
        
	cutestuff/legacy/ndns.cpp
        cutestuff/legacy/safedelete.cpp
        cutestuff/legacy/servsock.cpp
        cutestuff/legacy/srvresolver.cpp

 )

kde4_moc_headers(makneto
        cutestuff/legacy/ndns.h
        cutestuff/legacy/safedelete.h
        cutestuff/legacy/servsock.h
        cutestuff/legacy/srvresolver.h

        cutestuff/network/bsocket.h
        cutestuff/network/httpconnect.h
        cutestuff/network/socks.h
        cutestuff/network/httppoll.h

        cutestuff/util/bytestream.h

        iris/include/xmpp.h
        iris/include/xmpp_task.h
        iris/include/xmpp_client.h
        iris/include/xmpp_clientstream.h
        iris/include/xmpp_liverosteritem.h
        iris/include/xmpp_stream.h

        iris/jabber/filetransfer.h
        iris/jabber/s5b.h
        iris/jabber/xmpp_ibb.h

        iris/xmpp-core/compress.h
        iris/xmpp-core/compressionhandler.h
        iris/xmpp-core/securestream.h

        iris/xmpp-im/xmpp_tasks.h
        iris/xmpp-im/xmpp_discoinfotask.h
)

kde4_add_ui_files(makneto_SRCS maknetoview_base.ui prefs_base.ui)
kde4_add_kcfg_files(makneto_SRCS settings.kcfgc )

kde4_add_executable(makneto ${makneto_SRCS})

SET_TARGET_PROPERTIES(makneto PROPERTIES COMPILE_FLAGS "-g -O2")

target_link_libraries(makneto ${KDE4_KDEUI_LIBS} ${QT_LIBRARIES} ${QCA2_LIBRARIES} ${KDE4_KHTML_LIBS})

install(TARGETS makneto DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES makneto.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES makneto.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES maknetoui.rc  DESTINATION  ${DATA_INSTALL_DIR}/makneto )

add_subdirectory( icons )
