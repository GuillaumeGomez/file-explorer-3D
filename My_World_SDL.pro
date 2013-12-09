TEMPLATE = app
CONFIG -= console
CONFIG -= qt

INCLUDEPATH += objects/
CONFIG += c++11
DEFINES += WIN32

unix {
    INCLUDEPATH += SFML/

    #LIBS += -lGLU -lSDL -lSDL_gfx -lSDL_image -lSDL_ttf -lGLEW -lm -lpthread \
    LIBS += -lGL -lGLU -lGLEW -lsfml-graphics2 -lsfml-window2 -lsfml-system2 -lm -lpthread

    SOURCES += HandleSFML.cpp \

    HEADERS += HandleSFML.hpp \
}

win32 {
    INCLUDEPATH += SDL2/
    INCLUDEPATH += include/
    SOURCES +=  glew.c \
                HandleSDL.cpp \

    DEFINES += GLEW_STATIC _USE_MATH_DEFINES

    LIBS += -L./SDL2/ \
            #-L./SDL/ \
            -lwinmm \
            -lmingw32 \
            -luser32 \
            -lGdi32 \
            -lComctl32 \
            -lOleAut32 \
            -lOle32 \
            -luuid \
            #-lSDLmain \
            -lSDL2main \
            #-lSDL \
            -lSDL2 \
            #-lSDL_gfx \
            -lSDL2_gfx \
            #-lSDL_ttf \
            -lSDL2_ttf \
            #-lSDL_image \
            -lSDL2_image \
            -lopengl32 \
            -lglu32 \

    HEADERS += HandleSDL.hpp \
}

SOURCES += main.cpp \
    MyWindow.cpp \
    Camera.cpp \
    myGLWidget.cpp \
    objects/Vector3D.cpp \
    objects/Color.cpp \
    objects/Cube.cpp \
    objects/Cylinder.cpp \
    KeyHandler.cpp \
    Loader/Loader.cpp \
    #objects/MyModel.cpp \
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
    GraphicHandler.cpp \
    objects/GraphicFile.cpp \
    objects/VertexBuffer.cpp \
    shaders/Shader.cpp \
    objects/Box.cpp \
    VertexContainer.cpp \
    Texture.cpp \
    FrameBuffer.cpp \
    objects/Button.cpp \
    DataHandler.cpp

HEADERS += \
    MyWindow.hpp \
    Camera.hpp \
    myGLWidget.hpp \
    objects/Vector3D.hpp \
    objects/Color.hpp \
    objects/Cube.hpp \
    objects/Cylinder.hpp \
    KeyHandler.hpp \
    Loader/Loader.hpp \
    #objects/MyModel.hpp \
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
    test.hpp \
    objects/Room.hpp \
    Utils/Utils.hpp \
    Utils/Directory.hpp \
    objects/Door.hpp \
    objects/Text.hpp \
    Utils/HandleFile.hpp \
    HandleFpsCount.hpp \
    objects/TextureHandler.hpp \
    objects/Wall.hpp \
    GraphicHandler.hpp \
    objects/Plane.hpp \
    objects/GraphicFile.hpp \
    objects/VertexBuffer.hpp \
    shaders/Shader.hpp \
    objects/Box.hpp \
    VertexContainer.hpp \
    Texture.hpp \
    FrameBuffer.hpp \
    objects/Button.hpp \
    DataHandler.hpp
