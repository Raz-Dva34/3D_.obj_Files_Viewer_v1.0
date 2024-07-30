#include "mainwindow.h"

#include <QAction>
#include <QMovie>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QPainter>
#include <QTemporaryFile>
#include <QTimer>
#include <thread>

#include "../backend/readobj.c"
#include "../backend/readobj.h"
#include "./ui_mainwindow.h"
#include "glwidget.h"
#include "libgif/gif.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  setWindowTitle("3DViewer_v1.0");

  previousSettings();

  connect(ui->h_v_x, &QSlider::valueChanged, ui->v_x, &QSpinBox::setValue);
  connect(ui->v_x, QOverload<int>::of(&QSpinBox::valueChanged), ui->h_v_x,
          &QSlider::setValue);
  connect(ui->h_v_y, &QSlider::valueChanged, ui->v_y, &QSpinBox::setValue);
  connect(ui->v_y, QOverload<int>::of(&QSpinBox::valueChanged), ui->h_v_y,
          &QSlider::setValue);
  connect(ui->h_v_z, &QSlider::valueChanged, ui->v_z, &QSpinBox::setValue);
  connect(ui->v_z, QOverload<int>::of(&QSpinBox::valueChanged), ui->h_v_z,
          &QSlider::setValue);

  connect(ui->h_line_scale, &QSlider::valueChanged, ui->line_scale,
          &QSpinBox::setValue);
  connect(ui->line_scale, QOverload<int>::of(&QSpinBox::valueChanged),
          ui->h_line_scale, &QSlider::setValue);
  connect(ui->h_peaks_scale, &QSlider::valueChanged, ui->peaks_scale,
          &QSpinBox::setValue);
  connect(ui->peaks_scale, QOverload<int>::of(&QSpinBox::valueChanged),
          ui->h_peaks_scale, &QSlider::setValue);
  connect(ui->m_x, QOverload<int>::of(&QSpinBox::valueChanged), this,
          [this](int value) { moveModel(value, "m_x"); });
  connect(ui->m_y, QOverload<int>::of(&QSpinBox::valueChanged), this,
          [this](int value) { moveModel(value, "m_y"); });
  connect(ui->m_z, QOverload<int>::of(&QSpinBox::valueChanged), this,
          [this](int value) { moveModel(value, "m_z"); });
}

MainWindow::~MainWindow() {
  saveSettings();
  delete ui;
}

void MainWindow::defaultSettings() {
  ui->comboBox->setCurrentIndex(0);
  ui->choose_pieaks->setCurrentIndex(0);
  ui->cng_line->setCurrentIndex(0);
  ui->peaks_scale->setValue(0);
  ui->line_scale->setValue(0);

  ui->cinema->projectionMode = 0;
  ui->cinema->dotColor = Qt::white;
  ui->cinema->lineColor = Qt::white;
  ui->cinema->backgroundColor = Qt::black;
  ui->cinema->lineSize = 0.5f;
  ui->cinema->pointSize = 0.5f;
  ui->cinema->pointMode = 0;
  ui->cinema->lineMode = 0;

  ui->cinema->update();
}

void MainWindow::previousSettings() {
  QString settingFile = settings.fileName();
  if (QFile::exists(settingFile)) {
    ui->comboBox->setCurrentIndex(settings.value("projection").toInt());
    ui->cinema->dotColor = settings.value("point color").toString();
    ui->cinema->lineColor = settings.value("line color").toString();
    ui->cinema->backgroundColor = settings.value("background color").toString();
    ui->choose_pieaks->setCurrentIndex(settings.value("point type").toInt());
    ui->cng_line->setCurrentIndex(settings.value("line type").toInt());
    ui->peaks_scale->setValue(settings.value("point size").toInt());
    ui->line_scale->setValue(settings.value("line size").toInt());
    ui->h_peaks_scale->setValue(settings.value("point size slider").toInt());
    ui->h_line_scale->setValue(settings.value("line size slider").toInt());
  }
}

void MainWindow::saveSettings() {
  //    QSettings settings(settingFile, QSettings::IniFormat);

  settings.setValue("projection", ui->comboBox->currentIndex());
  settings.setValue("point color", ui->cinema->dotColor);
  settings.setValue("line color", ui->cinema->lineColor);
  settings.setValue("background color", ui->cinema->backgroundColor);
  settings.setValue("point type", ui->choose_pieaks->currentIndex());
  settings.setValue("line type", ui->cng_line->currentIndex());
  settings.setValue("point size", ui->peaks_scale->value());
  settings.setValue("line size", ui->line_scale->value());
  settings.setValue("point size slider", ui->h_peaks_scale->value());
  settings.setValue("line size slider", ui->h_line_scale->value());
}

void MainWindow::on_choose_obj_clicked() {
  QString filePath = QFileDialog::getOpenFileName(this, tr("Выберите файл"), "",
                                                  tr("Файлы (*.obj)"));
  if (!filePath.isEmpty()) {
    QFileInfo fileInfo(filePath);
    ui->name->setText(fileInfo.fileName());
    QByteArray byteArrayFileInfo = filePath.toLocal8Bit();
    const char *charFilePath = byteArrayFileInfo.constData();
    unsigned int *vertexIndices = nullptr;
    unsigned int vertexIndicesCount = 0;
    float *vertices = nullptr;
    unsigned int vertexCount = 0;
    processObjFile(charFilePath, &vertexIndices, &vertexIndicesCount, &vertices,
                   &vertexCount);
    int GeminiCount = vertexCount / 3;
    int edgesCount = vertexIndicesCount / 3;
    QString stringCount = QString::number(GeminiCount);
    QString verticesCount = QString::number(edgesCount);
    ui->kolvo_pieaks->setText(stringCount);
    ui->kolvo_reber->setText(verticesCount);

    QLayout *existingLayout = ui->cinema->layout();
    if (existingLayout) {
      delete existingLayout;
    }
    // Создаем экземпляр MyOpenGLWidget (если его нет) и добавляем его в
    // "cinema"
    MyOpenGLWidget *glWidget = dynamic_cast<MyOpenGLWidget *>(ui->cinema);
    if (!glWidget) {
      glWidget = new MyOpenGLWidget(this);
      QVBoxLayout *layout = new QVBoxLayout(ui->cinema);
      layout->addWidget(glWidget);
      layout->setContentsMargins(0, 0, 0, 0);
    }
    if (glWidget) {
      // Загружаем данные в glWidget
      glWidget->loadOBJData(&vertices, &vertexCount, &vertexIndices,
                            &vertexIndicesCount);
    }
    // Освобождаем память, выделенную в processObjFile
    if (vertices != nullptr) free(vertices);
    if (vertexIndices != nullptr) free(vertexIndices);
  }
}

void MainWindow::on_pushButton_plus_clicked() {
  // Увеличиваем масштаб модели на какое-то значение (например, 1.1 для
  // увеличения на 10%)
  const float scaleFactor = 1.1f;
  // int scPrev = scaleFactor;
  ui->cinema->changeModelScale(scaleFactor);
}

void MainWindow::on_pushButton_minus_clicked() {
  const float scaleFactor = 0.9f;

  ui->cinema->changeModelScale(scaleFactor);
}

void MainWindow::on_pushButton_Sbros_clicked() {
  ui->cinema->lineSize = 0.5f;
  ui->cinema->lineMode = 0;

  defaultSettings();

  ui->m_x->setValue(0);
  ui->m_y->setValue(0);
  ui->m_z->setValue(0);

  ui->cinema->update();
}

void MainWindow::on_pushButton_clicked()  //выход их программы
{
  QApplication::quit();
}

void MainWindow::on_comboBox_currentIndexChanged(int index) {
  if (index == 0) {
    ui->cinema->projectionMode = 0;
    ui->cinema->update();
  } else if (index == 1) {
    ui->cinema->projectionMode = 1;
    ui->cinema->update();
  }
}

void MainWindow::on_choose_pieaks_currentIndexChanged(int index) {
  if (index == 0) {  // отсутствует
    ui->cinema->pointMode = 0;
    ui->cinema->update();
  } else if (index == 1) {  // round
    ui->cinema->pointMode = 1;
    ui->cinema->update();
  } else if (index == 2) {  // square
    ui->cinema->pointMode = 2;
    ui->cinema->update();
  }
}

void MainWindow::on_cng_line_currentIndexChanged(int index) {
  if (index == 0) {  // сплошная
    ui->cinema->lineMode = 0;
    ui->cinema->update();
  } else if (index == 1) {  // пунктирная
    ui->cinema->lineMode = 1;
    ui->cinema->update();
  }
}

void MainWindow::moveModel(int value, const QString &objectName) {
  MyOpenGLWidget *glWidget = dynamic_cast<MyOpenGLWidget *>(
      ui->cinema);  // Получите указатель на виджет OpenGL

  if (glWidget) {
    if (objectName == "m_x") {
      glWidget->translation(0, value);
    } else if (objectName == "m_y") {
      glWidget->translation(1, value);
    } else if (objectName == "m_z") {
      glWidget->translation(2, value);
    }
    glWidget->update();
  }
}

void MainWindow::on_v_x_valueChanged(int value) {
  ui->cinema->rotation(value, 0);
}

void MainWindow::on_v_y_valueChanged(int value) {
  ui->cinema->rotation(value, 1);
}

void MainWindow::on_v_z_valueChanged(int value) {
  ui->cinema->rotation(value, 2);
}

void MainWindow::on_peaks_scale_valueChanged(int value) {
  ui->cinema->pointSize = value;
  ui->cinema->update();
}

void MainWindow::on_line_scale_valueChanged(int value) {
  ui->cinema->lineSize = value;
  ui->cinema->update();
}

void MainWindow::on_fon_clicked() {
  QColor color = QColorDialog::getColor(QColor(0.0, 0.0, 0.0, 0));

  if (color.isValid()) {
    ui->cinema->setBackgroundColor(color);
    ui->cinema->update();
  }
}

void MainWindow::on_peak_clicked() {
  QColor color = QColorDialog::getColor(QColor(0.0, 0.0, 0.0, 0));
  if (color.isValid()) {
    ui->cinema->setDotColor(color);
    ui->cinema->update();
  }
}

void MainWindow::on_eadge_clicked() {
  QColor color = QColorDialog::getColor(QColor(0.0, 0.0, 0.0, 0));
  if (color.isValid()) {
    ui->cinema->setLineColor(color);
    ui->cinema->update();
  }
}

void MainWindow::on_mk_screen_clicked() {
  // Check if a screenshot is already taken
  if (screenshotTaken) {
    // You can show a message to inform the user that a screenshot has already
    // been taken
    QMessageBox::information(this, "Info",
                             "Screenshot has already been taken.");
    return;
  }
  // Set the flag to true to prevent taking multiple screenshots in a row
  screenshotTaken = true;
  QTime time = QTime::currentTime();
  QString curTime = time.toString("hh_mm_ss");
  QImage screenshotImage = ui->cinema->grab().toImage();
  // Allow the user to choose a directory only if a screenshot hasn't been taken
  // yet
  QString defaultPath = QDir::homePath();
  QString selectedDirectory = QFileDialog::getExistingDirectory(
      this, "Выберите директорию для сохранения скриншота", defaultPath);
  if (!selectedDirectory.isEmpty() && ui->format->currentIndex() != 1) {
    // Check the value of the spin box
    QString filePath = selectedDirectory + "/screenshot_" + curTime + ".jpeg";
    screenshotImage.save(filePath);
  } else {
    QString filePath = selectedDirectory + "/screenshot_" + curTime + ".bmp";
    screenshotImage.save(filePath);
  }
  // Reset the flag to false after saving the screenshot
  screenshotTaken = false;
}

void MainWindow::on_mk_gif_clicked() {
  std::vector<QImage> frameImages;

  MyOpenGLWidget *glWidget = dynamic_cast<MyOpenGLWidget *>(ui->cinema);
  if (glWidget) {
    const int frameWidth = 640;
    const int frameHeight = 480;
    const int fps = 10;
    const int totalFrames =
        fps *
        5;  // Общее количество кадров для GIF (5 секунд * 10 кадров в секунду)

    for (int frameIndex = 0; frameIndex < totalFrames; ++frameIndex) {
      QImage frame = glWidget->grabFramebuffer().copy();
      frame = frame.scaled(frameWidth, frameHeight);

      // Используем метод transformed для поворота изображения на 180 градусов
      frame = frame.transformed(QTransform().rotate(180));

      frameImages.push_back(frame);

      std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));
    }

    std::thread gifThread(&MainWindow::createGif, this, frameImages, frameWidth,
                          frameHeight);
    gifThread.detach();
  }
}

// Создание GIF из сохраненных кадров
void MainWindow::createGif(const std::vector<QImage> &frameImages,
                           int frameWidth, int frameHeight) {
  QString file_name = QDir::homePath() + "/gif_image.gif";
  if (QFile::exists(file_name)) {
    QFile::remove(file_name);
  }

  const int fps = 10;
  int delay = 1000 / fps;

  GifWriter gif_wr;
  GifBegin(&gif_wr, file_name.toStdString().c_str(), frameWidth, frameHeight,
           delay);

  for (const auto &frame : frameImages) {
    GifWriteFrame(&gif_wr, frame.constBits(), frameWidth, frameHeight, delay);
  }

  GifEnd(&gif_wr);
}
