#include "glwidget.h"

#include <cmath>
#include <iostream>
#include <typeinfo>

#include "mainwindow.h"

#define GlCall(call)                  \
  while (glGetError() != GL_NO_ERROR) \
    ;                                 \
  call;                               \
  checkGlError(#call, __FILE__, __LINE__)

void checkGlError(const char *call, const char *file, int line) {
  uint32_t err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    qDebug() << "GlError(" << err << ") \"" << call << "\" in \"" << file
             << "\" at " << line;
  }
}

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent) {
  rL = 1;
  gL = 1;
  bL = 1;
  pointMode = 0;
  lineMode = 0;
  pointSize = 3;
  lineSize = 1;
  xRot = 0;
  yRot = 0;
  zRot = 0;
  z_near = 0.1f;  // Установите значение ближней плоскости отсечения
  z_far = 100.0f;  // Установите значение дальней плоскости отсечения
  projectionMode = 0;
  modelScale = 1.0f;
  leftMousePressed = false;
  rightMousePressed = false;
  setMouseTracking(true);  // Включить отслеживание движения мыши
}

MyOpenGLWidget::~MyOpenGLWidget() {
  if (vertexBufferId != GL_ZERO) {
    glDeleteBuffers(1, &vertexBufferId);
  }
  if (indexBufferId != GL_ZERO) {
    glDeleteBuffers(1, &indexBufferId);
  }
}

void MyOpenGLWidget::loadOBJData(float **vertices, unsigned int *vertexCount,
                                 unsigned int **vertexIndices,
                                 unsigned int *vertexIndicesCount) {
  // Выделить память и скопировать вершины
  makeCurrent();

  search_for_extremes(vertices, vertexCount, 0, &x_min, &x_max);
  search_for_extremes(vertices, vertexCount, 1, &y_min, &y_max);
  search_for_extremes(vertices, vertexCount, 2, &z_min, &z_max);

  searchExtreme(vertices, vertexCount, &cordMax);

  objVertices.resize(*vertexCount);
  std::copy(*vertices, *vertices + (*vertexCount), objVertices.begin());
  indices.resize(*vertexIndicesCount);
  std::copy(*vertexIndices, *vertexIndices + (*vertexIndicesCount),
            indices.begin());

  if (vertexBufferId == 0) {
    GlCall(glGenBuffers(1, &vertexBufferId));
  }
  GlCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId));
  GlCall(glBufferData(GL_ARRAY_BUFFER, objVertices.size() * sizeof(float),
                      objVertices.data(), GL_STATIC_DRAW));
  if (indexBufferId == 0) {
    GlCall(glGenBuffers(1, &indexBufferId));
  }
  GlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId));
  GlCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                      indices.size() * sizeof(unsigned int), indices.data(),
                      GL_STATIC_DRAW));
  doneCurrent();
}

void MyOpenGLWidget::initializeGL() {
  makeCurrent();

  // Инициализация OpenGL, настройка параметров рендеринга и буферов
  xRot = 0;
  yRot = 0;
  zRot = 0;
  initializeOpenGLFunctions();
  glEnable(GL_DEPTH_TEST);

  doneCurrent();
}

void MyOpenGLWidget::paintGL() {
  makeCurrent();
  glEnable(GL_DEPTH_TEST);

  glClearColor(backgroundColor.redF(), backgroundColor.greenF(),
               backgroundColor.blueF(), 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  setupProjection();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // glMultMatrixf(modelMatrix.constData());

  glRotatef(xRot, 1.0, 0.0, 0.0);
  glRotatef(yRot, 0.0, 1.0, 0.0);
  glRotatef(zRot, 0.0, 0.0, 1.0);

  paintFigure();

  doneCurrent();
}

void MyOpenGLWidget::setupProjection() {
  if (cordMax < 2) {
    cordMax = 5;
  }
  //    qDebug() << "max: " << cordMax;
  z_near = 0.001;
  z_far = cordMax * 7;

  if (projectionMode == 0) {
    glOrtho(-cordMax, cordMax, -cordMax, cordMax, z_near, z_far);
  } else if (projectionMode == 1) {
    GLdouble fov = 90;  // Поле зрения в градусах по оси y
    GLdouble heapH = tan(fov / 360 * M_PI) * z_near;
    glFrustum(-heapH, heapH, -heapH, heapH, z_near, z_far);
  }
  glTranslatef(-(x_max + x_min) / 2.0, -(y_max + y_min) / 2.0, -cordMax * 1.2);
}

float MyOpenGLWidget::getModelScale() const { return modelScale; }

void MyOpenGLWidget::setModelScale(float scale) { modelScale = scale; }

void MyOpenGLWidget::wheelEvent(QWheelEvent *event) {
  int delta = event->angleDelta().y();
  modelScale += 0.01f * delta / 120.0f;

  // Определите максимальный и минимальный масштаб
  float max_scale = 10.0f;
  float min_scale = 0.02f;

  // Проверьте, чтобы масштаб оставался внутри диапазона
  modelScale = qMax(min_scale, qMin(max_scale, modelScale));

  update();
}

void MyOpenGLWidget::paintFigure() {
  makeCurrent();
  // Привязать буфер вершин
  // vertexBuffer.bind();
  glVertexPointer(3, GL_FLOAT, 0, nullptr);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_INDEX_ARRAY);

  pointDrawing();
  lineDrawing();

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_INDEX_ARRAY);

  doneCurrent();
}

void MyOpenGLWidget::pointDrawing() {
  if (pointMode != 0) {
    //         qDebug() << "Point drawing: Vertex count = " <<
    //         objVertices.size() / 3;
    glColor3f(dotColor.redF(), dotColor.greenF(), dotColor.blueF());
    glPointSize(pointSize);
    if (pointMode == 1) {  // круг
      glEnable(GL_POINT_SMOOTH);
    }
    glDrawArrays(GL_POINTS, 0, objVertices.size() / 3);
    if (pointMode == 1) {
      glDisable(GL_POINT_SMOOTH);
    }
  }
}

void MyOpenGLWidget::lineDrawing() {
  //    qDebug() << "Line drawing: Index count = " << indices.size();
  glColor3f(lineColor.redF(), lineColor.greenF(), lineColor.blueF());
  glLineWidth(lineSize);
  if (lineMode == 1) {  // пунктирная
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0x00F0);
    //        qDebug() << "DBG:: GL_LINE_STIPPLE is enable";
  }
  GlCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId));
  GlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId));
  GlCall(glEnableVertexAttribArray(0));
  GlCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                               reinterpret_cast<void *>(0)));
  glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, vertexIndices);
  if (lineMode == 0) {
    glDisable(GL_LINE_STIPPLE);
    //        qDebug() << "DBG:: GL_LINE_STIPPLE is disable";
  }
}

void MyOpenGLWidget::resizeGL(int w, int h) { glViewport(0, 0, w, h); }

void MyOpenGLWidget::rotation(float angle, int axis) {
  unsigned int qVertexCount = objVertices.size();
  float *verticesArray = new float[qVertexCount];
  std::copy(objVertices.begin(), objVertices.end(), verticesArray);

  angle = angle_normalize(angle);

  if (axis == 0) {
    if (angle != rtX) {
      rtX = angle;
      rotate(&verticesArray, &qVertexCount, axis, rtX, rtPrevX);
      rtPrevX = rtX;
    }
  } else if (axis == 1) {
    if (angle != rtY) {
      rtY = angle;
      rotate(&verticesArray, &qVertexCount, axis, rtY, rtPrevY);
      rtPrevY = rtY;
    }
  } else if (axis == 2) {
    if (angle != rtZ) {
      rtZ = angle;
      rotate(&verticesArray, &qVertexCount, axis, rtZ, rtPrevZ);
      rtPrevZ = rtZ;
    }
  }
  // Копируем измененные данные обратно в objVertices
  for (unsigned int i = 0; i < qVertexCount; ++i) {
    objVertices[i] = verticesArray[i];
  }

  delete[] verticesArray;

  makeCurrent();
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  glBufferSubData(GL_ARRAY_BUFFER, 0, objVertices.size() * sizeof(float),
                  objVertices.data());
  doneCurrent();
  update();  // Запуск перерисовки
}

void MyOpenGLWidget::translation(int coordinateIndex, int value) {
  unsigned int qVertexCount = objVertices.size();
  float *verticesArray = new float[qVertexCount];
  std::copy(objVertices.begin(), objVertices.end(), verticesArray);

  if (coordinateIndex == 0) {
    if (value != mvX) {
      mvX = value;
      translate(&verticesArray, &qVertexCount, coordinateIndex, mvX, mvPrevX);
      mvPrevX = mvX;
    }
  } else if (coordinateIndex == 1) {
    if (value != mvY) {
      mvY = value;
      translate(&verticesArray, &qVertexCount, coordinateIndex, mvY, mvPrevY);
      mvPrevY = mvY;
    }
  } else if (coordinateIndex == 2) {
    if (value != mvZ) {
      mvZ = value;
      translate(&verticesArray, &qVertexCount, coordinateIndex, mvZ, mvPrevZ);
      mvPrevZ = mvZ;
    }
  }

  // Копируем измененные данные обратно в objVertices
  for (unsigned int i = 0; i < qVertexCount; ++i) {
    objVertices[i] = verticesArray[i];
  }

  delete[] verticesArray;

  makeCurrent();
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  glBufferSubData(GL_ARRAY_BUFFER, 0, objVertices.size() * sizeof(float),
                  objVertices.data());
  doneCurrent();
  update();  // Запуск перерисовки
}

void MyOpenGLWidget::changeModelScale(float scaleFactor) {
  unsigned int qVertexCount = objVertices.size();
  float *verticesArray = new float[qVertexCount];
  std::copy(objVertices.begin(), objVertices.end(), verticesArray);

  float scPrev = scaleFactor;
  scale(&verticesArray, &qVertexCount, scaleFactor, scPrev);
  // Копируем измененные данные обратно в objVertices
  for (unsigned int i = 0; i < qVertexCount; ++i) {
    objVertices[i] = verticesArray[i];
  }

  delete[] verticesArray;

  makeCurrent();
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  glBufferSubData(GL_ARRAY_BUFFER, 0, objVertices.size() * sizeof(float),
                  objVertices.data());
  doneCurrent();
  update();
}

void MyOpenGLWidget::setBackgroundColor(const QColor &color) {
  backgroundColor = color;
  update();
}

void MyOpenGLWidget::setDotColor(const QColor &color) {
  dotColor = color;
  update();
}

void MyOpenGLWidget::setLineColor(const QColor &color) {
  lineColor = color;
  update();
}

void MyOpenGLWidget::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    leftMousePressed = true;
    lastMousePos = event->pos();
  } else if (event->button() == Qt::RightButton) {
    rightMousePressed = true;
    lastMousePos = event->pos();
  }
}

void MyOpenGLWidget::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    leftMousePressed = false;
  } else if (event->button() == Qt::RightButton) {
    rightMousePressed = false;
  }
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
  if (leftMousePressed) {
    // Выполните вращение модели по X и Y в зависимости от изменения положения
    // мыши
    int deltaX = event->position().x() - lastMousePos.x();
    int deltaY = event->position().y() - lastMousePos.y();
    xRot += deltaY;
    yRot += deltaX;
    lastMousePos = event->pos();
    update();  // Запросите перерисовку
  }
}
