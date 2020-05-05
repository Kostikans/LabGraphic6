#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QQuaternion>
#include <QOpenGLVertexArrayObject>
#include <camera.h>
#include <QOpenGLTexture>

void initSphere(QOpenGLBuffer *buff,QOpenGLBuffer *ibo, const float& radius, const int& sectorCount, const int &stackCount);
void initGrid(QOpenGLBuffer *buff, float width, float sizeofCube);

struct PointData
{
    QVector3D vertex;
    QVector2D texture;
    PointData(QVector3D &&vert,QVector2D &&_texture)
    {
        vertex = std::move(vert);
        texture = std::move(_texture);
    }
    PointData(QVector3D &&vert)
    {
        vertex = std::move(vert);
    }
};
class Apple{
public:
    QOpenGLBuffer buffer;
    QOpenGLBuffer ibo;
    QVector3D pos;
    Apple(QVector3D _pos)
        :buffer(QOpenGLBuffer::VertexBuffer),ibo(QOpenGLBuffer::IndexBuffer)
    {
        pos = _pos * 2.0f;
        initSphere(&buffer,&ibo,1.0f,100,100);
    }
};

class OpenglWidget : public QOpenGLWidget , public QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenglWidget(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void initShaders();

    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    void pollEvents();
public:
    void addElement(QVector3D pos);
private:

    QMatrix4x4 projectionMatrix;
    QQuaternion q;
    float x{0},y{0},z{0};

    QOpenGLShaderProgram shader;
    QOpenGLShaderProgram gridshader;
    bool my = false;
    int lastX;
    int lastY;
    bool firstMouse{true};

    QOpenGLBuffer vboSphere;
    QOpenGLBuffer ibo;
    Camera *camera;
    bool keys[4024] = { false };
    float deltaTime = 1 / 60.0f;
    QOpenGLBuffer vboGrid;
    QVector<QVector2D> data;
    QVector<Apple> apples;
    QOpenGLTexture *texture;
};


#endif // OPENGLWIDGET_H
