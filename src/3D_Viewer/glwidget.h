#ifndef GLWIDGET_H
#define GLWIDGET_H
#define GL_SILENCE_DEPRECATION

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QVector>
#include <QWheelEvent>

#include "../backend/readobj.h"
#include "mainwindow.h"

class MyOpenGLWidget : public QOpenGLWidget, public QOpenGLFunctions {
  Q_OBJECT

 public:
  QMatrix4x4 modelMatrix;

  int xRot, yRot, zRot;
  ~MyOpenGLWidget();
  MyOpenGLWidget(QWidget *parent = nullptr);

  void loadOBJData(float **vertices, unsigned int *vertexCount,
                   unsigned int **vertexIndices,
                   unsigned int *vertexIndicesCount);

  int projectionMode = 0;
  int pointMode = 0;
  float pointSize = 0.5f;
  float lineSize = 0.5f;
  int lineMode = 0;

  QColor backgroundColor = Qt::black;
  QColor lineColor = Qt::white;
  QColor dotColor = Qt::white;

  float *vertices = nullptr;
  unsigned int vertexCount = 0;
  unsigned int *vertexIndices = nullptr;
  unsigned int vertexIndicesCount = 0;

  void rotation(float angle, int axis);
  void translation(int coordinateIndex, int value);
  void changeModelScale(float scaleFactor);

  void wheelEvent(QWheelEvent *event) override;
  float modelScale = 1.0f;  // Изначальный масштаб
  float getModelScale() const;
  void setModelScale(float scale);
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void setBackgroundColor(const QColor &);
  void setDotColor(const QColor &);
  void setLineColor(const QColor &);
  void setupProjection();

  float x_min;
  float x_max;
  float y_min;
  float y_max;
  float z_min;
  float z_max;
  float cordMax;
  GLfloat z_near;  // Ближняя плоскость отсечения перспективной проекции
  GLfloat z_far;  // Дальняя плоскость отсечения перспективной проекции

  int mvX = 0;
  int mvPrevX = 0;
  int mvY = 0;
  int mvPrevY = 0;
  int mvZ = 0;
  int mvPrevZ = 0;

  float rtX = 0;
  float rtPrevX = 0;
  float rtY = 0;
  float rtPrevY = 0;
  float rtZ = 0;
  float rtPrevZ = 0;

  int sc = 50;
  int scPrev = 0;

  void updateModelMatrix();

 private:
  QOpenGLShaderProgram shaderProgram;
  QOpenGLBuffer vertexBuffer;
  QOpenGLVertexArrayObject vertexArrayObject;
  QVector<float> objVertices;
  QOpenGLBuffer indicesBuffer;
  QVector<unsigned int> indices;

  uint32_t vertexBufferId = 0;
  uint32_t indexBufferId = 0;

  void pointDrawing();
  void lineDrawing();

  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int w, int h) override;
  void paintFigure();
  bool centralProjection;  // Центральная проекция
  float fov;   // Угол обзора центральной проекции
  float left;  // Параметры параллельной проекции
  float right;
  float bottom;
  float top;
  GLfloat rL, gL, bL;
  bool leftMousePressed;
  bool rightMousePressed;
  QPoint lastMousePos;
  float modelRotationX;
  float modelRotationY;
  float modelTranslationX;
  float modelTranslationY;
  float modelTranslationZ;
  float currentModelX = 0.0f;
  float currentModelY = 0.0f;
  float currentModelZ = 0.0f;
};

#endif  // GLWIDGET_H
