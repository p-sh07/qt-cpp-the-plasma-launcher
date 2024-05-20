#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

#include <iostream>

//============== Constructor ==============//
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,  mod_manager_(__log)
{
    __log << "MainWindow constructor Started";
    __log << "App working Path: "+fs::current_path().string();
    __log << "App current QDir: "+QDir::currentPath().toStdString();

    //TODO: Ask for folders during first launch!

    try {
        ui->setupUi(this);

        InitModsetCombo();
        InitIwadCombo();
        InitModList();
        UpdCmdDisplay();
    }
    catch(std::exception& ex) {
        __log << "ERROR: " + std::string(ex.what());
    }

    __log << "MainWindow construction Finished\n";

    //TODO: Setup window if first launch
    // if(mod_manager_.IsFirstLaunch()) {
    //     mod_manager_.SetWorkingDir(ChooseFolder("CHOOSE FOLDER FOR MOD MANAGER FILES e.g. \"Documents/My Games...\" "));
    // }
}

MainWindow::~MainWindow()
{
    __log << "MainWindow DESTRUCTOR called\n";

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
    cerr << "-> passing launch command: " << ui->modset_select_combo->currentIndex() << '\n';
    mod_manager_.LaunchGame();
}
void MainWindow::on_apply_btn_clicked()
{
    cerr << "-> Applying mod selection to: " << ui->modset_select_combo->currentIndex();
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
        cerr << "MW-error: Mod selection list inaccessible!\n";
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
        cerr << "MW-error: Modset combo inaccessible!\n";
        return;
    }
    ui->modset_select_combo->clear();
    ui->modset_select_combo->addItems(mod_manager_.GetModSetLabels());

}
void MainWindow::InitIwadCombo() {
    if(!ui->iwad_select_combo) {
        cerr << "MW-error: Iwad combo inaccessible!\n";
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
    cerr << "choose folder called: ";
    std::string chosen_dir = QFileDialog::getExistingDirectory(this, "Choose IWAD").toStdString();
    cerr << chosen_dir  << endl;
    mod_manager_.SetIwadDir(std::move(chosen_dir));
}

void MainWindow::UpdIwadCombo() {
    ui->iwad_select_combo->setCurrentIndex(mod_manager_.GetIwadIndex());
}
/*  Example, aux code:
 *
 *     //makes stuff semi-transparent:
    //setWindowOpacity(0.5);
    // if (ui->mods_selection_list) {
    //     ui->mods_selection_list->setWindowOpacity(0.5);
    // }

    // QStringListIterator it(mod_manager_.MakeModLabels());
    // while (it.hasNext())
    // {
    //     QListWidgetItem *listItem = new QListWidgetItem(it.next(),ui->mods_selection_list);
    //     listItem->setCheckState(Qt::Unchecked);
    //     ui->mods_selection_list->addItem(listItem);
    // }

    // auto iwads = mod_manager_.MakeIwadLabels();

    // if(iwads.empty()) {
    //     ui->iwad_select_combo->addItems({"IWAD NOT FOUND"});
    //     //TODO: open wad dir select window
    //     ChooseIwadFolder();
    // }
    // else {
    //     ui->iwad_select_combo->addItems(iwads);
    // }
 *
 *
 */

