#include "mainmenu.h"
#include "ui_mainmenu.h"
#include "calibration.h"
#include "taskchoice.h"

MainMenu::MainMenu(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainMenu),
    _timer(Q_NULLPTR)
{

    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

}

MainMenu::~MainMenu()
{
    delete ui;
}

//Inicia o programa
void MainMenu::on_startButton_clicked()
{
    //Abre diálogo para seleção do tipo de tarefa
    TaskChoice t_choice;

    t_choice.setModal(true);

    t_choice.exec();

    task = t_choice.get();

    //Caso treino selecionado
    if (task == 1){
        velocity = ui->trainingVelBox->text().toInt();
        repetitions = ui->trainingRepBox->text().toInt();
    }
    //Caso aquisição selecionada
    else if (task == 2){
        velocity = ui->acquiVelBox->text().toInt();
        repetitions = ui->acquiRepBox->text().toInt();
    }

    if (ui->flexButton->isChecked())
    {
        cam = FLEX13;
    } else if (ui->webcamButton->isChecked())
    {
        cam = WEBCAM;
    }
}

void MainMenu::on_calibrationButton_clicked()
{
    this->hide();

}

//Acessa a tela de configurações da tarefa
void MainMenu::on_configButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


//Volta os valores das variáveis de treino e aquisição para os padrões
void MainMenu::on_defaultButton_clicked()
{
    ui->acquiRepBox->setValue(7);
    ui->acquiVelBox->setValue(200);
    ui->trainingRepBox->setValue(21);
    ui->trainingVelBox->setValue(500);
}

//Aplica os valores alterados da configuração para as variáveis de treino aquisição
//e volta para o menu inicial
void MainMenu::on_applyButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

//Fecha o programa
void MainMenu::on_exitButton_clicked()
{
    this->close();
}


