#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE



namespace ST {
    enum TraversalType { DFS, BFS };
}

class Widget : public QWidget {
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void changeAddPathButtonStyle();
    void traversal(ST::TraversalType type);
private slots:
    void onTimeout();
    void paintWidgetMouseEvent();
    void on_btnOpen_clicked();
    void on_btnSave_clicked();
    void on_btnAddCity_clicked();
    void on_btnAddPath_clicked();
    void on_btnClearGraph_clicked();
    void on_ledCityName_textChanged(const QString &arg1);
    void on_ledX_textChanged(const QString &arg1);
    void on_ledY_textChanged(const QString &arg1);
    void on_ledEBegin_editingFinished();
    void on_ledEEnd_editingFinished();
    void on_ledDistance_textChanged(const QString &arg1);
    void on_btnDFS_clicked();
    void on_btnBFS_clicked();
    void on_ledSource_editingFinished();
    void on_btnClear_clicked();
    void on_ledEBegin_textChanged(const QString &arg1);
    void on_ledEEnd_textChanged(const QString &arg1);
    void on_btnExecute_clicked();
    void on_ledSBegin_editingFinished();
    void on_chkToAllCity_stateChanged(int arg1);
    void on_spnSetR_valueChanged(int arg1);
    void on_ledSEnd_editingFinished();
    void on_spnSetSpeed_valueChanged(int arg1);

private:
    Ui::Widget *ui;
    QTimer *tvsTimer;
};
#endif // WIDGET_H
