#include "taskchoice.h"
#include "ui_taskchoice.h"

TaskChoice::TaskChoice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskChoice)
{
    ui->setupUi(this);
}

TaskChoice::~TaskChoice()
{
    delete ui;
}

void TaskChoice::on_backButton_clicked()
{
    this->close();
}

void TaskChoice::on_trainButton_clicked()
{
    task_type = 1;
}

void TaskChoice::on_acquiButton_clicked()
{
    task_type = 2;
}

int TaskChoice::get() const{
    return task_type;
}
