#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //makes stuff semi-transparent:
    //setWindowOpacity(0.5);

    // if (ui->iwad_select_combo->view()->parentWidget()) {
    //     ui->iwad_select_combo->view()->parentWidget()->setWindowOpacity(0.5);
    // }
    // if (ui->modset_select_combo->view()->parentWidget()) {
    //     ui->modset_select_combo->view()->parentWidget()->setWindowOpacity(0.5);
    // }
    if (ui->mods_selection_list) {
        ui->mods_selection_list->setWindowOpacity(0.5);
    }

    QStringList  itemLabels{"one", "two", "three"}; //getLabels();

    QStringListIterator it(itemLabels);
    while (it.hasNext())
    {
        QListWidgetItem *listItem = new QListWidgetItem(it.next(),ui->mods_selection_list);
        listItem->setCheckState(Qt::Unchecked);
        ui->mods_selection_list->addItem(listItem);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_iwad_select_combo_currentIndexChanged(int index)
{

}

void MainWindow::on_modset_select_combo_currentIndexChanged(int index)
{

}


void MainWindow::on_mods_selection_list_itemDoubleClicked(QListWidgetItem *item)
{

}

