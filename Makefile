# Makefile pour le projet saisie_nombre
# Auteur : ericb  / samedi 26 mars 2016, 23:11:00 (UTC+0100)
# Ce document est sous Licence GPL v2
# voir : http://www.gnu.org/licenses/gpl-2.0.html

CURRENT_DATE =  `date "+20%y-%m-%d"`
BUILD_DIR = build
VERSION_MAJOR = 1
VERSION_MINOR = 7
# comment me for exact milestones
#VERSION_MICRO = _devel_${CURRENT_DATE}
VERSION = ${VERSION_MAJOR}.${VERSION_MINOR}${VERSION_MICRO}

SOURCES_DIR = sources
APPLICATION_NAME = logistics-platform
BINARY_NAME = ${BUILD_DIR}/${APPLICATION_NAME}
CC = gcc
CC_FLAGS = -lpthread
CC_STD = -std=c99
OS_FLAGS = -lpthread
OUTBIN = ${BUILD_DIR}/${APPLICATION_NAME}
OUTBIN_DEBUG = ${BUILD_DIR}/${APPLICATION_NAME}_debug
OUTBIN_ALL = 	$(OUTBIN) 	$(OUTBIN_DEBUG)

# needed for SDL applications building only
SDL_INCLUDE_DIR = -I/usr/include/SDL
INCLUDE_DIR = inc
FILES = *.c
DEPS = $^
LD_FLAGS = -lm

GCC_SECURITY_FLAGS = -fstack-protector -pie -fPIE -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -fno-builtin-memset
OPTIMZATION_FLAG = -O3

GCC_SECURITY_FLAGS += -Wextra 
# pb on Mac OS X, but -Wnon-literal-null-conversion is not available on Linux (tested gcc-5.4 only)

CFLAGS = ${CC_FLAGS} ${CC_STD} ${OS_FLAGS}
CFLAGS_DEBUG = -g -DDEBUG

OBJS = ${SOURCES_DIR}/${FILES}


ARCHIVE_EXT = .tar.gz

TO_BE_ZIPPED = \
        inc \
        sources \
        Makefile \

TO_BE_ZIPPED_BINARIES = \
        ${BUILD_DIR}


# ici commence la creation des binaires

all : $(OUTBIN_ALL)

$(OUTBIN) : $(OBJS)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) ${GCC_SECURITY_FLAGS} ${OPTIMIZATION_FLAGS} -o $@ $(DEPS) $(LD_FLAGS)

$(OUTBIN_DEBUG) : $(OBJS)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) ${GCC_SECURITY_FLAGS} $(CFLAGS_DEBUG) -o $(OUTBIN_DEBUG) $(DEPS) $(LD_FLAGS)

clean :
	rm -rf $(OUTBIN) $(OUTBIN_DEBUG) ${EXEC_NAME}_${VERSION} ${EXEC_NAME}_${VERSION}${ARCHIVE_EXT}
	rm -rf $(BUILD_DIR)/dbg*.dSYM
#	rm -rf ${APPLICATION_NAME}_*
	rm -rf ${APPLICATION_NAME}_${VERSION}
	echo Fichiers binaires supprimés.

devel: clean
	mkdir ${APPLICATION_NAME}_${VERSION}
	mkdir -p ${APPLICATION_NAME}_${VERSION}/build
	cp -R ${TO_BE_ZIPPED} ${APPLICATION_NAME}_${VERSION}
	tar cvzf ${APPLICATION_NAME}_${VERSION}${ARCHIVE_EXT} ${APPLICATION_NAME}_${VERSION}

archive : all
	mkdir ${APPLICATION_NAME}_${VERSION}
	cp -R ${TO_BE_ZIPPED} ${APPLICATION_NAME}_${VERSION}
	cp -R ${TO_BE_ZIPPED_BINARIES} ${APPLICATION_NAME}_${VERSION}
	tar cvzf ${APPLICATION_NAME}_${VERSION}${ARCHIVE_EXT} ${APPLICATION_NAME}_${VERSION}

real_clean : clean
	rm -rf ${APPLICATION_NAME}_${VERSION}${ARCHIVE_EXT}


