#include "openglwidget.h"
#include <QOpenGLExtraFunctions>
#include <qmath.h>

#include <QtMath>
#include <QVector>
OpenglWidget::OpenglWidget(QWidget *parent)
    : QOpenGLWidget(parent),ibo(QOpenGLBuffer::IndexBuffer),vboGrid(QOpenGLBuffer::VertexBuffer)
{
    camera = new Camera(QVector3D(0.0f, 0.0f, 4.0f));
    this->setMouseTracking(true);
    lastX = size().width() / 2.0f;
    lastY = size().height() / 2.0f;
    grabKeyboard();

}

void OpenglWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    texture = new QOpenGLTexture(QImage("G:/download/Kurtislara_AppleTexture.jpg").mirrored());
    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    initShaders();
    initGrid(&vboGrid,20,2);
}

void OpenglWidget::resizeGL(int w, int h)
{
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(45.0f, w / float(h), 0.01f, 100.0f);
}

void OpenglWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    QMatrix4x4 viewMatrix;
    viewMatrix.setToIdentity();

    shader.bind();


    pollEvents();
    shader.bind();
    shader.setUniformValue("viewMatrix", camera->GetViewMatrix());
    shader.setUniformValue("modelMatrix", modelMatrix);
    shader.setUniformValue("projectionMatrix", projectionMatrix);

    QMatrix4x4 temp = modelMatrix;
    QQuaternion q = QQuaternion::fromAxisAndAngle(QVector3D(1.0f,0.0,0.0f),90.0f);
    for(int i = 0; i < apples.size(); ++i){
        modelMatrix.translate(apples[i].pos);
        modelMatrix.rotate(q);
          texture->bind();
        apples[i].buffer.bind();
        int offset = 0;
        int vertLoc = shader.attributeLocation("a_Vertex");
        shader.enableAttributeArray(vertLoc);
        shader.setAttributeBuffer(vertLoc,GL_FLOAT, offset, 3, sizeof(PointData));
        offset+=sizeof(QVector3D);
        int texLoc = shader.attributeLocation("a_Texture");
        shader.enableAttributeArray(texLoc);
        shader.setAttributeBuffer(texLoc,GL_FLOAT, offset, 2, sizeof(PointData));
        apples[i].ibo.bind();

        shader.setUniformValue("modelMatrix", modelMatrix);
        glDrawElements(GL_TRIANGLES,apples[i].buffer.size(),GL_UNSIGNED_INT,0);
        apples[i].buffer.release();
        apples[i].ibo.release();
        modelMatrix = temp;
    }

    vboGrid.bind();
    int offset = 0;
    int vertLoc = shader.attributeLocation("a_Vertex");
    gridshader.bind();
    gridshader.enableAttributeArray(vertLoc);
    gridshader.setAttributeBuffer(vertLoc,GL_FLOAT, offset, 3, sizeof(PointData));
    gridshader.setUniformValue("modelMatrix", modelMatrix);
    gridshader.setUniformValue("viewMatrix", camera->GetViewMatrix());
    gridshader.setUniformValue("modelMatrix", modelMatrix);
    gridshader.setUniformValue("projectionMatrix", projectionMatrix);
    vboGrid.bind();
    glDrawArrays(GL_LINES,0,vboGrid.size());
    vboGrid.release();

}

void OpenglWidget::initShaders()
{
    shader.addShaderFromSourceFile(QOpenGLShader::Vertex,"C:\\Users\\ea56\\Documents\\labGraphic6\\sphere.vert");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment,"C:\\Users\\ea56\\Documents\\labGraphic6\\sphere.frag");
    shader.link();

    gridshader.addShaderFromSourceFile(QOpenGLShader::Vertex,"C:\\Users\\ea56\\Documents\\labGraphic6\\grid.vert");
    gridshader.addShaderFromSourceFile(QOpenGLShader::Fragment,"C:\\Users\\ea56\\Documents\\labGraphic6\\grid.frag");
    gridshader.link();
}

void initSphere(QOpenGLBuffer *buff,QOpenGLBuffer *ibo, const float& radius, const int& sectorCount, const int &stackCount)
{

    QVector<PointData> pData;
    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f / radius;

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stackCount; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);

            float s = (float)j / sectorCount;
            float t = (float)i / stackCount;
            pData.push_back(PointData(QVector3D(x,y,z),QVector2D(s,t)));
        }

    }
    QVector<int> indices;
    int k1, k2;
    for(int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            if(i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if(i != (stackCount-1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }


    buff->create();
    buff->bind();
    buff->allocate(pData.constData(),pData.size() * sizeof(PointData));
    buff->release();

    ibo->create();
    ibo->bind();
    ibo->allocate(indices.constData(),indices.size() * sizeof(int));
    ibo->release();
}

void OpenglWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (firstMouse)
    {
        lastX = event->localPos().x();
        lastY = event->localPos().y();
        firstMouse = false;
    }

    float xoffset = event->localPos().x() - lastX;
    float yoffset = lastY - event->localPos().y();

    lastX = event->localPos().x();
    lastY = event->localPos().y();

    if (event->buttons() == Qt::LeftButton)
    {
        camera->ProcessMouseMovement(xoffset, yoffset);
        this->update();
    }
}

void OpenglWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->type() == QKeyEvent::KeyPress)
     {
         int ikey = event->key();
         if(ikey > 1024)
             return;
         keys[ikey] = true;
     }
     this->update();
}

void OpenglWidget::keyReleaseEvent(QKeyEvent *event)
{
    int ikey = event->key();
    if(ikey > 1024)
        return;
    keys[ikey] = false;
    this->update();
}

void OpenglWidget::pollEvents()
{
    if (keys[Qt::Key_W] == true)
    {
        camera->ProcessKeyboard(FORWARD, deltaTime);
    }
    if (keys[Qt::Key_S] == true)
    {
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (keys[Qt::Key_A] == true)
    {
        camera->ProcessKeyboard(LEFT, deltaTime);
    }
    if (keys[Qt::Key_D] == true)
    {
        camera->ProcessKeyboard(RIGHT, deltaTime);

    }
    this->update();
}

void OpenglWidget::addElement(QVector3D pos)
{
    apples.push_back(Apple(pos));
}



void initGrid(QOpenGLBuffer *buff, float width, float sizeofCube)
{
    QVector<PointData> pData;
    int count = width/sizeofCube;
    for(int i = 0; i < count; ++i){
        float stepZ = i * sizeofCube;
        for(int j = 0; j < count; ++j){
            float stepX = j * sizeofCube;
            QVector3D stepx3D{stepX,0.0f,stepZ};
            pData.push_back(QVector3D(QVector3D(-sizeofCube / 2, -sizeofCube / 2, -sizeofCube / 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D(-sizeofCube / 2,  sizeofCube / 2, -sizeofCube / 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D(-sizeofCube / 2,  sizeofCube / 2,  sizeofCube / 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D(-sizeofCube / 2, -sizeofCube / 2,  sizeofCube / 2) +stepx3D));

            pData.push_back(QVector3D(QVector3D( sizeofCube / 2, -sizeofCube / 2, -sizeofCube / 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D( sizeofCube / 2, -sizeofCube / 2,  sizeofCube / 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D( sizeofCube / 2,  sizeofCube / 2,  sizeofCube / 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D( sizeofCube / 2,  sizeofCube / 2, -sizeofCube / 2) +stepx3D));

            pData.push_back(QVector3D(QVector3D(-sizeofCube / 2, -sizeofCube / 2, -sizeofCube / 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D(-sizeofCube / 2, -sizeofCube / 2,  sizeofCube / 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D( sizeofCube / 2, -sizeofCube / 2,  sizeofCube / 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D( sizeofCube / 2, -sizeofCube / 2, -sizeofCube / 2) +stepx3D));

            pData.push_back(QVector3D(QVector3D(-sizeofCube / 2, sizeofCube/ 2, -sizeofCube/ 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D(-sizeofCube / 2, sizeofCube/ 2,  sizeofCube/ 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D( sizeofCube / 2, sizeofCube/ 2,  sizeofCube/ 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D( sizeofCube / 2, sizeofCube/ 2, -sizeofCube/ 2) +stepx3D));

            pData.push_back(QVector3D(QVector3D(-sizeofCube / 2, -sizeofCube / 2, -sizeofCube / 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D( sizeofCube / 2, -sizeofCube / 2, -sizeofCube / 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D( sizeofCube / 2,  sizeofCube / 2, -sizeofCube / 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D(-sizeofCube / 2,  sizeofCube / 2, -sizeofCube / 2) +stepx3D));

            pData.push_back(QVector3D(QVector3D(-sizeofCube / 2, -sizeofCube / 2,  sizeofCube / 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D( sizeofCube / 2, -sizeofCube / 2,  sizeofCube / 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D( sizeofCube / 2,  sizeofCube / 2,  sizeofCube / 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D(-sizeofCube / 2,  sizeofCube / 2,  sizeofCube / 2) +stepx3D));

            pData.push_back(QVector3D(QVector3D( sizeofCube / 2,  -sizeofCube / 2,  sizeofCube / 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D( sizeofCube / 2,  sizeofCube / 2,  sizeofCube / 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D( sizeofCube / 2,  -sizeofCube / 2,  -sizeofCube / 2) +stepx3D));
            pData.push_back(QVector3D(QVector3D( sizeofCube / 2,  sizeofCube / 2, - sizeofCube / 2) +stepx3D));
        }

    }

    buff->create();
    buff->bind();
    buff->allocate(pData.constData(),pData.size() * sizeof(PointData));
    buff->release();
}
