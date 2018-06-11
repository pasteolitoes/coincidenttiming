#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#define WEBCAM 0
#define FLEX13 1

#include <QDialog>
#include <QTimer>
#include <QThread>
#include <QDebug>
#include <opencv2\opencv.hpp>
#include <opencv\highgui.h>
#include <opencv\cv.h>
#include <opencv2\videoio.hpp>
#include <cameralibrary.h>
#include <windows.h>

namespace Ui {
class TaskWindow;
}

class TaskWindow : public QDialog
{
    Q_OBJECT

public:

    TaskWindow(QWidget *parent = 0);

    ~TaskWindow();

    /* ------Variáveis básicas da tarefa------ */
    int taskType;
    int taskRep;
    int taskVel;
    int cam;
    int flag = 0;

    bool breakLoop = false;

    /* ------Variáveis do alvo------*/
    double area;
    int score = 0;
    bool targetFound = false;
    const int MIN_OBJECT_AREA = 200;
    const int MAX_OBJECT_AREA = 30000;

    /* ------Variáveis da câmera e da imagem------ */
    int camWidth;
    int camHeight;
    const int WIDTH = 640;
    const int HEIGHT = 480;
    const int BACKBUFFER_BITSPERPIXEL = 8;

    cv::VideoCapture capture;
    CameraLibrary::Camera *camFlex;





    /*------Variáveis de calibração e processamento------*/

    int block_t = 323;
    int gaussC_t = 75;
    int mult_t = 8;
    int thr_t = 0;


public:

    void setInfo(int camera, int rep, int vel, int type);

    void startCamera(int camera);

private slots:
    void on_startTaskButton_clicked();

private:


private:
    Ui::TaskWindow *ui;

    QTimer timer;
};

#endif // TASKWINDOW_H
