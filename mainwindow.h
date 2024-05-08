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
    void on_set_folders_btn_clicked();

private:
//---- Params ----
    _log::FileLogger __log{_log::LOG_FILE_NAME};

    Ui::MainWindow *ui;

#ifdef WIN_BUILD
    WinGzdml mod_manager_;
#elif defined(MAC_BUILD)
    MacGzdml mod_manager_;
#endif

//---- Helper Functions ----
    void InitModList();
    void UpdModList();
    void InitModsetCombo();
    void InitIwadCombo();
    void UpdIwadCombo();
    void UpdCmdDisplay();
    void ChooseIwadFolder();
    fs::path ChooseFolder(const QString& caption);
};
//+++++++++++++++++++ Eof MainWindow Class +++++++++++++++++++//
//==============================================================//
