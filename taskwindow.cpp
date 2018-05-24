#include "taskwindow.h"
#include "ui_taskwindow.h"

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

