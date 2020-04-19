#ifndef VIDEORENDERWIDGET_H
#define VIDEORENDERWIDGET_H

#include <qopenglwidget.h>
#include <QOpenGLFunctions>
#include <qopenglbuffer.h>

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
    QOpenGLShader *vertexShader;
    QOpenGLShader *fragmentShader;
    QOpenGLShaderProgram *shaderProgram;
    QOpenGLBuffer *vbo;
    QOpenGLTexture *texture;


public:
    VideoRenderWidget(QWidget *parent = nullptr);
    ~VideoRenderWidget();


public:
    void initializeTexture(int width,int height);
    void setVideoSize(int videoWidth, int videoHeight) { this->videoWidth=videoWidth; this->videoHeight=videoHeight;}
    void update(const uint8_t *buffer, int size);

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
};

#endif // VIDEORENDERWIDGET_H
