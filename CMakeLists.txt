cmake_minimum_required(VERSION 3.16)

project(Sales_PressBrake VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        pages/mainwindow.cpp
        pages/mainwindow.h
        pages/welcomepage.cpp
        pages/welcomepage.h
        pages/loginpage.cpp
        pages/loginpage.h
        pages/userspage.cpp
        pages/userspage.h
        pages/materialspage.cpp
        pages/materialspage.h
        pages/machinespage.h
        pages/machinespage.cpp
        pages/toolingpage.h
        pages/toolingpage.cpp
        pages/adminshellpage.h
        pages/adminshellpage.cpp
        pages/optionspage.h
        pages/optionspage.cpp
        pages/recommendationpage.h
        pages/recommendationpage.cpp
        pages/quotespage.h
        pages/quotespage.cpp
        pages/usershellpage.h
        pages/usershellpage.cpp

        helpers/shortcuthelper.h
        helpers/stylehelper.h
        helpers/stylehelper.cpp
        helpers/quotepdflayout.h
        helpers/quotepdflayout.cpp
        helpers/quotepdfcommon.h
        helpers/quotepdfcommon.cpp
        helpers/quotepdfheader.h
        helpers/quotepdfheader.cpp
        helpers/quotepdfcustomer.h
        helpers/quotepdfcustomer.cpp
        helpers/quotepdfdatabuilder.h
        helpers/quotepdfdatabuilder.cpp
        helpers/quotepdfitems.h
        helpers/quotepdfitems.cpp
        helpers/recommendationservice.h
        helpers/recommendationservice.cpp
        helpers/recommendationtablehelper.h
        helpers/recommendationtablehelper.cpp
        helpers/recommendationformatutils.h
        helpers/recommendationformatutils.cpp

        widgets/userformwidget.cpp
        widgets/userformwidget.h
        widgets/userstablewidget.cpp
        widgets/userstablewidget.h
        widgets/materialstablewidget.cpp
        widgets/materialstablewidget.h
        widgets/materialformwidget.cpp
        widgets/materialformwidget.h
        widgets/machineformwidget.h
        widgets/machineformwidget.cpp
        widgets/machinestablewidget.h
        widgets/machinestablewidget.cpp
        widgets/toolingformwidget.h
        widgets/toolingformwidget.cpp
        widgets/toolingstablewidget.h
        widgets/toolingstablewidget.cpp
        widgets/optionformwidget.h
        widgets/optionformwidget.cpp
        widgets/optionstablewidget.h
        widgets/optionstablewidget.cpp

        models/user.h
        models/materialrecord.h
        models/machinerecord.h
        models/toolingrecord.h
        models/optionrecord.h
        models/recommendationresult.h
        models/toolingrecommendationresult.h
        models/quoterecord.h
        models/quotepdfdata.h

        repositories/userrepository.cpp
        repositories/userrepository.h
        repositories/materialrepository.cpp
        repositories/materialrepository.h
        repositories/machinerepository.cpp
        repositories/machinerepository.h
        repositories/toolingrepository.h
        repositories/toolingrepository.cpp
        repositories/optionrepository.h
        repositories/optionrepository.cpp
        repositories/quoterepository.h
        repositories/quoterepository.cpp

        dialogues/savequotedialog.h
        dialogues/savequotedialog.cpp
        dialogues/toastmessage.h
        dialogues/toastmessage.cpp

        resources/resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Sales_PressBrake
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(Sales_PressBrake SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(Sales_PressBrake
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Sales_PressBrake PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)



# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Sales_PressBrake)
endif()
set_target_properties(Sales_PressBrake PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Sales_PressBrake
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Sales_PressBrake)
endif()

target_include_directories(Sales_PressBrake PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/models
    ${CMAKE_CURRENT_SOURCE_DIR}/repositories
    ${CMAKE_CURRENT_SOURCE_DIR}/pages
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/helpers
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogues
)
