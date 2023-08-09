#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore>
#include <QtGui>
#include <fstream>
#include <iostream>
#include <QSound>
#include <QMediaPlayer>
#include <QFileDialog>
#include <QWidget>
#include <Qpixmap>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    WavPlayer = new QMediaPlayer(this);

    ui->setupUi(this);
    QPixmap pix("C:/Users/burak/Desktop/anayurt.jpg");
    ui->label_pic->setPixmap(pix.scaled(170,170,Qt::KeepAspectRatio));
    ui->pushButton_play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->pushButton_pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui->pushButton_Stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->pushButton_Seek_back->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    ui->pushButton_Seek_forward->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));



    ui->horizontalSlider_Audio_Volume->setMinimum(1);
    ui->horizontalSlider_Audio_Volume->setMaximum(100);
    ui->horizontalSlider_Audio_Volume->setValue(30);

    WavPlayer->setVolume(ui->horizontalSlider_Audio_Volume->value());

    connect(WavPlayer,&QMediaPlayer::durationChanged,this, &MainWindow::durationChanged);
    connect(WavPlayer,&QMediaPlayer::positionChanged,this, &MainWindow::positionChanged);



    ui->horizontalSlider->setRange(0,WavPlayer->duration()/1000);


    Statlabel = new QLabel(this);
    StatProgress = new QProgressBar(this);

    ui->statusbar->addPermanentWidget(Statlabel);
    ui->statusbar->addPermanentWidget(StatProgress,15);
    StatProgress->setTextVisible(false);

}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::uptateduration(qint64 duration)
{
    if(duration || Mduration){
        QTime CurrentTime((duration/3600)%60,(duration /60)% 60, duration %60,(duration *1000) % 1000);
        QTime totalTime((Mduration/3600) % 60,(Mduration /60)% 60, Mduration % 60, (Mduration * 1000)%1000);
        QString format = "mm:ss";
        if(Mduration > 3600)
            format = "hh:mm:ss";
        ui->label_Value1->setText(CurrentTime.toString(format));
        ui->label_Value_2->setText(totalTime.toString(format));

    }

}


void MainWindow::durationChanged(qint64 duration)
{
    Mduration=duration/1000;
    ui->horizontalSlider->setMaximum(Mduration);
}

void MainWindow::positionChanged(qint64 progress)
{
    if(!ui->horizontalSlider->isSliderDown()){
        ui->horizontalSlider->setValue(progress / 1000);
    }
    uptateduration(progress / 1000);

}

void MainWindow::on_pushButton_clicked()
{
    QString selectedFile = QFileDialog::getOpenFileName(this, "Select File", QDir::homePath(), "Text files (*.txt)");
    if (!selectedFile.isEmpty()) {
        readTxtFile(selectedFile.toStdString());
        writeWavFile("C:/Users/burak/Documents/txt_to_file/output.wav");
        StatProgress->setValue(100);
    } else {
        qDebug() << "No file selected.";
        StatProgress->setValue(0);
    }
}
void MainWindow::readTxtFile(const std::string& filePath)
{
    // Open the txt file
    std::ifstream inFile(filePath);
    if (!inFile) {
        qDebug() << "Error opening input file.";
        return;
    }
    std::vector<float> data;
    // Read the float array data from the text file
    data.clear(); // Clear any existing data
    std::string line;
    while (std::getline(inFile, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
        float value = std::stof(line);
        data.push_back(value);
    }

    // Close the input file
    inFile.close();
    // Store the data in the member variable
    this->data = data;
}

void MainWindow::writeWavFile(const std::string& filePath)
{
    const int sampleRate = 22050;
    const int channels = 1;  // Mono
    const int numSamples = data.size();

    // Open the output WAV file
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile) {
        qDebug() << "Error opening output file.";
        return;
    }

    // Write the WAV file header
    // RIFF chunk
    outFile.write("RIFF", 4);  // Chunk ID
    const int chunkSize = 36 + numSamples * sizeof(float);
    outFile.write(reinterpret_cast<const char*>(&chunkSize), 4);  // Chunk Size
    outFile.write("WAVE", 4);  // Format

    // Format subchunk
    outFile.write("fmt ", 4);  // Subchunk1 ID
    const int subchunk1Size = 16;
    outFile.write(reinterpret_cast<const char*>(&subchunk1Size), 4);  // Subchunk1 Size
    const short audioFormat = 3;  // Floating-point format
    outFile.write(reinterpret_cast<const char*>(&audioFormat), 2);  // Audio Format
    outFile.write(reinterpret_cast<const char*>(&channels), 2);  // Num Channels
    outFile.write(reinterpret_cast<const char*>(&sampleRate), 4);  // Sample Rate
    const int byteRate = sampleRate * channels * sizeof(float);
    outFile.write(reinterpret_cast<const char*>(&byteRate), 4);  // Byte Rate
    const short blockAlign = channels * sizeof(float);
    outFile.write(reinterpret_cast<const char*>(&blockAlign), 2);  // Block Align
    const short bitsPerSample = 8 * sizeof(float);
    outFile.write(reinterpret_cast<const char*>(&bitsPerSample), 2);  // Bits Per Sample

    // Data subchunk
    outFile.write("data", 4);  // Subchunk2 ID
    const int subchunk2Size = numSamples * sizeof(float);
    outFile.write(reinterpret_cast<const char*>(&subchunk2Size), 4);  // Subchunk2 Size

    // Write the float array data to the file
    outFile.write(reinterpret_cast<const char*>(data.data()), numSamples * sizeof(float));

    // Close the output file
    outFile.close();
    qDebug() << "WAV file created: " << QString::fromStdString(filePath);
}

void MainWindow::on_pushButton_Volume_clicked()
{
    if (IS_Muted== false){
        ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        IS_Muted = true;
        WavPlayer->setMuted(true);
    }
    else{
        IS_Muted =false;
        ui->pushButton_Volume->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
         WavPlayer->setMuted(false);
    }
}


void MainWindow::on_actionOpen_triggered()
{

    QString FileName = QFileDialog::getOpenFileName(this, tr("Select Audio File"),"",tr("Wav Files(*.wav)"));
    WavPlayer->setMedia(QUrl(FileName));

    QFileInfo fileinfo(FileName);

    ui->label_file_name->setText(fileinfo.fileName());

}


void MainWindow::on_pushButton_play_clicked()
{

    WavPlayer->play();
}


void MainWindow::on_pushButton_pause_clicked()
{
    WavPlayer->pause();

}

void MainWindow::on_pushButton_Stop_clicked()
{
      WavPlayer->stop();

}

void MainWindow::on_pushButton_Seek_forward_clicked()
{

      ui->horizontalSlider->setValue(ui->horizontalSlider->value()+20 );
      WavPlayer->setPosition(ui->horizontalSlider->value()*1000);
}
void MainWindow::on_pushButton_Seek_back_clicked()
{
      ui->horizontalSlider->setValue(ui->horizontalSlider->value()-20 );
      WavPlayer->setPosition(ui->horizontalSlider->value()*1000);
}


void MainWindow::on_horizontalSlider_valueChanged(int value)
{

      /*if (WavPlayer != nullptr) {
         WavPlayer->setPosition(value*1000);
      }*/
}
void MainWindow::on_horizontalSlider_Audio_Volume_valueChanged(int value)
{
      WavPlayer->setVolume(value);
}


