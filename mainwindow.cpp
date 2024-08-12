#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

#include <iostream>

//============== Constructor ==============//
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , working_folder_(QDir::currentPath().toStdString())
    , log_file_full_(working_folder_ / _log::LOG_FILE_NAME)
    , __logger(std::make_shared<_log::FileLogger>(log_file_full_))
    , mod_manager_(working_folder_ / dgc::DEFAULT_DGC_FILE)
{
    __logger << "MainWindow constructor Started";
    __logger << "App working Path: "+fs::current_path().string();
    __logger << "App current QDir: "+QDir::currentPath().toStdString();

    try {
        ui->setupUi(this);

        InitModsetCombo();
        InitIwadCombo();
        InitModList();
        UpdCmdDisplay();
    }
    catch(std::exception& ex) {
        __logger << "ERROR: " + std::string(ex.what());
    }

    __logger << "MainWindow construction Finished\n";

    //TODO: Setup window if first launch
    // if(mod_manager_.IsFirstLaunch()) {
    //     mod_manager_.SetWorkingDir(ChooseFolder("CHOOSE FOLDER FOR MOD MANAGER FILES e.g. \"Documents/My Games...\" "));
    // }
}

MainWindow::~MainWindow()
{
    __logger << "MainWindow DESTRUCTOR called\n";

    delete ui;
}
//TODO: save and reset buttons !

//============== Private Slots (Control Methods) ==============//
//---- Iwad Select ----
void MainWindow::on_iwad_select_combo_currentIndexChanged(int index) {
    mod_manager_.SetIwad(index);
    UpdCmdDisplay();
}


void MainWindow::on_actionChoose_iwads_folder_triggered()
{
    ChooseIwadFolder();
}

//---- Mod/ModSet Select ----
void MainWindow::on_mods_selection_list_itemChanged(QListWidgetItem *item)
{
    mod_manager_.SetMod(ui->mods_selection_list->row(item), item->checkState());

    UpdCmdDisplay();
}

void MainWindow::on_modset_select_combo_currentIndexChanged(int index)
{
    mod_manager_.SetModSet(static_cast<size_t>(index));

    //Update displays:
    UpdIwadCombo();
    UpdModList();
    UpdCmdDisplay();
}

//---- Buttons ----

void MainWindow::on_launch_game_btn_clicked()
{
    std::cerr << "-> passing launch command: " << ui->modset_select_combo->currentIndex() << '\n';
    mod_manager_.LaunchGame();
}
void MainWindow::on_apply_btn_clicked()
{
    std::cerr << "-> Applying mod selection to: " << ui->modset_select_combo->currentIndex();
    mod_manager_.ApplyChanges();

    UpdCmdDisplay();
}


void MainWindow::on_set_folders_btn_clicked()
{
    mod_manager_.SetWorkingDir(ChooseFolder("Choose Working Folder (for mods, wads, etc.)"));
}

void MainWindow::on_save_new_btn_clicked()
{
    //TODO
    mod_manager_.AddNewModSet();
}

//============== Private Helper Methods ==============//
void MainWindow::InitModList() {
    if(!ui->mods_selection_list) {
        std::cerr << "MW-error: Mod selection list inaccessible!\n";
        return;
    }

    ui->mods_selection_list->clear();
    ui->mods_selection_list->addItems(mod_manager_.MakeModListLabels());

    for(int i = 0; i < ui->mods_selection_list->count(); ++i) {
        ui->mods_selection_list->item(i)->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::UpdModList() {
    //TODO: if mods dir changed, Init with new dir:
    InitModList();
    //Uncheck all
    for(int i = 0; i < ui->mods_selection_list->count(); ++i) {
        ui->mods_selection_list->item(i)->setCheckState(Qt::Unchecked);
    }
    //Check all selected
    //if (auto it = std::ranges::find(cfg_.all_mod_names, mod); it != cfg_.all_mod_names.end()) {
    for(const size_t mod : mod_manager_.GetEnabledMods()) {
        ui->mods_selection_list->item(mod)->setCheckState(Qt::Checked);
    }
}

void MainWindow::InitModsetCombo() {
    if(!ui->modset_select_combo) {
        __logger << "MW-error: Modset combo inaccessible!\n";
        return;
    }
    ui->modset_select_combo->clear();
    ui->modset_select_combo->addItems(mod_manager_.GetModSetLabels());
}

void MainWindow::InitIwadCombo() {
    if(!ui->iwad_select_combo) {
        std::cerr << "MW-error: Iwad combo inaccessible!\n";
        return;
    }

    ui->iwad_select_combo->clear();
    ui->iwad_select_combo->addItems(mod_manager_.MakeIwadLabels());
}

void MainWindow::UpdCmdDisplay() {
    if(ui->text_cmd_display) {
        ui->text_cmd_display->setText(QString::fromStdString(mod_manager_.GetDisplayCmd()));
    }
}

fs::path MainWindow::ChooseFolder(const QString& caption) {
    return QFileDialog::getExistingDirectory(this, caption).toStdString();
}

void MainWindow::ChooseIwadFolder() {
    std::cerr << "choose folder called: ";
    std::string chosen_dir = QFileDialog::getExistingDirectory(this, "Choose IWAD").toStdString();
    std::cerr << chosen_dir  << std::endl;
    mod_manager_.SetIwadDir(std::move(chosen_dir));
}

void MainWindow::UpdIwadCombo() {
    ui->iwad_select_combo->setCurrentIndex(mod_manager_.GetIwadIndex());
}

