CC            = qcc -Vgcc_ntoarmv7le
CXX           = qcc -Vgcc_ntoarmv7le
DEFINES       = -DQT_NO_IMPORT_QT47_QML -DQ_OS_BLACKBERRY -DBB10_BLD -DQT_NO_DEBUG_OUTPUT -DQT_NO_WARNING_OUTPUT -DPJ_AUTOCONF -DQT_NO_DEBUG -DQT_SQL_LIB -DQT_XML_LIB -DQT_GUI_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -Wno-psabi -fstack-protector-strong -mcpu=cortex-a9 -mcpu=cortex-a9 -ffast-math -fstack-protector-strong -fvisibility=hidden -fPIE -pie -Wl,-z,relro -Wl,-z,now -Wformat -Wformat-security -Werror=format-security -Wall -Wextra -O2 -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -Wno-psabi -lang-c++ -fstack-protector-strong -mcpu=cortex-a9 -mcpu=cortex-a9 -ffast-math -fstack-protector-strong -fvisibility=hidden -fPIE -pie -Wl,-z,relro -Wl,-z,now -Wformat -Wformat-security -Werror=format-security -Wall -Wextra -O2 -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/opt/bb/Qt48BB10/ARM/mkspecs/default -I. -I/opt/bb/Qt48BB10/ARM/include/QtCore -I/opt/bb/Qt48BB10/ARM/include/QtNetwork -I/opt/bb/Qt48BB10/ARM/include/QtGui -I/opt/bb/Qt48BB10/ARM/include/QtXml -I/opt/bb/Qt48BB10/ARM/include/QtSql -I/opt/bb/Qt48BB10/ARM/include -I/opt/bb/Qt48BB10/ARM/include/QtSensors -I../pjsip2-bb10-trunk/pjlib/include -I../pjsip2-bb10-trunk/pjlib-util/include -I../pjsip2-bb10-trunk/pjnath/include -I../pjsip2-bb10-trunk/pjmedia/include -I../pjsip2-bb10-trunk/pjsip/include -I../pjsip2-bb10-trunk/third_party/ZRTP4PJ-master/zsrtp/include -I../pjsip2-bb10-trunk/third_party/ZRTP4PJ-master/zsrtp/zrtp -I../pjsip2-bb10-trunk/third_party/ZRTP4PJ-master/zsrtp/zrtp/zrtp -I/opt/bb/Qt48BB10/ARM/include -I/opt/bb/Qt48BB10/ARM/include/QtMobility -I.moc -I.ui -I/opt/bb/bbndk-10.3/target_10_3_0_698/qnx6/usr/include -I/opt/bb/bbndk-10.3/target_10_3_0_698/qnx6/usr/include/freetype2
LINK          = qcc -Vgcc_ntoarmv7le
LFLAGS        = -lang-c++ -Wl,-rpath-link,/opt/bb/bbndk-10.3/target_10_3_0_698/qnx6/armle-v7/lib -Wl,-rpath-link,/opt/bb/bbndk-10.3/target_10_3_0_698/qnx6/armle-v7/usr/lib -Wl,-rpath,'./app/native/lib' -Wl,-O1 -Wl,-rpath,/opt/bb/Qt48BB10/ARM/lib
LIBS          = $(SUBLIBS)  -L/opt/bb/bbndk-10.3/target_10_3_0_698/qnx6/armle-v7/lib -L/opt/bb/bbndk-10.3/target_10_3_0_698/qnx6/armle-v7/usr/lib -L/opt/bb/Qt48BB10/ARM//lib -lcpp -lbb -lbbsystem -lbbpim -lbbplatform -limg -lscreen -lcrypto -L../pjsip2-bb10-trunk/pjlib/lib -L../pjsip2-bb10-trunk/pjlib-util/lib -L../pjsip2-bb10-trunk/pjnath/lib -L../pjsip2-bb10-trunk/pjmedia/lib -L../pjsip2-bb10-trunk/pjsip/lib -L../pjsip2-bb10-trunk/third_party/lib -L../pjsip2-bb10-trunk/third_party/ZRTP4PJ-master/lib -v -lpjsua-arm-unknown-nto-qnx8.0.0eabi -lpjsip-ua-arm-unknown-nto-qnx8.0.0eabi -lpjsip-simple-arm-unknown-nto-qnx8.0.0eabi -lpjsip-arm-unknown-nto-qnx8.0.0eabi -lpjmedia-codec-arm-unknown-nto-qnx8.0.0eabi -lpjmedia-arm-unknown-nto-qnx8.0.0eabi -lpjmedia-audiodev-arm-unknown-nto-qnx8.0.0eabi -lpjnath-arm-unknown-nto-qnx8.0.0eabi -lpjlib-util-arm-unknown-nto-qnx8.0.0eabi -lpj-arm-unknown-nto-qnx8.0.0eabi -lportaudio-arm-unknown-nto-qnx8.0.0eabi -lgsmcodec-arm-unknown-nto-qnx8.0.0eabi -lilbccodec-arm-unknown-nto-qnx8.0.0eabi -lspeex-arm-unknown-nto-qnx8.0.0eabi -lresample-arm-unknown-nto-qnx8.0.0eabi -lmilenage-arm-unknown-nto-qnx8.0.0eabi -lsrtp-arm-unknown-nto-qnx8.0.0eabi -lzsrtp-arm-unknown-nto-qnx8.0.0eabi -lasound -lssl -laudio_manager -L/opt/bb/Qt48BB10/ARM/lib -lQtSensors -lQtSql -L/opt/bb/Qt48BB10/ARM//lib -L/opt/bb/bbndk-10.2/target_10_0_10_822/qnx6/armle-v7/lib -L/opt/bb/bbndk-10.2/target_10_0_10_822/qnx6/armle-v7/usr/lib -lQtXml -lQtGui -lQtNetwork -lsocket -lQtCore -lm -lbps 
AR            = ntoarmv7-ar cqs
RANLIB        = ntoarmv7-ranlib
QMAKE         = /opt/bb/Qt48BB10/ARM/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = ntoarmv7-strip
INSTALL_FILE  = $(COPY_FILE)
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = $(COPY_FILE)
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = .obj/

####### Files

SOURCES       = main.cpp \
		mainwindow.cpp \
		PjCallback.cpp \
		account.cpp \
		callwidget.cpp \
		cdr.cpp \
		callhistory.cpp \
		addressbook.cpp \
		richtextpushbutton.cpp \
		settingsdialog.cpp \
		genconfig.cpp \
		accountssettings.cpp \
		generalsettings.cpp \
		sysdepapp.cpp \
		calldetails.cpp \
		contactdetails.cpp \
		cont.cpp \
		helpwindow.cpp \
		exttabwidget.cpp \
		historyactionmenu.cpp \
		actionmenu.cpp \
		contactsactionmenu.cpp \
		accountsactionmenu.cpp \
		QsKineticScroller.cpp \
		callsactionmenu.cpp \
		dialeractionmenu.cpp \
		flickcharm.cpp \
		helpdoc.cpp \
		bpshandler.cpp \
		phone.cpp \
		callinfo.cpp \
		projectstrings.cpp \
		transferactionmenu.cpp \
		messagehistory.cpp \
		messageview.cpp \
		messagefile.cpp \
		messagerec.cpp \
		messagedb.cpp \
		messagereclist.cpp \
		htmldelegate.cpp \
		extlineedit.cpp \
		styledcombobox.cpp \
		lineeditwitherase.cpp \
		accountnewdialog.cpp \
		reginfo.cpp \
		transportinfo.cpp \
		accountstatus.cpp \
		contactview.cpp .moc/moc_mainwindow.cpp \
		.moc/moc_PjCallback.cpp \
		.moc/moc_callwidget.cpp \
		.moc/moc_callhistory.cpp \
		.moc/moc_addressbook.cpp \
		.moc/moc_richtextpushbutton.cpp \
		.moc/moc_settingsdialog.cpp \
		.moc/moc_genconfig.cpp \
		.moc/moc_accountssettings.cpp \
		.moc/moc_generalsettings.cpp \
		.moc/moc_sysdepapp.cpp \
		.moc/moc_calldetails.cpp \
		.moc/moc_contactdetails.cpp \
		.moc/moc_helpwindow.cpp \
		.moc/moc_historyactionmenu.cpp \
		.moc/moc_actionmenu.cpp \
		.moc/moc_contactsactionmenu.cpp \
		.moc/moc_accountsactionmenu.cpp \
		.moc/moc_QsKineticScroller.cpp \
		.moc/moc_callsactionmenu.cpp \
		.moc/moc_dialeractionmenu.cpp \
		.moc/moc_flickcharm.cpp \
		.moc/moc_helpdoc.cpp \
		.moc/moc_bpshandler.cpp \
		.moc/moc_phone.cpp \
		.moc/moc_transferactionmenu.cpp \
		.moc/moc_messagehistory.cpp \
		.moc/moc_messageview.cpp \
		.moc/moc_messagedb.cpp \
		.moc/moc_extlineedit.cpp \
		.moc/moc_styledcombobox.cpp \
		.moc/moc_lineeditwitherase.cpp \
		.moc/moc_accountnewdialog.cpp \
		.moc/moc_accountstatus.cpp \
		.moc/moc_contactview.cpp \
		.qrc/qrc_taki.cpp
OBJECTS       = .obj/main.o \
		.obj/mainwindow.o \
		.obj/PjCallback.o \
		.obj/account.o \
		.obj/callwidget.o \
		.obj/cdr.o \
		.obj/callhistory.o \
		.obj/addressbook.o \
		.obj/richtextpushbutton.o \
		.obj/settingsdialog.o \
		.obj/genconfig.o \
		.obj/accountssettings.o \
		.obj/generalsettings.o \
		.obj/sysdepapp.o \
		.obj/calldetails.o \
		.obj/contactdetails.o \
		.obj/cont.o \
		.obj/helpwindow.o \
		.obj/exttabwidget.o \
		.obj/historyactionmenu.o \
		.obj/actionmenu.o \
		.obj/contactsactionmenu.o \
		.obj/accountsactionmenu.o \
		.obj/QsKineticScroller.o \
		.obj/callsactionmenu.o \
		.obj/dialeractionmenu.o \
		.obj/flickcharm.o \
		.obj/helpdoc.o \
		.obj/bpshandler.o \
		.obj/phone.o \
		.obj/callinfo.o \
		.obj/projectstrings.o \
		.obj/transferactionmenu.o \
		.obj/messagehistory.o \
		.obj/messageview.o \
		.obj/messagefile.o \
		.obj/messagerec.o \
		.obj/messagedb.o \
		.obj/messagereclist.o \
		.obj/htmldelegate.o \
		.obj/extlineedit.o \
		.obj/styledcombobox.o \
		.obj/lineeditwitherase.o \
		.obj/accountnewdialog.o \
		.obj/reginfo.o \
		.obj/transportinfo.o \
		.obj/accountstatus.o \
		.obj/contactview.o \
		.obj/moc_mainwindow.o \
		.obj/moc_PjCallback.o \
		.obj/moc_callwidget.o \
		.obj/moc_callhistory.o \
		.obj/moc_addressbook.o \
		.obj/moc_richtextpushbutton.o \
		.obj/moc_settingsdialog.o \
		.obj/moc_genconfig.o \
		.obj/moc_accountssettings.o \
		.obj/moc_generalsettings.o \
		.obj/moc_sysdepapp.o \
		.obj/moc_calldetails.o \
		.obj/moc_contactdetails.o \
		.obj/moc_helpwindow.o \
		.obj/moc_historyactionmenu.o \
		.obj/moc_actionmenu.o \
		.obj/moc_contactsactionmenu.o \
		.obj/moc_accountsactionmenu.o \
		.obj/moc_QsKineticScroller.o \
		.obj/moc_callsactionmenu.o \
		.obj/moc_dialeractionmenu.o \
		.obj/moc_flickcharm.o \
		.obj/moc_helpdoc.o \
		.obj/moc_bpshandler.o \
		.obj/moc_phone.o \
		.obj/moc_transferactionmenu.o \
		.obj/moc_messagehistory.o \
		.obj/moc_messageview.o \
		.obj/moc_messagedb.o \
		.obj/moc_extlineedit.o \
		.obj/moc_styledcombobox.o \
		.obj/moc_lineeditwitherase.o \
		.obj/moc_accountnewdialog.o \
		.obj/moc_accountstatus.o \
		.obj/moc_contactview.o \
		.obj/qrc_taki.o
DIST          = /opt/bb/Qt48BB10/ARM/mkspecs/common/g++-base.conf \
		/opt/bb/Qt48BB10/ARM/mkspecs/common/g++-unix.conf \
		/opt/bb/Qt48BB10/ARM/mkspecs/common/unix.conf \
		/opt/bb/Qt48BB10/ARM/mkspecs/common/qcc-base.conf \
		/opt/bb/Qt48BB10/ARM/mkspecs/common/qcc-base-qnx.conf \
		/opt/bb/Qt48BB10/ARM/mkspecs/qconfig.pri \
		/opt/bb/Qt48BB10/ARM/mkspecs/features/qt_functions.prf \
		/opt/bb/Qt48BB10/ARM/mkspecs/features/qt_config.prf \
		/opt/bb/Qt48BB10/ARM/mkspecs/qnx-armv7le-qcc/qmake.conf \
		/opt/bb/Qt48BB10/ARM/mkspecs/features/exclusive_builds.prf \
		/opt/bb/Qt48BB10/ARM/mkspecs/features/default_pre.prf \
		/opt/bb/Qt48BB10/ARM/mkspecs/features/release.prf \
		/opt/bb/Qt48BB10/ARM/mkspecs/features/default_post.prf \
		/opt/bb/Qt48BB10/ARM/mkspecs/features/mobility.prf \
		/opt/bb/Qt48BB10/ARM/mkspecs/features/shared.prf \
		/opt/bb/Qt48BB10/ARM/mkspecs/features/warn_on.prf \
		/opt/bb/Qt48BB10/ARM/mkspecs/features/qt.prf \
		/opt/bb/Qt48BB10/ARM/mkspecs/features/unix/thread.prf \
		/opt/bb/Qt48BB10/ARM/mkspecs/features/moc.prf \
		/opt/bb/Qt48BB10/ARM/mkspecs/features/resources.prf \
		/opt/bb/Qt48BB10/ARM/mkspecs/features/uic.prf \
		/opt/bb/Qt48BB10/ARM/mkspecs/features/yacc.prf \
		/opt/bb/Qt48BB10/ARM/mkspecs/features/lex.prf \
		taki.pro
QMAKE_TARGET  = taki
DESTDIR       = 
TARGET        = taki

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET): .ui/ui_mainwindow.h .ui/ui_call.h .ui/ui_settingsdialog.h .ui/ui_genconfig.h .ui/ui_calldetails.h .ui/ui_contactdetails.h .ui/ui_helpwindow.h .ui/ui_historyactionmenu.h .ui/ui_contactsactionmenu.h .ui/ui_accountsactionmenu.h .ui/ui_callsactionmenu.h .ui/ui_dialeractionmenu.h .ui/ui_helpdoc.h .ui/ui_transferactionmenu.h .ui/ui_messagehistory.h .ui/ui_messageview.h .ui/ui_accountnewdialog.h .ui/ui_accountstatus.h .ui/ui_contactview.h $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile

check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: .moc/moc_mainwindow.cpp .moc/moc_PjCallback.cpp .moc/moc_callwidget.cpp .moc/moc_callhistory.cpp .moc/moc_addressbook.cpp .moc/moc_richtextpushbutton.cpp .moc/moc_settingsdialog.cpp .moc/moc_genconfig.cpp .moc/moc_accountssettings.cpp .moc/moc_generalsettings.cpp .moc/moc_sysdepapp.cpp .moc/moc_calldetails.cpp .moc/moc_contactdetails.cpp .moc/moc_helpwindow.cpp .moc/moc_historyactionmenu.cpp .moc/moc_actionmenu.cpp .moc/moc_contactsactionmenu.cpp .moc/moc_accountsactionmenu.cpp .moc/moc_QsKineticScroller.cpp .moc/moc_callsactionmenu.cpp .moc/moc_dialeractionmenu.cpp .moc/moc_flickcharm.cpp .moc/moc_helpdoc.cpp .moc/moc_bpshandler.cpp .moc/moc_phone.cpp .moc/moc_transferactionmenu.cpp .moc/moc_messagehistory.cpp .moc/moc_messageview.cpp .moc/moc_messagedb.cpp .moc/moc_extlineedit.cpp .moc/moc_styledcombobox.cpp .moc/moc_lineeditwitherase.cpp .moc/moc_accountnewdialog.cpp .moc/moc_accountstatus.cpp .moc/moc_contactview.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) .moc/moc_mainwindow.cpp .moc/moc_PjCallback.cpp .moc/moc_callwidget.cpp .moc/moc_callhistory.cpp .moc/moc_addressbook.cpp .moc/moc_richtextpushbutton.cpp .moc/moc_settingsdialog.cpp .moc/moc_genconfig.cpp .moc/moc_accountssettings.cpp .moc/moc_generalsettings.cpp .moc/moc_sysdepapp.cpp .moc/moc_calldetails.cpp .moc/moc_contactdetails.cpp .moc/moc_helpwindow.cpp .moc/moc_historyactionmenu.cpp .moc/moc_actionmenu.cpp .moc/moc_contactsactionmenu.cpp .moc/moc_accountsactionmenu.cpp .moc/moc_QsKineticScroller.cpp .moc/moc_callsactionmenu.cpp .moc/moc_dialeractionmenu.cpp .moc/moc_flickcharm.cpp .moc/moc_helpdoc.cpp .moc/moc_bpshandler.cpp .moc/moc_phone.cpp .moc/moc_transferactionmenu.cpp .moc/moc_messagehistory.cpp .moc/moc_messageview.cpp .moc/moc_messagedb.cpp .moc/moc_extlineedit.cpp .moc/moc_styledcombobox.cpp .moc/moc_lineeditwitherase.cpp .moc/moc_accountnewdialog.cpp .moc/moc_accountstatus.cpp .moc/moc_contactview.cpp
.moc/moc_mainwindow.cpp: callinfo.h \
		callwidget.h \
		cdr.h \
		.ui/ui_call.h \
		account.h \
		reginfo.h \
		transportinfo.h \
		callhistory.h \
		addressbook.h \
		cont.h \
		generalsettings.h \
		accountssettings.h \
		QsKineticScroller.h \
		messagerec.h \
		messagedb.h \
		messagereclist.h \
		htmldelegate.h \
		bpshandler.h \
		mainwindow.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ mainwindow.h -o .moc/moc_mainwindow.cpp

.moc/moc_PjCallback.cpp: callinfo.h \
		messagerec.h \
		cont.h \
		reginfo.h \
		transportinfo.h \
		PjCallback.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ PjCallback.h -o .moc/moc_PjCallback.cpp

.moc/moc_callwidget.cpp: cdr.h \
		.ui/ui_call.h \
		callwidget.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ callwidget.h -o .moc/moc_callwidget.cpp

.moc/moc_callhistory.cpp: callhistory.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ callhistory.h -o .moc/moc_callhistory.cpp

.moc/moc_addressbook.cpp: cont.h \
		addressbook.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ addressbook.h -o .moc/moc_addressbook.cpp

.moc/moc_richtextpushbutton.cpp: richtextpushbutton.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ richtextpushbutton.h -o .moc/moc_richtextpushbutton.cpp

.moc/moc_settingsdialog.cpp: account.h \
		reginfo.h \
		transportinfo.h \
		QsKineticScroller.h \
		settingsdialog.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ settingsdialog.h -o .moc/moc_settingsdialog.cpp

.moc/moc_genconfig.cpp: generalsettings.h \
		QsKineticScroller.h \
		genconfig.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ genconfig.h -o .moc/moc_genconfig.cpp

.moc/moc_accountssettings.cpp: account.h \
		reginfo.h \
		transportinfo.h \
		accountssettings.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ accountssettings.h -o .moc/moc_accountssettings.cpp

.moc/moc_generalsettings.cpp: generalsettings.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ generalsettings.h -o .moc/moc_generalsettings.cpp

.moc/moc_sysdepapp.cpp: sysdepapp.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ sysdepapp.h -o .moc/moc_sysdepapp.cpp

.moc/moc_calldetails.cpp: cdr.h \
		calldetails.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ calldetails.h -o .moc/moc_calldetails.cpp

.moc/moc_contactdetails.cpp: cont.h \
		QsKineticScroller.h \
		contactdetails.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ contactdetails.h -o .moc/moc_contactdetails.cpp

.moc/moc_helpwindow.cpp: QsKineticScroller.h \
		helpwindow.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ helpwindow.h -o .moc/moc_helpwindow.cpp

.moc/moc_historyactionmenu.cpp: actionmenu.h \
		historyactionmenu.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ historyactionmenu.h -o .moc/moc_historyactionmenu.cpp

.moc/moc_actionmenu.cpp: actionmenu.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ actionmenu.h -o .moc/moc_actionmenu.cpp

.moc/moc_contactsactionmenu.cpp: actionmenu.h \
		contactsactionmenu.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ contactsactionmenu.h -o .moc/moc_contactsactionmenu.cpp

.moc/moc_accountsactionmenu.cpp: actionmenu.h \
		accountsactionmenu.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ accountsactionmenu.h -o .moc/moc_accountsactionmenu.cpp

.moc/moc_QsKineticScroller.cpp: QsKineticScroller.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ QsKineticScroller.h -o .moc/moc_QsKineticScroller.cpp

.moc/moc_callsactionmenu.cpp: actionmenu.h \
		callsactionmenu.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ callsactionmenu.h -o .moc/moc_callsactionmenu.cpp

.moc/moc_dialeractionmenu.cpp: actionmenu.h \
		dialeractionmenu.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ dialeractionmenu.h -o .moc/moc_dialeractionmenu.cpp

.moc/moc_flickcharm.cpp: flickcharm.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ flickcharm.h -o .moc/moc_flickcharm.cpp

.moc/moc_helpdoc.cpp: QsKineticScroller.h \
		helpdoc.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ helpdoc.h -o .moc/moc_helpdoc.cpp

.moc/moc_bpshandler.cpp: bpshandler.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ bpshandler.h -o .moc/moc_bpshandler.cpp

.moc/moc_phone.cpp: account.h \
		reginfo.h \
		transportinfo.h \
		accountssettings.h \
		generalsettings.h \
		cont.h \
		messagerec.h \
		PjCallback.h \
		callinfo.h \
		phone.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ phone.h -o .moc/moc_phone.cpp

.moc/moc_transferactionmenu.cpp: actionmenu.h \
		transferactionmenu.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ transferactionmenu.h -o .moc/moc_transferactionmenu.cpp

.moc/moc_messagehistory.cpp: cont.h \
		messagerec.h \
		messagereclist.h \
		QsKineticScroller.h \
		htmldelegate.h \
		messagehistory.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ messagehistory.h -o .moc/moc_messagehistory.cpp

.moc/moc_messageview.cpp: messagerec.h \
		cont.h \
		messagereclist.h \
		QsKineticScroller.h \
		htmldelegate.h \
		messageview.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ messageview.h -o .moc/moc_messageview.cpp

.moc/moc_messagedb.cpp: messagereclist.h \
		messagerec.h \
		cont.h \
		messagedb.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ messagedb.h -o .moc/moc_messagedb.cpp

.moc/moc_extlineedit.cpp: extlineedit.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ extlineedit.h -o .moc/moc_extlineedit.cpp

.moc/moc_styledcombobox.cpp: styledcombobox.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ styledcombobox.h -o .moc/moc_styledcombobox.cpp

.moc/moc_lineeditwitherase.cpp: lineeditwitherase.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ lineeditwitherase.h -o .moc/moc_lineeditwitherase.cpp

.moc/moc_accountnewdialog.cpp: account.h \
		reginfo.h \
		transportinfo.h \
		QsKineticScroller.h \
		accountnewdialog.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ accountnewdialog.h -o .moc/moc_accountnewdialog.cpp

.moc/moc_accountstatus.cpp: QsKineticScroller.h \
		accountstatus.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ accountstatus.h -o .moc/moc_accountstatus.cpp

.moc/moc_contactview.cpp: QsKineticScroller.h \
		contactview.h
	/opt/bb/Qt48BB10/ARM//bin/moc $(DEFINES) $(INCPATH) -D__QNXNTO__ contactview.h -o .moc/moc_contactview.cpp

compiler_rcc_make_all: .qrc/qrc_taki.cpp
compiler_rcc_clean:
	-$(DEL_FILE) .qrc/qrc_taki.cpp
.qrc/qrc_taki.cpp: taki.qrc \
		icon.png \
		cert/ca-bundle.crt \
		doc/manual.html \
		icons/help.png \
		icons/cb-checked-pressed.png \
		icons/arrow-left.png \
		icons/action-menu.png \
		icons/call-disbl.png \
		icons/call.png \
		icons/dialpad.png \
		icons/ch-inbound.png \
		icons/hangup-disbl.png \
		icons/cb-unchecked-pressed-passp.png \
		icons/delete-all-disbl.png \
		icons/conf-cursor.png \
		icons/arrow-up.png \
		icons/registered.png \
		icons/details.png \
		icons/help-w.png \
		icons/stop-disbl.png \
		icons/add-contact-disbl.png \
		icons/calls-bg2.png \
		icons/delete-disbl.png \
		icons/arrow-right.png \
		icons/ch-outbound-err.png \
		icons/add-disbl.png \
		icons/edit-disbl.png \
		icons/play-disbl.png \
		icons/lock-unlocked.png \
		icons/arrow-down.png \
		icons/stop.png \
		icons/ch-outbound.png \
		icons/hangup.png \
		icons/history.png \
		icons/details-disbl.png \
		icons/handset.png \
		icons/lock-locked.png \
		icons/voicemail.png \
		icons/cb-unchecked-passp.png \
		icons/cb-unchecked.png \
		icons/handset-disbl.png \
		icons/contacts.png \
		icons/speaker.png \
		icons/settings-w.png \
		icons/ch-inbound-err.png \
		icons/settings.png \
		icons/delete.png \
		icons/copy.png \
		icons/delete-all.png \
		icons/settings-w-disbl.png \
		icons/cb-unchecked-pressed.png \
		icons/support-w.png \
		icons/contact.png \
		icons/edit.png \
		icons/messagess.png \
		icons/messagess-w.png \
		icons/add.png \
		icons/registration-error.png \
		icons/cb-checked-pressed-passp.png \
		icons/play.png \
		icons/add-contact.png \
		icons/cb-checked-passp.png \
		icons/help-w2.png \
		icons/ch-inbound-rec.png \
		icons/cb-checked.png \
		icons/ch-outbound-rec.png \
		icons/speaker-disbl.png \
		icons/erase.png
	/opt/bb/Qt48BB10/ARM/bin/rcc -name taki taki.qrc -o .qrc/qrc_taki.cpp

compiler_image_collection_make_all: .ui/qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) .ui/qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all: .ui/ui_mainwindow.h .ui/ui_call.h .ui/ui_settingsdialog.h .ui/ui_genconfig.h .ui/ui_calldetails.h .ui/ui_contactdetails.h .ui/ui_helpwindow.h .ui/ui_historyactionmenu.h .ui/ui_contactsactionmenu.h .ui/ui_accountsactionmenu.h .ui/ui_callsactionmenu.h .ui/ui_dialeractionmenu.h .ui/ui_helpdoc.h .ui/ui_transferactionmenu.h .ui/ui_messagehistory.h .ui/ui_messageview.h .ui/ui_accountnewdialog.h .ui/ui_accountstatus.h .ui/ui_contactview.h
compiler_uic_clean:
	-$(DEL_FILE) .ui/ui_mainwindow.h .ui/ui_call.h .ui/ui_settingsdialog.h .ui/ui_genconfig.h .ui/ui_calldetails.h .ui/ui_contactdetails.h .ui/ui_helpwindow.h .ui/ui_historyactionmenu.h .ui/ui_contactsactionmenu.h .ui/ui_accountsactionmenu.h .ui/ui_callsactionmenu.h .ui/ui_dialeractionmenu.h .ui/ui_helpdoc.h .ui/ui_transferactionmenu.h .ui/ui_messagehistory.h .ui/ui_messageview.h .ui/ui_accountnewdialog.h .ui/ui_accountstatus.h .ui/ui_contactview.h
.ui/ui_mainwindow.h: mainwindow.ui \
		extlineedit.h \
		richtextpushbutton.h \
		exttabwidget.h
	/opt/bb/Qt48BB10/ARM/bin/uic mainwindow.ui -o .ui/ui_mainwindow.h

.ui/ui_call.h: call.ui
	/opt/bb/Qt48BB10/ARM/bin/uic call.ui -o .ui/ui_call.h

.ui/ui_settingsdialog.h: settingsdialog.ui \
		extlineedit.h \
		styledcombobox.h
	/opt/bb/Qt48BB10/ARM/bin/uic settingsdialog.ui -o .ui/ui_settingsdialog.h

.ui/ui_genconfig.h: genconfig.ui \
		extlineedit.h \
		styledcombobox.h
	/opt/bb/Qt48BB10/ARM/bin/uic genconfig.ui -o .ui/ui_genconfig.h

.ui/ui_calldetails.h: calldetails.ui
	/opt/bb/Qt48BB10/ARM/bin/uic calldetails.ui -o .ui/ui_calldetails.h

.ui/ui_contactdetails.h: contactdetails.ui \
		extlineedit.h
	/opt/bb/Qt48BB10/ARM/bin/uic contactdetails.ui -o .ui/ui_contactdetails.h

.ui/ui_helpwindow.h: helpwindow.ui
	/opt/bb/Qt48BB10/ARM/bin/uic helpwindow.ui -o .ui/ui_helpwindow.h

.ui/ui_historyactionmenu.h: historyactionmenu.ui
	/opt/bb/Qt48BB10/ARM/bin/uic historyactionmenu.ui -o .ui/ui_historyactionmenu.h

.ui/ui_contactsactionmenu.h: contactsactionmenu.ui
	/opt/bb/Qt48BB10/ARM/bin/uic contactsactionmenu.ui -o .ui/ui_contactsactionmenu.h

.ui/ui_accountsactionmenu.h: accountsactionmenu.ui
	/opt/bb/Qt48BB10/ARM/bin/uic accountsactionmenu.ui -o .ui/ui_accountsactionmenu.h

.ui/ui_callsactionmenu.h: callsactionmenu.ui
	/opt/bb/Qt48BB10/ARM/bin/uic callsactionmenu.ui -o .ui/ui_callsactionmenu.h

.ui/ui_dialeractionmenu.h: dialeractionmenu.ui
	/opt/bb/Qt48BB10/ARM/bin/uic dialeractionmenu.ui -o .ui/ui_dialeractionmenu.h

.ui/ui_helpdoc.h: helpdoc.ui
	/opt/bb/Qt48BB10/ARM/bin/uic helpdoc.ui -o .ui/ui_helpdoc.h

.ui/ui_transferactionmenu.h: transferactionmenu.ui
	/opt/bb/Qt48BB10/ARM/bin/uic transferactionmenu.ui -o .ui/ui_transferactionmenu.h

.ui/ui_messagehistory.h: messagehistory.ui
	/opt/bb/Qt48BB10/ARM/bin/uic messagehistory.ui -o .ui/ui_messagehistory.h

.ui/ui_messageview.h: messageview.ui \
		extlineedit.h
	/opt/bb/Qt48BB10/ARM/bin/uic messageview.ui -o .ui/ui_messageview.h

.ui/ui_accountnewdialog.h: accountnewdialog.ui \
		extlineedit.h
	/opt/bb/Qt48BB10/ARM/bin/uic accountnewdialog.ui -o .ui/ui_accountnewdialog.h

.ui/ui_accountstatus.h: accountstatus.ui
	/opt/bb/Qt48BB10/ARM/bin/uic accountstatus.ui -o .ui/ui_accountstatus.h

.ui/ui_contactview.h: contactview.ui
	/opt/bb/Qt48BB10/ARM/bin/uic contactview.ui -o .ui/ui_contactview.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_rcc_clean compiler_uic_clean 

####### Compile

.obj/main.o: main.cpp mainwindow.h \
		callinfo.h \
		callwidget.h \
		cdr.h \
		.ui/ui_call.h \
		account.h \
		reginfo.h \
		transportinfo.h \
		callhistory.h \
		addressbook.h \
		cont.h \
		generalsettings.h \
		accountssettings.h \
		QsKineticScroller.h \
		messagerec.h \
		messagedb.h \
		messagereclist.h \
		htmldelegate.h \
		bpshandler.h \
		sysdepapp.h \
		projectstrings.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/main.o main.cpp

.obj/mainwindow.o: mainwindow.cpp mainwindow.h \
		callinfo.h \
		callwidget.h \
		cdr.h \
		.ui/ui_call.h \
		account.h \
		reginfo.h \
		transportinfo.h \
		callhistory.h \
		addressbook.h \
		cont.h \
		generalsettings.h \
		accountssettings.h \
		QsKineticScroller.h \
		messagerec.h \
		messagedb.h \
		messagereclist.h \
		htmldelegate.h \
		bpshandler.h \
		.ui/ui_mainwindow.h \
		accountnewdialog.h \
		settingsdialog.h \
		genconfig.h \
		sysdepapp.h \
		calldetails.h \
		helpwindow.h \
		helpdoc.h \
		callsactionmenu.h \
		actionmenu.h \
		transferactionmenu.h \
		dialeractionmenu.h \
		contactsactionmenu.h \
		historyactionmenu.h \
		accountsactionmenu.h \
		phone.h \
		PjCallback.h \
		projectstrings.h \
		messagehistory.h \
		messageview.h \
		accountstatus.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/mainwindow.o mainwindow.cpp

.obj/PjCallback.o: PjCallback.cpp PjCallback.h \
		callinfo.h \
		messagerec.h \
		cont.h \
		reginfo.h \
		transportinfo.h \
		projectstrings.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/PjCallback.o PjCallback.cpp

.obj/account.o: account.cpp account.h \
		reginfo.h \
		transportinfo.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/account.o account.cpp

.obj/callwidget.o: callwidget.cpp callwidget.h \
		cdr.h \
		.ui/ui_call.h \
		sysdepapp.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/callwidget.o callwidget.cpp

.obj/cdr.o: cdr.cpp cdr.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/cdr.o cdr.cpp

.obj/callhistory.o: callhistory.cpp callhistory.h \
		cdr.h \
		calldetails.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/callhistory.o callhistory.cpp

.obj/addressbook.o: addressbook.cpp addressbook.h \
		cont.h \
		contactdetails.h \
		QsKineticScroller.h \
		contactview.h \
		generalsettings.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/addressbook.o addressbook.cpp

.obj/richtextpushbutton.o: richtextpushbutton.cpp richtextpushbutton.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/richtextpushbutton.o richtextpushbutton.cpp

.obj/settingsdialog.o: settingsdialog.cpp settingsdialog.h \
		account.h \
		reginfo.h \
		transportinfo.h \
		QsKineticScroller.h \
		.ui/ui_settingsdialog.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/settingsdialog.o settingsdialog.cpp

.obj/genconfig.o: genconfig.cpp genconfig.h \
		generalsettings.h \
		QsKineticScroller.h \
		.ui/ui_genconfig.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/genconfig.o genconfig.cpp

.obj/accountssettings.o: accountssettings.cpp accountssettings.h \
		account.h \
		reginfo.h \
		transportinfo.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/accountssettings.o accountssettings.cpp

.obj/generalsettings.o: generalsettings.cpp generalsettings.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/generalsettings.o generalsettings.cpp

.obj/sysdepapp.o: sysdepapp.cpp sysdepapp.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/sysdepapp.o sysdepapp.cpp

.obj/calldetails.o: calldetails.cpp calldetails.h \
		cdr.h \
		sysdepapp.h \
		.ui/ui_calldetails.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/calldetails.o calldetails.cpp

.obj/contactdetails.o: contactdetails.cpp contactdetails.h \
		cont.h \
		QsKineticScroller.h \
		.ui/ui_contactdetails.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/contactdetails.o contactdetails.cpp

.obj/cont.o: cont.cpp cont.h \
		cdr.h \
		account.h \
		reginfo.h \
		transportinfo.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/cont.o cont.cpp

.obj/helpwindow.o: helpwindow.cpp projectstrings.h \
		helpwindow.h \
		QsKineticScroller.h \
		.ui/ui_helpwindow.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/helpwindow.o helpwindow.cpp

.obj/exttabwidget.o: exttabwidget.cpp exttabwidget.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/exttabwidget.o exttabwidget.cpp

.obj/historyactionmenu.o: historyactionmenu.cpp historyactionmenu.h \
		actionmenu.h \
		.ui/ui_historyactionmenu.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/historyactionmenu.o historyactionmenu.cpp

.obj/actionmenu.o: actionmenu.cpp actionmenu.h \
		sysdepapp.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/actionmenu.o actionmenu.cpp

.obj/contactsactionmenu.o: contactsactionmenu.cpp contactsactionmenu.h \
		actionmenu.h \
		.ui/ui_contactsactionmenu.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/contactsactionmenu.o contactsactionmenu.cpp

.obj/accountsactionmenu.o: accountsactionmenu.cpp accountsactionmenu.h \
		actionmenu.h \
		.ui/ui_accountsactionmenu.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/accountsactionmenu.o accountsactionmenu.cpp

.obj/QsKineticScroller.o: QsKineticScroller.cpp QsKineticScroller.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/QsKineticScroller.o QsKineticScroller.cpp

.obj/callsactionmenu.o: callsactionmenu.cpp callsactionmenu.h \
		actionmenu.h \
		.ui/ui_callsactionmenu.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/callsactionmenu.o callsactionmenu.cpp

.obj/dialeractionmenu.o: dialeractionmenu.cpp dialeractionmenu.h \
		actionmenu.h \
		.ui/ui_dialeractionmenu.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/dialeractionmenu.o dialeractionmenu.cpp

.obj/flickcharm.o: flickcharm.cpp flickcharm.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/flickcharm.o flickcharm.cpp

.obj/helpdoc.o: helpdoc.cpp helpdoc.h \
		QsKineticScroller.h \
		.ui/ui_helpdoc.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/helpdoc.o helpdoc.cpp

.obj/bpshandler.o: bpshandler.cpp bpshandler.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/bpshandler.o bpshandler.cpp

.obj/phone.o: phone.cpp phone.h \
		account.h \
		reginfo.h \
		transportinfo.h \
		accountssettings.h \
		generalsettings.h \
		cont.h \
		messagerec.h \
		PjCallback.h \
		callinfo.h \
		projectstrings.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/phone.o phone.cpp

.obj/callinfo.o: callinfo.cpp callinfo.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/callinfo.o callinfo.cpp

.obj/projectstrings.o: projectstrings.cpp projectstrings.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/projectstrings.o projectstrings.cpp

.obj/transferactionmenu.o: transferactionmenu.cpp transferactionmenu.h \
		actionmenu.h \
		.ui/ui_transferactionmenu.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/transferactionmenu.o transferactionmenu.cpp

.obj/messagehistory.o: messagehistory.cpp messagehistory.h \
		cont.h \
		messagerec.h \
		messagereclist.h \
		QsKineticScroller.h \
		htmldelegate.h \
		.ui/ui_messagehistory.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/messagehistory.o messagehistory.cpp

.obj/messageview.o: messageview.cpp messageview.h \
		messagerec.h \
		cont.h \
		messagereclist.h \
		QsKineticScroller.h \
		htmldelegate.h \
		sysdepapp.h \
		.ui/ui_messageview.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/messageview.o messageview.cpp

.obj/messagefile.o: messagefile.cpp messagefile.h \
		messagereclist.h \
		messagerec.h \
		cont.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/messagefile.o messagefile.cpp

.obj/messagerec.o: messagerec.cpp messagerec.h \
		cont.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/messagerec.o messagerec.cpp

.obj/messagedb.o: messagedb.cpp messagedb.h \
		messagereclist.h \
		messagerec.h \
		cont.h \
		messagefile.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/messagedb.o messagedb.cpp

.obj/messagereclist.o: messagereclist.cpp messagereclist.h \
		messagerec.h \
		cont.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/messagereclist.o messagereclist.cpp

.obj/htmldelegate.o: htmldelegate.cpp htmldelegate.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/htmldelegate.o htmldelegate.cpp

.obj/extlineedit.o: extlineedit.cpp extlineedit.h \
		sysdepapp.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/extlineedit.o extlineedit.cpp

.obj/styledcombobox.o: styledcombobox.cpp styledcombobox.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/styledcombobox.o styledcombobox.cpp

.obj/lineeditwitherase.o: lineeditwitherase.cpp lineeditwitherase.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/lineeditwitherase.o lineeditwitherase.cpp

.obj/accountnewdialog.o: accountnewdialog.cpp accountnewdialog.h \
		account.h \
		reginfo.h \
		transportinfo.h \
		QsKineticScroller.h \
		.ui/ui_accountnewdialog.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/accountnewdialog.o accountnewdialog.cpp

.obj/reginfo.o: reginfo.cpp reginfo.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/reginfo.o reginfo.cpp

.obj/transportinfo.o: transportinfo.cpp transportinfo.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/transportinfo.o transportinfo.cpp

.obj/accountstatus.o: accountstatus.cpp accountstatus.h \
		QsKineticScroller.h \
		account.h \
		reginfo.h \
		transportinfo.h \
		.ui/ui_accountstatus.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/accountstatus.o accountstatus.cpp

.obj/contactview.o: contactview.cpp cont.h \
		contactview.h \
		QsKineticScroller.h \
		sysdepapp.h \
		.ui/ui_contactview.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/contactview.o contactview.cpp

.obj/moc_mainwindow.o: .moc/moc_mainwindow.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_mainwindow.o .moc/moc_mainwindow.cpp

.obj/moc_PjCallback.o: .moc/moc_PjCallback.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_PjCallback.o .moc/moc_PjCallback.cpp

.obj/moc_callwidget.o: .moc/moc_callwidget.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_callwidget.o .moc/moc_callwidget.cpp

.obj/moc_callhistory.o: .moc/moc_callhistory.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_callhistory.o .moc/moc_callhistory.cpp

.obj/moc_addressbook.o: .moc/moc_addressbook.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_addressbook.o .moc/moc_addressbook.cpp

.obj/moc_richtextpushbutton.o: .moc/moc_richtextpushbutton.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_richtextpushbutton.o .moc/moc_richtextpushbutton.cpp

.obj/moc_settingsdialog.o: .moc/moc_settingsdialog.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_settingsdialog.o .moc/moc_settingsdialog.cpp

.obj/moc_genconfig.o: .moc/moc_genconfig.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_genconfig.o .moc/moc_genconfig.cpp

.obj/moc_accountssettings.o: .moc/moc_accountssettings.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_accountssettings.o .moc/moc_accountssettings.cpp

.obj/moc_generalsettings.o: .moc/moc_generalsettings.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_generalsettings.o .moc/moc_generalsettings.cpp

.obj/moc_sysdepapp.o: .moc/moc_sysdepapp.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_sysdepapp.o .moc/moc_sysdepapp.cpp

.obj/moc_calldetails.o: .moc/moc_calldetails.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_calldetails.o .moc/moc_calldetails.cpp

.obj/moc_contactdetails.o: .moc/moc_contactdetails.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_contactdetails.o .moc/moc_contactdetails.cpp

.obj/moc_helpwindow.o: .moc/moc_helpwindow.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_helpwindow.o .moc/moc_helpwindow.cpp

.obj/moc_historyactionmenu.o: .moc/moc_historyactionmenu.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_historyactionmenu.o .moc/moc_historyactionmenu.cpp

.obj/moc_actionmenu.o: .moc/moc_actionmenu.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_actionmenu.o .moc/moc_actionmenu.cpp

.obj/moc_contactsactionmenu.o: .moc/moc_contactsactionmenu.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_contactsactionmenu.o .moc/moc_contactsactionmenu.cpp

.obj/moc_accountsactionmenu.o: .moc/moc_accountsactionmenu.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_accountsactionmenu.o .moc/moc_accountsactionmenu.cpp

.obj/moc_QsKineticScroller.o: .moc/moc_QsKineticScroller.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_QsKineticScroller.o .moc/moc_QsKineticScroller.cpp

.obj/moc_callsactionmenu.o: .moc/moc_callsactionmenu.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_callsactionmenu.o .moc/moc_callsactionmenu.cpp

.obj/moc_dialeractionmenu.o: .moc/moc_dialeractionmenu.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_dialeractionmenu.o .moc/moc_dialeractionmenu.cpp

.obj/moc_flickcharm.o: .moc/moc_flickcharm.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_flickcharm.o .moc/moc_flickcharm.cpp

.obj/moc_helpdoc.o: .moc/moc_helpdoc.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_helpdoc.o .moc/moc_helpdoc.cpp

.obj/moc_bpshandler.o: .moc/moc_bpshandler.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_bpshandler.o .moc/moc_bpshandler.cpp

.obj/moc_phone.o: .moc/moc_phone.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_phone.o .moc/moc_phone.cpp

.obj/moc_transferactionmenu.o: .moc/moc_transferactionmenu.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_transferactionmenu.o .moc/moc_transferactionmenu.cpp

.obj/moc_messagehistory.o: .moc/moc_messagehistory.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_messagehistory.o .moc/moc_messagehistory.cpp

.obj/moc_messageview.o: .moc/moc_messageview.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_messageview.o .moc/moc_messageview.cpp

.obj/moc_messagedb.o: .moc/moc_messagedb.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_messagedb.o .moc/moc_messagedb.cpp

.obj/moc_extlineedit.o: .moc/moc_extlineedit.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_extlineedit.o .moc/moc_extlineedit.cpp

.obj/moc_styledcombobox.o: .moc/moc_styledcombobox.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_styledcombobox.o .moc/moc_styledcombobox.cpp

.obj/moc_lineeditwitherase.o: .moc/moc_lineeditwitherase.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_lineeditwitherase.o .moc/moc_lineeditwitherase.cpp

.obj/moc_accountnewdialog.o: .moc/moc_accountnewdialog.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_accountnewdialog.o .moc/moc_accountnewdialog.cpp

.obj/moc_accountstatus.o: .moc/moc_accountstatus.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_accountstatus.o .moc/moc_accountstatus.cpp

.obj/moc_contactview.o: .moc/moc_contactview.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/moc_contactview.o .moc/moc_contactview.cpp

.obj/qrc_taki.o: .qrc/qrc_taki.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/qrc_taki.o .qrc/qrc_taki.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:

