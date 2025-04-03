kcoreaddons_add_plugin(krunner_vbox SOURCES vbox.cpp INSTALL_NAMESPACE "kf${QT_MAJOR_VERSION}/krunner")
target_link_libraries(krunner_vbox KF${QT_MAJOR_VERSION}::Runner Qt::Widgets Qt::Xml
    KF${QT_MAJOR_VERSION}::I18n KF${QT_MAJOR_VERSION}::KIOCore KF${QT_MAJOR_VERSION}::KIOFileWidgets KF${QT_MAJOR_VERSION}::KIOWidgets)

install(DIRECTORY icons/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/vbox-runner)
