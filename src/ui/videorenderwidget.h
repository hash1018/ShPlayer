#ifndef VIDEORENDERWIDGET_H
#define VIDEORENDERWIDGET_H

#include <qopenglwidget.h>
#include <QOpenGLFunctions>
#include <qopenglbuffer.h>
#include <QOpenGLVertexArrayObject>

class QOpenGLTexture;
class QOpenGLShader;
class QOpenGLShaderProgram;

class VideoRenderWidget : public QOpenGLWidget , protected QOpenGLFunctions {

private:
    bool initializedTexture;
    int videoWidth;
    int videoHeight;

    uint8_t *buffer;

private:

    GLuint positionVbo;
    GLuint colorVbo;
    QOpenGLVertexArrayObject vao;  //vertex array object.

    GLuint vertexShader;
    GLuint fragmentShader;

    GLuint sharderProgram;


    GLuint elementBufferObject;


    GLuint textureVbo;


    GLuint yTexture;
    GLuint uTexture;
    GLuint vTexture;

public:
    VideoRenderWidget(QWidget *parent = nullptr);
    ~VideoRenderWidget() override;


public:
    void initializeTexture(int width,int height);
    void setVideoSize(int videoWidth, int videoHeight) { this->videoWidth=videoWidth; this->videoHeight=videoHeight;}
    void update(const uint8_t *buffer, int size);

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
};

#endif // VIDEORENDERWIDGET_H
