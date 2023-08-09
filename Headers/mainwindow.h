#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia>
#include <QtCore>
#include <QtWidgets>
#include <QtGui>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    std::vector<float> data;

private slots:

    void durationChanged(qint64 duration);

    void positionChanged(qint64 progress);

    void on_pushButton_clicked();

    void readTxtFile(const std::string& filePath);

    void writeWavFile(const std::string& filePath);

    void on_pushButton_Volume_clicked();

    void on_actionOpen_triggered();

    void on_pushButton_play_clicked();

    void on_pushButton_pause_clicked();

    void on_pushButton_Seek_forward_clicked();

    void on_pushButton_Stop_clicked();

    void on_pushButton_Seek_back_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_Audio_Volume_valueChanged(int value);

private:

    void uptateduration(qint64 duration);
    Ui::MainWindow *ui;

    bool IS_Muted = false;
    QMediaPlayer *WavPlayer;

    qint64 Mduration;

    QLabel * Statlabel;
    QProgressBar * StatProgress;


};
#endif // MAINWINDOW_H
