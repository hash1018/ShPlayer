

#include "videorenderwidget3.h"
#include <qopenglshaderprogram.h>
#include <qopengltexture.h>

/*
float position[] = {

    0.0f,  0.5f, 0.0f,    // 위 중앙
    0.5f, -0.5f, 0.0f,    // 오른쪽 아래
    -0.5f,-0.5f, 0.0f     // 왼쪽 아래

};

float color[] = {

    1.0f, 0.0f, 0.0f,   //red
    0.0f, 1.0f, 0.0f,   //green
    0.0f, 0.0f, 1.0f    // blue
};
*/

/*

  //사각형 하지만,,, 중복 있음
float position[]={

    -0.5f, 0.5f, 0.0f,
    0.5f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f

};

float color[] ={
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f


};

*/


//////////////////////////////////////

//중복없이 사각형

float position [] = {

    -0.5f, 0.5f, 0.0f,
    0.5f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f

};

float color [] = {

    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f

};

GLuint elements[] = {

    0, 1, 2,
    2, 3, 0
};


/////////////////////////////////////////

const GLchar* vertexShaderSource =
        "attribute vec3 positionAttribute;\n"
        "attribute vec3 colorAttribute;\n"
        "varying vec3 passColorAttribute;\n"
        "void main(void)\n"
        "{\n"
        "gl_Position = vec4(positionAttribute, 1.0);\n"
        "passColorAttribute = colorAttribute;\n"
        "}";

const GLchar* fragmentShaderSource =
        "varying vec3 passColorAttribute;\n"
        "void main()\n"
        "{\n"
        "gl_FragColor = vec4(passColorAttribute, 1.0);\n"
        "}";

VideoRenderWidget::VideoRenderWidget(QWidget *parent)
    :QOpenGLWidget(parent), initializedTexture(false), videoWidth(0), videoHeight(0),
      buffer(nullptr) {



}

VideoRenderWidget::~VideoRenderWidget(){

    if(this->buffer!=nullptr)
        delete [] this->buffer;


    glUseProgram(0);
    this->vao.release();
    this->vao.destroy();

}


void VideoRenderWidget::initializeGL() {


    this->initializeOpenGLFunctions();


    glGenBuffers(1, &this->positionVbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->positionVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);

    glGenBuffers(1, &this->colorVbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->colorVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);



    this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(this->vertexShader);

    GLint result;
    GLchar errorLog[512];

    glGetShaderiv(this->vertexShader, GL_COMPILE_STATUS, &result);
    if(!result){

        glGetShaderInfoLog(this->vertexShader,512,NULL,errorLog);
        qDebug() <<"error compile " << errorLog;
    }


    ///////////////////////////////////////////////////////////////////////

    this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(this->fragmentShader);


    glGetShaderiv(this->fragmentShader, GL_COMPILE_STATUS, &result);
    if(!result){

        glGetShaderInfoLog(this->fragmentShader, 512, NULL, errorLog);
        qDebug() <<"error compile " << errorLog;
    }

    ////////////////////////////////////////////////////////////////////////


    this->sharderProgram = glCreateProgram();

    glAttachShader(this->sharderProgram, this->vertexShader);
    glAttachShader(this->sharderProgram, this->fragmentShader);

    glLinkProgram(this->sharderProgram);

    glDeleteShader(this->vertexShader);
    glDeleteShader(this->fragmentShader);

    glGetProgramiv(this->sharderProgram, GL_LINK_STATUS, &result);
    if(!result){

        glGetProgramInfoLog(this->sharderProgram, 512, NULL, errorLog);
        qDebug() <<"error link " << errorLog;
    }

    ////////////////////////////////////////////////////////////////////////


    vao.create();
    vao.bind();


    ///////////////////////////
    // 중복없이 사각형
    ////////////////////////////////
    glGenBuffers(1, &this->elementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    ////////////////////////////


    //--- VertexShader 속성과 VBO 연결 ---


    //1. shader 에 해당 속성변수를 바인딩된 인덱스를 리턴받아서 GLint positionAttribute에 저장
    GLint positionAttribute = glGetAttribLocation(this->sharderProgram, "positionAttribute");
    if(positionAttribute == -1){

        qDebug() <<" dasd";
    }


    //2. vertexBufferObject 바인딩
    glBindBuffer(GL_ARRAY_BUFFER, this->positionVbo);

    //3. 1번에서 바인딩된 인덱스가 저장된 positionAttribute 변수가 VertexBufferObject 에서 어떻게 데이터를 가져오는지 지정.
    //   positionAttribute
    //   positionVertex 속성은 vec3이니까 3
    //
    //   vertexBufferObject에서 연속적으로 저장되어 있는 Vertex 속성간의 간격. 여기선 position 속성만 배열에 있으므로 간격은 0
    //   position 데이터가 시작되는 위치
    glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,0 ,0);

    //4. 연결 하기위해서 활성화.
    glEnableVertexAttribArray(positionAttribute);


    GLint colorAttribute = glGetAttribLocation(this->sharderProgram, "colorAttribute");
    if(colorAttribute == -1){

        qDebug() << "dddkkkkkkkkkkkkkk";
    }

    glBindBuffer(GL_ARRAY_BUFFER, this->colorVbo);
    glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorAttribute);


    vao.release();


    glUseProgram(this->sharderProgram);
    vao.bind();

}

void VideoRenderWidget::paintGL() {


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

/*
    if(this->buffer==nullptr)
        return;
*/
    if(this->videoWidth == 0 || this->videoHeight == 0 )
        return;

    if(this->initializedTexture == false){

        this->initializeTexture(this->videoWidth, this->videoHeight);
    }





    //삼각형
    //glDrawArrays(GL_TRIANGLES, 0, 3);


    //사각형 중복
    //glDrawArrays(GL_TRIANGLES,0,6);


    //중복없이 사각형
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);


}


void VideoRenderWidget::initializeTexture(int width, int height) {


}

void VideoRenderWidget::update(const uint8_t *buffer, int size){

/*
    if(this->buffer!=nullptr)
        delete [] this->buffer;

    this->buffer= new uint8_t[size];

    memcpy(this->buffer,buffer,size);
*/
    QOpenGLWidget::update();

}
