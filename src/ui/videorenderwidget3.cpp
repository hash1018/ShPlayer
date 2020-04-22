

#include "videorenderwidget3.h"
#include <qopenglshaderprogram.h>
#include <qopengltexture.h>


//////////////////////////////////////

//중복없이 사각형

/*
float position [] = {

    -1.0f, 1.0f, 0.0f,    // index 0     topLeft
    1.0f, 1.0f, 0.0f,     //  index 1    topRight
    1.0f, -1.0f, 0.0f,    //  index 2    bottomRight
    -1.0f, -1.0f, 0.0f    //  index 3    bottomLeft

};


GLuint elements[] = {

    0, 1, 2,
    2, 3, 0
};

float textureCoordinate[] = {

    0.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f
};
*/

float position [] = {

    -1.0f, 1.0f, 0.0f,    //  index 0     topLeft
    -1.0f, -1.0f, 0.0f,     //  index 1    bottomLeft
    1.0f, -1.0f, 0.0f,    //  index 2    bottomRight
    1.0f, 1.0f, 0.0f    //  index 3    topRight

};


GLuint elements[] = {

    0, 1, 2, //3
    0, 2, 3
};

float textureCoordinate[] = {

    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
};


/////////////////////////////////////////

const GLchar* vertexShaderSource =
        "attribute vec3 positionAttribute;\n"
        "attribute vec2 textureCoordinateAttribute;\n"
        "varying vec2 passTextureCoordinateAttribute;\n"
        "void main(void)\n"
        "{\n"
        "gl_Position = vec4(positionAttribute, 1.0);\n"
        "passTextureCoordinateAttribute = textureCoordinateAttribute;\n"
        "}";

const GLchar* fragmentShaderSource =
        "varying vec2 passTextureCoordinateAttribute;\n"
        "uniform sampler2D Ytex;\n"
        "uniform sampler2D Utex;\n"
        "uniform sampler2D Vtex;\n"
        "void main()\n"
        "{\n"
        "vec3 yuv;\n"
        "vec3 rgb;\n"
        "\n"
        "yuv.x = texture2D(Ytex, passTextureCoordinateAttribute).r;\n"
        "yuv.y = texture2D(Utex, passTextureCoordinateAttribute).r - 0.5;\n"
        "yuv.z = texture2D(Vtex, passTextureCoordinateAttribute).r - 0.5;\n"
        "rgb = mat3(1,      1,          1,          \n"
        "           0,  -0.39465,   2.03211,        \n"
        "         1.13983, -0.58060,    0           \n"
        "                                ) *  yuv;  \n"
        "\n"
        "gl_FragColor = vec4(rgb, 1);\n"
        "}";

VideoRenderWidget::VideoRenderWidget(QWidget *parent)
    :QOpenGLWidget(parent), initializedTexture(false), videoWidth(0), videoHeight(0),
      buffer(nullptr),vao(this) {



}

VideoRenderWidget::~VideoRenderWidget(){

    if(this->buffer!=nullptr)
        delete [] this->buffer;

    makeCurrent();


}


void VideoRenderWidget::initializeGL() {


    this->initializeOpenGLFunctions();

    //VertexBufferObject 에 데이터 복사   (gpu에서 바로 사용하기 위해)
    glGenBuffers(1, &this->positionVbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->positionVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_DYNAMIC_DRAW);


    glGenBuffers(1,&this->textureVbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->textureVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinate),textureCoordinate, GL_DYNAMIC_DRAW);





    /////////////// 쉐이더



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


    ////// 프로그램
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_DYNAMIC_DRAW);
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




    //////////////////////////////////////////////

    GLint textureCoordinateAttribute = glGetAttribLocation(this->sharderProgram, "textureCoordinateAttribute");
    if(textureCoordinateAttribute == -1){

        qDebug() <<"rrr";
    }

    glBindBuffer(GL_ARRAY_BUFFER, this->textureVbo);
    glVertexAttribPointer(textureCoordinateAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(textureCoordinateAttribute);

    vao.release();




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

    glUseProgram(this->sharderProgram);
    vao.bind();

    /*
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, this->width(), this->height(),0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
*/



    int uIndex = this->videoWidth * this->videoHeight;
    int vIndex = uIndex + (uIndex / 4);

    int i = glGetUniformLocation(this->sharderProgram,"Utex");
    glUniform1i(i,1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->uTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->videoWidth / 2, this->videoHeight / 2 ,
                    GL_LUMINANCE, GL_UNSIGNED_BYTE, &this->buffer[uIndex]);



    i = glGetUniformLocation(this->sharderProgram,"Vtex");
    glUniform1i(i,2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, this->vTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->videoWidth / 2, this->videoHeight / 2,
                    GL_LUMINANCE, GL_UNSIGNED_BYTE, &this->buffer[vIndex]);


    i = glGetUniformLocation(this->sharderProgram,"Ytex");
    glUniform1i(i,0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->yTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->videoWidth, this->videoHeight,
                    GL_LUMINANCE, GL_UNSIGNED_BYTE, this->buffer);


    //중복없이 사각형
    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);


    vao.release();
    glUseProgram(0);


}


void VideoRenderWidget::initializeTexture(int width, int height) {


    //UTexture
    glGenTextures(2, &this->uTexture);
    glBindTexture(GL_TEXTURE_2D, this->uTexture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glEnable(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width/2, height/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLint Utex = glGetUniformLocation(this->sharderProgram, "Utex");

    if(Utex == -1){

        qDebug() << "Utttt";
    }

    glUniform1i(Utex,1);




    //VTexture
    glGenTextures(3, &this->vTexture);
    glBindTexture(GL_TEXTURE_2D, this->vTexture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glEnable(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width/2, height/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLint Vtex = glGetUniformLocation(this->sharderProgram, "Vtex");

    if(Vtex == -1){

        qDebug() << "Vtttt";
    }

    glUniform1i(Vtex,2);




    //YTexture
    glGenTextures(1, &this->yTexture);
    glBindTexture(GL_TEXTURE_2D, this->yTexture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glEnable(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);

    GLint Ytex = glGetUniformLocation(this->sharderProgram, "Ytex");

    if(Ytex == -1){

        qDebug() << "Ytttt";
    }

    glUniform1i(Ytex,0);



    this->initializedTexture = true;
}

void VideoRenderWidget::update(const uint8_t *buffer, int size){


    if(this->buffer!=nullptr)
        delete [] this->buffer;

    this->buffer= new uint8_t[size];

    memcpy(this->buffer,buffer,size);

    QOpenGLWidget::update();

}
