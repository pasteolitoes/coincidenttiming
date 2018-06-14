#include "taskwindow.h"
#include "ui_taskwindow.h"

using namespace CameraLibrary;
using namespace std;

TaskWindow::TaskWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskWindow)
{
    ui->setupUi(this);

//    timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), this, SLOT(nextState()));

    connect(&clockTimer, SIGNAL(timeout()), this, SLOT(updateTime()));

    connect(this, SIGNAL(finishedTask()), this, SLOT(onFinished()));
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
    ui->startTaskButton->hide();
    ui->exitButton->hide();
    cv::namedWindow("nessec", cv::WINDOW_AUTOSIZE);
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

        qDebug() << camWidth;
        qDebug() << camHeight;

        // cv::Mat matFrame(cv::Size(WIDTH, HEIGHT), CV_8UC1);
        cv::Mat matFrame(cv::Size(camWidth, camHeight), CV_8UC1);
        cv::Mat finalFrame;
        matFrame.copyTo(finalFrame);

        Frame *frame = camFlex->GetFrame();

        while(true)
        {
            //termina a captura de imagens
            if(breakLoop == true) break;

            //captura das imagens
            frame = camFlex->GetFrame();

            if(frame)
            {
                frame->Rasterize(camWidth, camHeight, matFrame.step, BACKBUFFER_BITSPERPIXEL, matFrame.data);
                frame->Release();
            }

            //Quando a tecla "i" é pressionada, inicia a tarefa, mudando o estado para o inicial
            if(GetAsyncKeyState(0x49)){
                if (firstFlag == false){
                    qDebug() << "Entrou aqui";
                    ui->timeLabel->setText(QTime(0, 0, 0, 0).toString());
                    c.restart();
                    firstFlag = true;
                    clockTimer.start(50);
                    times.append(QTime::currentTime());
                }
                if (state == -1){
                    qDebug() << "Entrou no estado -1";
                    t0 = c.getTime();
                    times.append(t0);
                    firstState();
                    doneCount++;
                    ui->repLabel->setText(QString::number(doneCount)+
                                          "/"+QString::number(taskRep));

                }
            }

            if (firstFlag == true && state != -1){
                if(state == (nBlocks-1) && -(c.getTime().msecsTo(t0)) >= taskVel*(state) + 2000 && FIM == false){
                    qDebug() << c.getTime().msecsTo(t0);
                    FIM = true;
                    score.append(0);
                    QSound::play(":/sounds/216090__richerlandtv__bad-beep-incorrect.wav");
                    times.append(c.getTime());
                    restartState();
                }

                else if(state < (nBlocks-1) && (-(c.getTime().msecsTo(t0)) >= taskVel*(state+1))){
                    nextState();
                }
            }


//            if(state == nBlocks-1){
//                if(finalFlag==false){
//                    stopTimer();
//                    qDebug() << c.getTime();
//                    QTimer::singleShot(200, this, SLOT(restartState()));
//                }

//                finalFlag = true;
//            }

            threshold(matFrame, matFrame, 200, 255, cv::THRESH_BINARY);

            findTarget(matFrame, finalFrame);

            imshow("nessec", finalFrame);

            if(GetAsyncKeyState(0x45)){
                break;
            }

            cv::waitKey(5);
        }

        camFlex->Stop();
        camFlex->Release();
        ui->exitButton->show();
        clockTimer.stop();
        c.restart();
    }
}

void
TaskWindow::on_exitButton_clicked()
{
    this->close();
}

void
TaskWindow::firstState()
{

    qDebug() << "First State";

    state = 0;

    finalFlag = false;
}

void
TaskWindow::nextState()
{
    state++;
    qDebug() << "Next state: " << state;
}

void
TaskWindow::restartState()
{
    state = -1;
    flag = 0;

    qDebug() << doneCount;

    FIM = false;

    if(doneCount == taskRep)
    {
        qDebug() << "Começou a finalizar";
        times.append(QTime::currentTime());
        emit finishedTask();
    }
}


void
TaskWindow::updateTime()
{
    ui->timeLabel->setText(c.getTime().toString("hh:mm:ss.zzz"));
}

void
TaskWindow::findTarget(cv::Mat &matFrame, cv::Mat &finalFrame)
{
    vector<vector<cv::Point> > contornos;
    vector<cv::Point> approx;

    int alvo = 0;
    int quadrado = 0;
    int formas = 0;

    cv::Scalar color(66, 238, 244);
    cv::Scalar color_g(38, 249, 14);
    cv::Scalar color_c(194, 196, 198);
    cv::Scalar color_w(255,255,255);

    cv::findContours(matFrame, contornos, cv::RETR_LIST, CV_CHAIN_APPROX_SIMPLE);


    for (int i = 0; i < contornos.size(); i++)
    {
        cv::approxPolyDP(cv::Mat(contornos[i]), approx, 0.02 * mult_t * cv::arcLength(cv::Mat(contornos[i]), true), true);

        area = cv::contourArea(contornos[i]);

        if (area > MIN_OBJECT_AREA)
        {
            if (approx.size() == 4)
            {
                quadrado = i;
                alvo ++;
                targetFound = true;
                formas++;
            }
        }

//        if(alvo == 0)
//        {
//            targetFound = 0;
//            if (state == (nBlocks-1) && flag == 0)
//            {
//                score.append(1);
//                flag++;
//                QSound::play(":/sounds/243701__ertfelda__correct.wav");
//                times.append(c.getTime());
//            }

//            else if (state != -1 && state < (nBlocks-1) && flag == 0)
//            {
//                qDebug() << "Entrou quando errou";
//                score.append(0);
//                flag++;
//                QSound::play(":/sounds/216090__richerlandtv__bad-beep-incorrect.wav");
//                times.append(c.getTime());
//                break;
//            }
//        }



        if(FIM == false && alvo == 0 && flag == 0 && state >= 0)
        {
            FIM = true;
            targetFound = 0;
            QTime tHand = c.getTime();
            QTime tAlvo = t0.addMSecs((nBlocks-1)*taskVel);
            int dif = tAlvo.msecsTo(tHand);
            qDebug() << "tHand: " << tHand;
            qDebug() << "tAlvo: " << tAlvo;
            qDebug() << "Diferença " << dif;
            if (dif > -200 && dif < 200)
            {
                flag++;
                score.append(1);
                QSound::play(":/sounds/243701__ertfelda__correct.wav");
                times.append(c.getTime());
                restartState();
            }
            else
            {
                qDebug() << "Entrou quando errou";
                score.append(0);
                flag++;
                QSound::play(":/sounds/216090__richerlandtv__bad-beep-incorrect.wav");
                times.append(c.getTime());
                restartState();
            }

        }


    }
    cv::cvtColor(matFrame, finalFrame, cv::COLOR_GRAY2BGR);
    if (formas > 0){
        if(state >= 0){
            for(int i = 0; i < (nBlocks-1); i++){
                if(state < (nBlocks-1)){
                    if (i == 0){
                        cv::drawContours(finalFrame, contornos, quadrado, color_c, CV_FILLED, cv::LINE_8);
                        cv::drawContours(finalFrame, contornos, quadrado, color_w, 2);
                    }
                    if(((nBlocks-2)-state) == i){
                        cv::drawContours(finalFrame, contornos, quadrado, color, CV_FILLED, cv::LINE_8, cv::noArray(), INT_MAX, cv::Point(0, -(90 + (i*90))));
                        cv::drawContours(finalFrame, contornos, quadrado, color_w, 2, cv::LINE_8, cv::noArray(), INT_MAX, cv::Point(0, -(90+(i*90))));
                    }else {
                        cv::drawContours(finalFrame, contornos, quadrado, color_c, CV_FILLED, cv::LINE_8, cv::noArray(), INT_MAX, cv::Point(0, -(90 + (i*90))));
                        cv::drawContours(finalFrame, contornos, quadrado, color_w, 2, cv::LINE_8, cv::noArray(), INT_MAX, cv::Point(0, -(90+(i*90))));
                    }
                } else {
                    if(i == 0){
                        cv::drawContours(finalFrame, contornos, quadrado, color_g, CV_FILLED, cv::LINE_8);
                        cv::drawContours(finalFrame, contornos, quadrado, color_w, 2);
                    }
                    cv::drawContours(finalFrame, contornos, quadrado, color_c, CV_FILLED, cv::LINE_8, cv::noArray(), INT_MAX, cv::Point(0, -(90 + (i*90))));
                    cv::drawContours(finalFrame, contornos, quadrado, color_w, 2, cv::LINE_8, cv::noArray(), INT_MAX, cv::Point(0, -(90+(i*90))));
                }
            }
        }
    }
}

void
TaskWindow::onFinished()
{
    qDebug() << "Entrou aqui";

    breakLoop = true;

    bool closed = false;

    while(closed == false)
    {
        QDir currentDir = QApplication::applicationDirPath();
        QString startingPath = currentDir.absolutePath();

        QString fileName = QFileDialog::getSaveFileName(
                    this,
                    tr("Salvar Resultados"),
                    startingPath,
                    tr("Text Files (*.txt)") );

        if( fileName.isEmpty() )
        {
            QMessageBox::warning(this,"Erro ao criar arquivo", "O arquivo não pôde ser salvo, tente novamente" );
            continue;
        }

        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly | QFile::Text) )
        {
            QMessageBox::warning(this,"Erro ao abrir arquivo", "O arquivo não pôde ser aberto para salvar, tente novamente " + file.errorString() );
            continue;
        }

        QTextStream out(&file);

        out << QDateTime::currentDateTime().toString() << endl;

        if(taskType == 1){
            out << "Velocidade da tarefa de Treino: " <<  QString::number(taskVel) << endl;
        }
        else if (taskType == 2)
        {
            out << "Velocidade da tarefa de Aquisição: " << QString::number(taskVel) << endl;
        }



        out << "Time" << endl;

        for(int i = 0; i < times.size(); i++)
        {
            if (i == 0)
            {
                out << "START TIME:" << times[i].toString("hh:mm:ss.zzz") << endl;
            } else {
                out << times[i].toString("hh:mm:ss.zzz") << endl;
            }

        }

        for(int j = 0; j < score.size(); j++){
            out << QString::number(score[j]) << "  ";
        }

        file.close();

        closed = true;
    }
}
