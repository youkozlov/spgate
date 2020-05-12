
DEFINES += QMAKE_VARIANT='"\\\"yes\\\""'
DEFINES += PROJECT_NAME='"\\\"spgate\\\""'
DEFINES += PROJECT_VER='"\\\"1.0.0\\\""'
DEFINES += GIT_BUILD_INFO='"\\\"$$system(git diff --quiet || echo dirty-)$$system(git rev-parse --abbrev-ref HEAD)-$$system(git rev-parse --short HEAD)\\\""'
