all:
	CC=/usr/bin/clang xcodebuild install 

install:
	mv /System/Library/Frameworks/GLUT.framework ./GLUT.framework.old
	mv /tmp/GLUT_External.dst/System/Library/Frameworks/GLUT.framework /System/Library/Frameworks/GLUT.framework
