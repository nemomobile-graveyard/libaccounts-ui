#-----------------------------------------------------------------------------
# Common configuration for all projects.
#-----------------------------------------------------------------------------

CONFIG         += link_pkgconfig
CONFIG         += warn_on
MOC_DIR         = .moc
OBJECTS_DIR     = .obj
RCC_DIR         = resources
QT             -= thread

TOP_SRC_DIR     = $$PWD
TOP_BUILD_DIR   = $${TOP_SRC_DIR}/$(BUILD_DIR)
QMAKE_LIBDIR   += $${TOP_BUILD_DIR}/lib/AccountsUI
INCLUDEPATH    += $${TOP_SRC_DIR}/lib

QMAKE_CXXFLAGS += -fvisibility=hidden

# we don't like warnings...
#QMAKE_CXXFLAGS *= -Werror

# use the lines below to hide export symbols by default
	#Commented the below lines as it was not compiling
#QMAKE_CXXFLAGS *= -fvisibility=internal # or `hidden' is you like
#QMAKE_CXXFLAGS *= -fvisibility-inlines-hidden
# NOTE: with above you then need to explicitly export you API with
# e.g. #define MY_EXPORT __attribute__ ((visibility("default")))

# cleanup any generated .pc and .prl files
# QMAKE_CLEAN += $${OUT_PWD}/*.pc $${OUT_PWD}/*.prl

# End of File

