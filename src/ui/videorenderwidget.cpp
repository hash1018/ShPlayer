

#include "videorenderwidget.h"
#include <qopenglshaderprogram.h>
#include <qopengltexture.h>



GLuint          m_textureIds[3];
const char g_indices[] = { 0, 3, 2, 0, 2, 1 };
const char g_vertextShader[] = { "attribute vec4 aPosition;\n"
                                 "attribute vec2 aTextureCoord;\n"
                                 "varying vec2 vTextureCoord;\n"
                                 "void main() {\n"
                                 "  gl_Position = aPosition;\n"
                                 "  vTextureCoord = aTextureCoord;\n"
                                 "}\n" };
const char g_fragmentShader[] = {
    "uniform sampler2D Ytex,Utex,Vtex;\n"
    "varying vec2 vTextureCoord;\n"
    "void main(void) {\n"
    "vec3 yuv;\n"
    "vec3 rgb;\n"
    "yuv.x = texture2D(Ytex, vTextureCoord).r;\n"
    "yuv.y = texture2D(Utex, vTextureCoord).r - 0.5;\n"
    "yuv.z = texture2D(Vtex, vTextureCoord).r - 0.5;\n"
    "rgb = mat3(1,1,1,\n"
    "0,-0.39465,2.03211,\n"
    "1.13983, -0.58060,  0) * yuv;\n"
    "gl_FragColor = vec4(rgb, 1);\n"
    "}\n" };


GLfloat m_verticesA[20] = { 1, 1, 0, 1, 0, -1, 1, 0, 0, 0, -1, -1, 0, 0,
                            1, 1, -1, 0, 1, 1, };




VideoRenderWidget::VideoRenderWidget(QWidget *parent)
    :QOpenGLWidget(parent), initializedTexture(false), videoWidth(0), videoHeight(0),
      buffer(nullptr) {


}

VideoRenderWidget::~VideoRenderWidget(){

    if(this->buffer!=nullptr)
        delete [] this->buffer;

}


void VideoRenderWidget::initializeGL() {



    this->initializeOpenGLFunctions();

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glEnable(GL_TEXTURE_2D);


    // glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    // glClearDepth(1.0f);
    // glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


    this->vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    this->vertexShader->compileSourceCode(g_vertextShader);
    this->fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    this->fragmentShader->compileSourceCode(g_fragmentShader);
    this->shaderProgram = new QOpenGLShaderProgram;
    this->shaderProgram->addShader(this->vertexShader);
    this->shaderProgram->addShader(this->fragmentShader);
    this->shaderProgram->link();

    int positionHandle=this->shaderProgram->attributeLocation("aPosition");
    int textureHandle=this->shaderProgram->attributeLocation("aTextureCoord");

    glVertexAttribPointer(positionHandle, 3, GL_FLOAT, false,
                          5 * sizeof(GLfloat), m_verticesA);

    glEnableVertexAttribArray(positionHandle);

    glVertexAttribPointer(textureHandle, 2, GL_FLOAT, false,
                          5 * sizeof(GLfloat), &m_verticesA[3]);

    glEnableVertexAttribArray(textureHandle);

    int i=this->shaderProgram->uniformLocation("Ytex");
    glUniform1i(i, 0);
    i=this->shaderProgram->uniformLocation("Utex");
    glUniform1i(i, 1);
    i=this->shaderProgram->uniformLocation("Vtex");
    glUniform1i(i, 2);


    ///////////////////////////////////////////////////////////////////////////


}

void VideoRenderWidget::paintGL() {


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    if(this->buffer==nullptr)
        return;

    if(this->videoWidth == 0 || this->videoHeight == 0 )
        return;

    if(this->initializedTexture == false){

        this->initializeTexture(this->videoWidth, this->videoHeight);
    }


    this->shaderProgram->bind();
    int width=this->videoWidth;
    int height=this->videoHeight;

    int _idxU = width * height;
    int _idxV = _idxU + (_idxU / 4);



    //  UUUUUUUUU
    int i=this->shaderProgram->uniformLocation("Utex");
    glUniform1i(i, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_textureIds[1]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE,
                    GL_UNSIGNED_BYTE, &buffer[_idxU]);





    //   VVVVVVVVVVVV
    i=this->shaderProgram->uniformLocation("Vtex");
    glUniform1i(i, 2);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_textureIds[2]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE,
                    GL_UNSIGNED_BYTE, &buffer[_idxV]);







    //    YYYYYYYYYYYY
    i=this->shaderProgram->uniformLocation("Ytex");
    glUniform1i(i, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE,
                    GL_UNSIGNED_BYTE, buffer);







    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, g_indices);
    this->shaderProgram->release();

}


void VideoRenderWidget::initializeTexture(int width, int height) {


    // UUU
    glActiveTexture(GL_TEXTURE1);
    glGenTextures(2,&m_textureIds[1]);
    glBindTexture(GL_TEXTURE_2D, m_textureIds[1]);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // This is necessary for non-power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glEnable(GL_TEXTURE_2D);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_LUMINANCE,
                 width / 2,
                 height / 2,
                 0,
                 GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,
                 NULL);




    // VVV
    glActiveTexture(GL_TEXTURE2);
    glGenTextures(3,&m_textureIds[2]);
    glBindTexture(GL_TEXTURE_2D, m_textureIds[2]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // This is necessary for non-power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glEnable(GL_TEXTURE_2D);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_LUMINANCE,
                 width / 2,
                 height / 2,
                 0,
                 GL_LUMINANCE,
                 GL_UNSIGNED_BYTE,
                 NULL);






    // YYY
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1,&m_textureIds[0]);
    glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // This is necessary for non-power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glEnable(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE, width, height,
                     0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);


    this->initializedTexture = true;

}
