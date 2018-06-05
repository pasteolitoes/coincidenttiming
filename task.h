//#ifndef TASK_H
//#define TASK_H

//#include <QObject>
//#include <QDialog>
//#include <QTimer>
//#include <QThread>
//#include <QDebug>
//#include <opencv2\opencv.hpp>
//#include <opencv\highgui.h>
//#include <opencv\cv.h>
//#include <opencv2\videoio.hpp>
//#include <cameralibrary.h>

//class Task : public QObject
//{
//    Q_OBJECT
//public:
//    explicit Task(int cam, int task, int velocity, int repetitions);

//    /* ------Variáveis básicas da tarefa------ */
//    int taskType;
//    int taskRep;
//    int taskVel;
//    int cam;
//    int flag = 0;

//    bool breakLoop = false;

//    /* ------Variáveis do alvo------*/
//    double area;
//    int score = 0;
//    bool targetFound = false;
//    const int MIN_OBJECT_AREA = 200;
//    const int MAX_OBJECT_AREA = 30000;

//    /* ------Variáveis da câmera e da imagem------ */
//    int camWidth;
//    int camHeight;
//    const int WIDTH = 640;
//    const int HEIGHT = 480;
//    const int BACKBUFFER_BITSPERPIXEL = 8;

//    cv::VideoCapture capture;
//    CameraLibrary::Camera *camera;

//    /*------Variáveis de calibração e processamento------*/

//    int block_t = 323;
//    int gaussC_t = 75;
//    int mult_t = 8;
//    int thr_t = 0;

//signals:
//    void frameReady();

//public slots:


//private:

//};

//#endif // TASK_H
