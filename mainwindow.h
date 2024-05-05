#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

#include "gzdml.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_iwad_select_combo_currentIndexChanged(int index);
    void on_modset_select_combo_currentIndexChanged(int index);

    void on_mods_selection_list_itemChanged(QListWidgetItem *item);

    void ChooseIwadFolder();

    void on_actionChoose_iwads_folder_triggered();

    void on_apply_btn_pressed();
    void on_launch_game_btn_clicked();

private:
    Ui::MainWindow *ui;
    gzdml_mac_qt mod_manager_;

    void UpdCmdDisplay();
};
#endif // MAINWINDOW_H
