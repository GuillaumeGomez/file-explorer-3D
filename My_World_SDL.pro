TEMPLATE = app
CONFIG -= console
CONFIG -= qt

INCLUDEPATH += objects/
INCLUDEPATH += SDL2/
INCLUDEPATH += include/

unix {
    QMAKE_CXXFLAGS += -std=c++0x
    #INCLUDEPATH += /usr/local/include/bullet/

    LIBS += -lGL -lGLU -lSDL2 -lSDL2_gfx -lSDL2_image -lSDL2_ttf -lGLEW -lm -lpthread -lassimp \
    #LIBS += -lBulletDynamics -lBulletCollision -lLinearMath
}

win32 {
    INCLUDEPATH += ./assimp/include/
    INCLUDEPATH += Bullet/include/
    CONFIG += c++11
    DEFINES += WIN32
    DEFINES += GLEW_STATIC _USE_MATH_DEFINES
    DEFINES += USE_PHYSICS

    LIBS += -L./SDL2/ \
            -L./assimp/ \
            -L./Bullet/lib/ \
            -L./ \
            -lwinmm \
            -lmingw32 \
            -luser32 \
            -lGdi32 \
            -lComctl32 \
            -lOleAut32 \
            -lOle32 \
            -luuid \
            -lSDL2main \
            -lSDL2 \
            -lSDL2_gfx \
            -lSDL2_ttf \
            -lSDL2_image \
            -lopengl32 \
            -lglu32 \
            -lassimp \
            -lBullet \

    SOURCES +=  glew.c \
}

SOURCES += main.cpp \
    HandleSDL.cpp \
    MyWindow.cpp \
    Camera.cpp \
    myGLWidget.cpp \
    objects/Vector3D.cpp \
    objects/Color.cpp \
    objects/Cube.cpp \
    objects/Cylinder.cpp \
    KeyHandler.cpp \
    Loader/Loader.cpp \
    objects/Plane.cpp \
    objects/Rectangle.cpp \
    objects/Rotation.cpp \
    objects/SkyBox.cpp \
    objects/Sphere.cpp \
    Utils/HandleThread.cpp \
    Utils/HandleMutex.cpp \
    Utils/MyMutexLocker.cpp \
    Utils/MyException.cpp \
    Loader/LoadingMenu.cpp \
    HandleError.cpp \
    objects/Room.cpp \
    Utils/Utils.cpp \
    Utils/Directory.cpp \
    objects/Door.cpp \
    objects/Text.cpp \
    Utils/HandleFile.cpp \
    HandleFpsCount.cpp \
    objects/TextureHandler.cpp \
    objects/Wall.cpp \
    objects/GraphicFile.cpp \
    shaders/Shader.cpp \
    Texture.cpp \
    FrameBuffer.cpp \
    objects/Button.cpp \
    DataHandler.cpp \
    Tetris.cpp \
    objects/Model.cpp \
    objects/Line.cpp \
#    HandlePhysics.cpp \
    shaders/ShaderHandler.cpp \
    objects/HeightMap.cpp \
    objects/Point.cpp \
    Handle_2048.cpp \
    objects/Menu.cpp \
    Utils/PerlinNoise.cpp

HEADERS += \
    HandleSDL.hpp \
    MyWindow.hpp \
    Camera.hpp \
    myGLWidget.hpp \
    objects/Vector3D.hpp \
    objects/Color.hpp \
    objects/Cube.hpp \
    objects/Cylinder.hpp \
    KeyHandler.hpp \
    Loader/Loader.hpp \
    Loader/ObjectFactory.hpp \
    objects/Rectangle.hpp \
    objects/Rotation.hpp \
    objects/SkyBox.hpp \
    objects/Sphere.hpp \
    Utils/HandleThread.hpp \
    Utils/HandleMutex.hpp \
    Utils/MyMutexLocker.hpp \
    Utils/MyException.hpp \
    Loader/LoadingMenu.hpp \
    String_utils.hpp \
    HandleError.hpp \
    objects/Room.hpp \
    Utils/Utils.hpp \
    Utils/Directory.hpp \
    objects/Door.hpp \
    objects/Text.hpp \
    Utils/HandleFile.hpp \
    HandleFpsCount.hpp \
    objects/TextureHandler.hpp \
    objects/Wall.hpp \
    objects/Plane.hpp \
    objects/GraphicFile.hpp \
    shaders/Shader.hpp \
    Texture.hpp \
    FrameBuffer.hpp \
    objects/Button.hpp \
    DataHandler.hpp \
    Tetris.hpp \
    objects/Model.hpp \
    objects/Line.hpp \
#    HandlePhysics.hpp \
    shaders/ShaderHandler.hpp \
    objects/HeightMap.hpp \
    objects/Point.hpp \
    Handle_2048.hpp \
    objects/Menu.hpp \
    Utils/PerlinNoise.hpp
