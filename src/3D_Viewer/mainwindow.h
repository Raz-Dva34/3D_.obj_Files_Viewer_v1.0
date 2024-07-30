#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <stdbool.h>

#include <QColor>
#include <QColorDialog>
#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>  // Для QMessageBox
#include <QMovie>
#include <QOpenGLWidget>
#include <QScrollArea>
#include <QSettings>
#include <QThread>  // Для QThread
#include <QVBoxLayout>
#include <QWidget>

//#include "./QtGifImage/src/gifimage/qgifimage.h"// Подключение заголовочного
//файла QGifImage

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();
  void createGif(const std::vector<QImage>& frameImages, int frameWidth,
                 int frameHeight);
  void on_m_x_valueChanged(int value);
  QImage m_frame;

 public slots:
  void moveModel(int value, const QString& objectName);

 private slots:

  void previousSettings();
  void defaultSettings();

  void saveSettings();

  void on_choose_obj_clicked();

  void on_pushButton_Sbros_clicked();

  void on_mk_gif_clicked();

  void on_pushButton_clicked();

  void on_comboBox_currentIndexChanged(int index);

  void on_choose_pieaks_currentIndexChanged(int index);

  void on_cng_line_currentIndexChanged(int index);

  void on_v_x_valueChanged(int value);

  void on_v_y_valueChanged(int value);

  void on_v_z_valueChanged(int value);

  void on_peaks_scale_valueChanged(int value);

  void on_line_scale_valueChanged(int value);
  void on_pushButton_plus_clicked();
  void on_pushButton_minus_clicked();

  void on_fon_clicked();

  void on_peak_clicked();

  void on_eadge_clicked();

  void on_mk_screen_clicked();

 private:
  Ui::MainWindow* ui;
  bool screenshotTaken = false;
  //    QString settingFile;
  QSettings settings;
};

#endif  // MAINWINDOW_H
