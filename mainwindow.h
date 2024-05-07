#pragma once

#include <QMainWindow>
#include <QListWidgetItem>

#include "gzdml.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

//================================================================//
//================ Main Window ================//
class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
//============== Controls ==============//
private slots:
//---- Iwad Select ----
    void on_iwad_select_combo_currentIndexChanged(int index);
    void on_actionChoose_iwads_folder_triggered();

//---- Mod/ModSet Select ----
    void on_mods_selection_list_itemChanged(QListWidgetItem *item);
    void on_modset_select_combo_currentIndexChanged(int index);

//---- Buttonst ----
    void on_launch_game_btn_clicked();
    void on_apply_btn_clicked();

    void on_save_new_btn_clicked();

    void on_set_folders_btn_clicked();

    void on_reset_btn_clicked();

    void on_updmodlist_btn_clicked();

private:
//---- Params ----
    Ui::MainWindow *ui;
    MacGzdml mod_manager_;

//---- Helper Functions ----
    void InitModList();
    void UpdModList();
    void InitModsetCombo();
    void InitIwadCombo();
    void UpdIwadCombo();
    void UpdCmdDisplay();
    void ChooseIwadFolder();
};
//+++++++++++++++++++ Eof MainWindow Class +++++++++++++++++++//
//==============================================================//
