#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

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

    QStringListIterator it(mod_manager_.MakeModLabels());
    while (it.hasNext())
    {
        QListWidgetItem *listItem = new QListWidgetItem(it.next(),ui->mods_selection_list);
        listItem->setCheckState(Qt::Unchecked);
        ui->mods_selection_list->addItem(listItem);
    }

    auto iwads = mod_manager_.MakeIwadLabels();

    if(iwads.empty()) {
        ui->iwad_select_combo->addItems({"IWAD NOT FOUND"});
        //TODO: open wad dir select window
        ChooseIwadFolder();
    }
    else {
        ui->iwad_select_combo->addItems(iwads);
    }
    ui->modset_select_combo->addItems(mod_manager_.GetModsetLabels());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_iwad_select_combo_currentIndexChanged(int index)
{
    mod_manager_.SetIwad(index);
    UpdCmdDisplay();
}

void MainWindow::on_modset_select_combo_currentIndexChanged(int index)
{
    mod_manager_.SetModSet(index);

    mod_manager_.MakeModLabels();

    for(const auto& mod : mod_manager_.GetCfg().chosen_mods) {
        ui->mods_selection_list->item(mod)->setCheckState(Qt::Checked);
    }

    UpdCmdDisplay();
}


void MainWindow::on_mods_selection_list_itemChanged(QListWidgetItem *item)
{
    if(item->checkState()) {
        mod_manager_.EnableMod(ui->mods_selection_list->row(item));
    } else {
        mod_manager_.EnableMod(ui->mods_selection_list->row(item), false);
    }
    UpdCmdDisplay();
}

void MainWindow::UpdCmdDisplay() {
    if(ui->text_cmd_display) {
        ui->text_cmd_display->setText(QString::fromStdString(mod_manager_.GetDisplayCmd()));
    }
}

void MainWindow::ChooseIwadFolder() {
    cerr << "choose folder called: ";
    std::string chosen_dir = QFileDialog::getExistingDirectory().toStdString();
    cerr << chosen_dir  << endl;
    mod_manager_.SetIwadDir(std::move(chosen_dir));

    ui->iwad_select_combo->clear();
    auto iwads = mod_manager_.MakeIwadLabels();

    if(iwads.empty()) {
        ui->iwad_select_combo->addItems({"IWAD NOT FOUND"});
    }
    else {
        ui->iwad_select_combo->addItems(iwads);
    }
}

void MainWindow::on_actionChoose_iwads_folder_triggered()
{
    ChooseIwadFolder();
}


void MainWindow::on_apply_btn_pressed()
{
    cerr << "-> Applying mod selection to: " << ui->modset_select_combo->currentIndex();
    mod_manager_.ApplyChanges();
}


void MainWindow::on_launch_game_btn_clicked()
{
    cerr << "-> passing launch command: " << ui->modset_select_combo->currentIndex();
    mod_manager_.LaunchGame();
}
