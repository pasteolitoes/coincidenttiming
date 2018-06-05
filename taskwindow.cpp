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

void TaskWindow::startCamera(int camera)
{
    cv::namedWindow("Imagem Tarefa");
    if (camera == WEBCAM)
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

            capture.read(matFrame);

            cv::imshow("Imagem Tarefa", matFrame);

            if(GetAsyncKeyState(VK_ESCAPE)){
                breakLoop = true;
            }

            cv::waitKey(30);
        }

    }
    //Inicia a câmera FLEX13 e começa a gravação de imagem
    else if (camera == FLEX13)
    {
        CameraManager::X().WaitForInitialization();
        camFlex = CameraManager::X().GetCamera();

        if(!camera){
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

        Frame *frame = camera->getFrame;

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

            threshold(matFrame, matFrame, 200, 255, THRESH_BINARY);

//            findTargetFlex(matFrame, finalFrame);

            imshow("Imagem Tarefa", finalFrame);

            if(GetAsyncKeyState(VK_ESCAPE)){
                breakLoop = true;
            }

            cv::waitKey(5);
        }
    }
}

