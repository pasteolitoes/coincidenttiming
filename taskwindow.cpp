#include "taskwindow.h"
#include "ui_taskwindow.h"

using namespace CameraLibrary;

TaskWindow::TaskWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskWindow)
{
    ui->setupUi(this);
}

TaskWindow::~TaskWindow()
{

    this->close();
    delete ui;
}

//Seta as informações necessárias para a realização da tarefa que
//foram configuradas a partir do menu principal
void TaskWindow::setInfo(int camera, int rep, int vel, int type)
{
    cam = camera;
    taskRep = rep;
    taskType = type;
    taskVel = vel;

    if (taskType == 1)
    {
        ui->typeLabel->setText("Treino");
    }
    else if (taskType == 2)
    {
        ui->typeLabel->setText("Aquisição");
    }

    ui->repLabel->setText("0/"+QString::number(taskRep));
    return;
}


void TaskWindow::on_startTaskButton_clicked()
{
    cv::namedWindow("nessec", cv::WINDOW_NORMAL);
    if (cam == WEBCAM)
    {
        cv::Mat matFrame;
        //cv::Mat grayScale;
        //Inicializa a webcam
        capture.open(CV_CAP_ANY);
        capture.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
        capture.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);

        while(true)
        {
            if(breakLoop == true) return;

            capture >> matFrame;

//            cv::imshow("Imagem Tarefa", matFrame);

            cv::resize(matFrame, matFrame, cv::Size(ui->label_image->width(), ui->label_image->height()));

            QImage imgFrame = QImage((uchar*) matFrame.data, matFrame.cols, matFrame.rows, matFrame.step, QImage::Format_RGB888);
            QPixmap pixFrame = QPixmap::fromImage(imgFrame);

            ui->label_image->setPixmap(pixFrame);

            if(GetAsyncKeyState(VK_ESCAPE)){
                breakLoop = true;
            }

            cv::waitKey(30);
        }

    }
    //Inicia a câmera FLEX13 e começa a gravação de imagem
    else if (cam == FLEX13)
    {
        CameraManager::X().WaitForInitialization();
        camFlex = CameraManager::X().GetCamera();

        if(!camFlex){
            qDebug() << QString("Camera não encontrada!");
        }

        camFlex->SetVideoType(Core::GrayscaleMode);
        camFlex->SetExposure(50);
        camFlex->SetThreshold(200);
        camFlex->SetIntensity(15);

        camFlex->Start();
        camWidth = camFlex->Width();
        camHeight = camFlex->Height();

        cv::Mat matFrame(cv::Size(camWidth, camHeight), CV_8UC1);
        cv::Mat finalFrame;
        matFrame.copyTo(finalFrame);

        Frame *frame = camFlex->GetFrame();

        while(true)
        {
            if(breakLoop == true) return;

            frame = camFlex->GetFrame();

            if(frame)
            {
                frame->Rasterize(camWidth, camHeight, matFrame.step, BACKBUFFER_BITSPERPIXEL, matFrame.data);
                frame->Release();
            }

//            if(GetAsyncKeyState(0x49)){
//                if (state == -1){
//                    inicia_estado();
//                }
//            }

//            if(state == 4){
//                para_timer();
//            }

            threshold(matFrame, matFrame, 200, 255, cv::THRESH_BINARY);

//            findTargetFlex(matFrame, finalFrame);

            imshow("Imagem Tarefa", finalFrame);

            if(GetAsyncKeyState(VK_ESCAPE)){
                breakLoop = true;
            }

            cv::waitKey(5);
        }
    }
}
