all:
	CC=/usr/bin/clang xcodebuild install 

install:
	ditto /tmp/GLUT_External.dst/System/Library/Frameworks/GLUT.framework /System/Library/Frameworks/GLUT.framework
