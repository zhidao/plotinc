PROJNAME=plotinc
PREFIX=${HOME}/usr
INCLUDE_DIR=${PREFIX}/include
LIB_DIR=${PREFIX}/lib

TARGET=lib${PROJNAME}.so

all:
	cd src; make; cd -
clean:
	cd src; make clean; cd -
	cd example; make clean; cd -
install:
	cp src/${TARGET} ${LIB_DIR}/
	mkdir -p ${INCLUDE_DIR}/${PROJNAME}
	cp -a include/${PROJNAME} ${INCLUDE_DIR}/
uninstall:
	rm -rf ${INCLUDE_DIR}/${PROJNAME}
	rm -f ${LIB_DIR}/${TARGET}
