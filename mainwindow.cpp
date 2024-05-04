#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //makes stuff semi-transparent:
    //setWindowOpacity(0.5);
    if (ui->mods_selection_list) {
        ui->mods_selection_list->setWindowOpacity(0.5);
    }

    QStringListIterator it(mod_manager_.GetModLabels());
    while (it.hasNext())
    {
        QListWidgetItem *listItem = new QListWidgetItem(it.next(),ui->mods_selection_list);
        listItem->setCheckState(Qt::Unchecked);
        ui->mods_selection_list->addItem(listItem);
    }

    ui->iwad_select_combo->addItems(mod_manager_.GetIwadLabels());
    ui->modset_select_combo->addItems(mod_manager_.GetModsetLabels());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_iwad_select_combo_currentIndexChanged(int index)
{
    mod_manager_.SetIwad(index);
}

void MainWindow::on_modset_select_combo_currentIndexChanged(int index)
{
    mod_manager_.SetModSet(index);
}

void MainWindow::on_launch_game_btn_clicked()
{
    cerr << "-> passing launch command: " << ui->modset_select_combo->currentIndex();
    mod_manager_.LaunchGame(ui->modset_select_combo->currentIndex());
}


void MainWindow::on_mods_selection_list_itemSelectionChanged()
{

}

