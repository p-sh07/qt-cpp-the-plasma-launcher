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

    void on_launch_game_btn_clicked();

    void on_mods_selection_list_itemSelectionChanged();

private:
    Ui::MainWindow *ui;
    gzdml_mac_qt mod_manager_;
};
#endif // MAINWINDOW_H
