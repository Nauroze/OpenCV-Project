#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    webCam.open(0);

    if(webCam.isOpened()==false){
        ui->Radius->appendPlainText("Error: WebCam is not accessible.");
        return;
    }
    Timer= new QTimer(this);
    connect(Timer, SIGNAL(timeout()),this, SLOT(processFrameAndUpdateGUI()));
    Timer->start(20); //Thread would be preferrable here.


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::processFrameAndUpdateGUI(){
    webCam.read(matImgOriginal);

    if(matImgOriginal.empty()==true) return;

    cv::inRange(matImgOriginal, cv::Scalar(0, 0.175), cv::Scalar(100, 100, 256), matImgProcessed);
    cv::GaussianBlur(matImgProcessed,matImgProcessed,cv::Size(9,9),1.5);
    cv::HoughCircles(matImgProcessed,vecCircles,CV_HOUGH_GRADIENT,2,matImgProcessed.rows/4,100,50,10,400);

     for(itrCircles =vecCircles.begin();itrCircles !=vecCircles.end();itrCircles++)
       {
           ui->Radius->appendPlainText(QString("X: ")+ QString::number((*itrCircles)[0]).rightJustified(4, ' ') +
           QString("Y: ")+ QString::number((*itrCircles)[1]).rightJustified(4, ' ') +
           QString("Radius: ")+ QString::number((*itrCircles)[2], 'f', 3).rightJustified(7,' '));

           cv::circle(matImgOriginal, cv::Point((int)(*itrCircles)[0], (int)(*itrCircles)[1]), 3, cv::Scalar(0,255,0), CV_FILLED);
           cv::circle(matImgOriginal, cv::Point((int)(*itrCircles)[0], (int)(*itrCircles)[1]), (int)(*itrCircles)[2], cv::Scalar(0,255,0), 3);
     }

     cv::cvtColor(matImgOriginal, matImgOriginal, CV_BGR2RGB);

     QImage qimgOriginal((uchar*)matImgOriginal.data, matImgOriginal.cols, matImgOriginal.rows, matImgOriginal.step, QImage::Format_RGB888);
     QImage qimgProcessed((uchar*)matImgProcessed.data, matImgProcessed.cols, matImgProcessed.rows, matImgProcessed.step, QImage::Format_Indexed8);

     ui->Original->setPixmap(QPixmap::fromImage(qimgOriginal));
     ui->Processed->setPixmap(QPixmap::fromImage(qimgProcessed));

}

void MainWindow::on_Button_clicked()
{
    if(Timer->isActive()==true){
        Timer->stop();
        ui->Button->setText("Resume");
    } else {
        Timer->start(20);
        ui->Button->setText("pause");
    }

}
