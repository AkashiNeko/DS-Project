#include "include/widget.h"
#include "ui_widget.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QValidator>
#include <QByteArray>

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);
    this->setWindowTitle("交通咨询系统");
    ui->btnAddCity->setDisabled(true);
    ui->btnAddPath->setDisabled(true);
    ui->btnClear->setDisabled(true);
    ui->ledX->setValidator(new QIntValidator(ui->ledX));
    ui->ledY->setValidator(new QIntValidator(ui->ledY));
    ui->ledEBegin->setValidator(new QIntValidator(ui->ledEBegin));
    ui->ledEEnd->setValidator(new QIntValidator(ui->ledEEnd));
    ui->ledDistance->setValidator(new QIntValidator(ui->ledDistance));
    ui->ledSource->setValidator(new QIntValidator(ui->ledSource));
    ui->ledSBegin->setValidator(new QIntValidator(ui->ledEBegin));
    ui->ledSEnd->setValidator(new QIntValidator(ui->ledEEnd));
    tvsTimer = new QTimer(this);
    tvsTimer->setInterval(280);
    connect(ui->paintWidget, SIGNAL(PaintWidgetMouseClick()), this, SLOT(paintWidgetMouseEvent()));
    connect(tvsTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

Widget::~Widget() {
    delete ui;
}

void Widget::changeAddPathButtonStyle() {
    if (ui->ledEBegin->text().isEmpty() || ui->ledEEnd->text().isEmpty() || ui->ledDistance->text().isEmpty())
        ui->btnAddPath->setDisabled(true);
    else
        ui->btnAddPath->setDisabled(false);
    size_t begin = ui->ledEBegin->text().toUInt();
    size_t end = ui->ledEEnd->text().toUInt();
    int distance = ui->ledDistance->text().toUInt();
    if (ui->paintWidget->g.getLine(begin, end) == distance) {
        ui->btnAddPath->setText("删除路径");
    }
    else if (ui->paintWidget->g.getLine(begin, end) != INT_MAX){
        ui->btnAddPath->setText("修改路径");
    }
    else {
        ui->btnAddPath->setText("添加路径");
    }
}

void Widget::traversal(ST::TraversalType type) {
    if (ui->ledSource->text().isEmpty()) {
        QMessageBox::critical(this, "遍历", "请输入源点（遍历起点）");
        return;
    }
    auto source = ui->ledSource->text().toUInt();
    ui->paintWidget->tvsResult = (type == ST::DFS) ? ui->paintWidget->g.dfs(source) : ui->paintWidget->g.bfs(source);
    ui->paintWidget->tvsStep = 0;
    tvsTimer->start();
    ui->btnClear->setDisabled(false);
}

void Widget::onTimeout() {
    auto& step = ui->paintWidget->tvsStep;
    auto sz = ui->paintWidget->tvsResult.size();
    if (step++ < sz) {
        ui->paintWidget->drawGraph();
    }
    if (step == sz) {
        tvsTimer->stop();
    }
}

void Widget::paintWidgetMouseEvent() {
    if (ui->paintWidget->tempCity) {
        ui->btnAddCity->setDisabled(false);
        ui->btnAddCity->setText("添加城市");
        ui->ledX->setText(QString::number(ui->paintWidget->_x));
        ui->ledY->setText(QString::number(ui->paintWidget->_y));
        if (ui->ledCityName->text().isSimpleText())
            ui->ledCityName->setText("city");
        ui->ledEBegin->setText("");
        ui->ledSBegin->setText("");
        ui->ledSource->setText("");
    }
    else if (ui->paintWidget->select != (size_t)-1) {
        ui->btnAddCity->setDisabled(false);
        ui->btnAddCity->setText("删除城市");
        auto t = QString::number(ui->paintWidget->select);
        ui->ledX->setText(QString::number(ui->paintWidget->g[ui->paintWidget->select].coordinate.x));
        ui->ledY->setText(QString::number(ui->paintWidget->g[ui->paintWidget->select].coordinate.y));
        ui->ledCityName->setText(QString::fromStdString(ui->paintWidget->g[ui->paintWidget->select].cityName));
        ui->ledEBegin->setText(t);
        ui->ledSBegin->setText(t);
        ui->ledSource->setText(t);
    }
    else {
        ui->btnAddCity->setDisabled(true);
        ui->btnAddCity->setText("添加城市");
        ui->ledX->setText("");
        ui->ledY->setText("");
        ui->ledCityName->setText("");
        ui->ledEBegin->setText("");
        ui->ledSBegin->setText("");
        ui->ledSource->setText("");
    }
}

void Widget::on_btnOpen_clicked() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("打开Graph文件"), "./csv", tr("graph (*.csv)"));
    QByteArray bytes = filePath.toLocal8Bit();
    if (!filePath.isEmpty()) {

        auto res = ui->paintWidget->g.readFromCSV(bytes.data());

        if (!res) {
            QMessageBox::critical(this, "打开文件", "打开文件 " + filePath + " 失败，请确认这是一个有效的Graph文件，以及没有被其他程序占用");
            return;
        }
        ui->paintWidget->tempCity = false;
        ui->paintWidget->select = (size_t)-1;
        ui->ledX->setText("");
        ui->ledY->setText("");
        ui->ledCityName->setText("");
        ui->paintWidget->shortestPathEnd = (size_t)-1;
        tvsTimer->stop();
        ui->paintWidget->tvsStep = 0;
        ui->btnClear->setDisabled(true);
        ui->paintWidget->drawGraph();
    }
}

void Widget::on_btnSave_clicked() {
    QString filePath = QFileDialog::getSaveFileName(this, tr("保存Graph文件"), "./csv/", "Graph (*.csv)");
    QByteArray bytes = filePath.toLocal8Bit();
    if (!filePath.isEmpty()) {
        auto res = ui->paintWidget->g.saveToCSV(bytes.data());
        if (!res) QMessageBox::critical(this, "保存到文件", "保存到 " + filePath + " 失败，请检查该文件是否被占用");
    }
}

void Widget::on_btnAddCity_clicked() {
    if (ui->paintWidget->select != (size_t)-1) {
        ui->btnAddCity->setDisabled(true);
        ui->btnAddCity->setText("添加城市");
        ui->paintWidget->g.delCity(ui->paintWidget->select);
        ui->paintWidget->select = (size_t)-1;
        ui->ledEBegin->setText("");
        ui->ledSBegin->setText("");
        ui->ledSource->setText("");
        tvsTimer->stop();
        ui->paintWidget->tvsStep = 0;
        ui->btnClear->setDisabled(true);
        ui->paintWidget->shortestPathEnd = (size_t)-1;
        ui->paintWidget->drawGraph();
    }
    else if (!ui->paintWidget->tempCity) {
        QMessageBox::critical(this, "添加城市", "请用鼠标点击图中空白区域添加城市");
    }
    else if (ui->ledCityName->text().isEmpty()) {
        QMessageBox::critical(this, "添加城市", "请输入城市名！");
    }
    else if (ui->ledX->text().isEmpty() || ui->ledY->text().isEmpty()) {
        QMessageBox::critical(this, "添加城市", "请输入坐标！");
    }
    else {
        ui->btnAddCity->setDisabled(true);
        ui->btnAddCity->setText("添加城市");
        ui->paintWidget->g.addCity(
            ui->paintWidget->g.minVacancy(),                        // city number
            {},                                                     // neighbors
            ui->ledCityName->text().toStdString(),                  // city name
            {ui->ledX->text().toInt(), ui->ledY->text().toInt()}    // coordinate
        );
        ui->paintWidget->tempCity = false;
        tvsTimer->stop();
        ui->paintWidget->tvsStep = 0;
        ui->btnClear->setDisabled(true);
        ui->paintWidget->shortestPathEnd = (size_t)-1;
        ui->paintWidget->drawGraph();
    }
}

void Widget::on_btnAddPath_clicked() {
    if (ui->ledEBegin->text().isEmpty()) {
        QMessageBox::critical(this, "添加路径", "请输入路径起点");
        return;
    }
    if (ui->ledEEnd->text().isEmpty()) {
        QMessageBox::critical(this, "添加路径", "请输入路径终点");
        return;
    }
    if (ui->ledDistance->text().isEmpty()) {
        QMessageBox::critical(this, "添加路径", "请输入路径长度");
        return;
    }
    size_t begin = ui->ledEBegin->text().toUInt();
    size_t end = ui->ledEEnd->text().toUInt();
    if (begin == end) {
        QMessageBox::critical(this, "添加路径", "起点不能和终点相同！");
        return;
    }
    int distance = ui->ledDistance->text().toInt();
    if (ui->paintWidget->g.getLine(begin, end) == distance) {
        ui->paintWidget->g.removeLine(begin, end);
    }
    else {
        ui->paintWidget->g.setLine(begin, end, distance);
    }
    changeAddPathButtonStyle();
    tvsTimer->stop();
    ui->paintWidget->tvsStep = 0;
    ui->btnClear->setDisabled(true);
    ui->paintWidget->shortestPathEnd = (size_t)-1;
    ui->paintWidget->drawGraph();
}

void Widget::on_btnClearGraph_clicked() {
    ui->paintWidget->g.clear();
    ui->paintWidget->tempCity = false;
    ui->paintWidget->select = (size_t)-1;
    ui->btnAddCity->setDisabled(true);
    ui->btnAddCity->setText("添加城市");
    ui->ledX->setText("");
    ui->ledY->setText("");
    ui->ledCityName->setText("");
    tvsTimer->stop();
    ui->paintWidget->tvsStep = 0;
    ui->btnClear->setDisabled(true);
    ui->paintWidget->shortestPathEnd = (size_t)-1;
    ui->paintWidget->drawGraph();
}

void Widget::on_ledCityName_textChanged(const QString &arg1) {
    if (ui->paintWidget->select != (size_t)-1) {
        auto& cur = ui->paintWidget->g[ui->paintWidget->select].cityName;
        if (arg1.isEmpty())
            cur = "unnamed";
        else
            cur = ui->ledCityName->text().toStdString();
        ui->paintWidget->drawGraph();
    }
}

void Widget::on_ledX_textChanged(const QString &arg1) {
    if (ui->paintWidget->select != (size_t)-1) {
        auto& cur = ui->paintWidget->g[ui->paintWidget->select].coordinate.x;
        if (!arg1.isEmpty()) {
            cur = ui->ledX->text().toInt();
            ui->paintWidget->drawGraph();
        }
    }
}

void Widget::on_ledY_textChanged(const QString &arg1) {
    if (ui->paintWidget->select != (size_t)-1) {
        auto& cur = ui->paintWidget->g[ui->paintWidget->select].coordinate.y;
        if (!arg1.isEmpty()) {
            cur = ui->ledY->text().toInt();
            ui->paintWidget->drawGraph();
        }
    }
}

void Widget::on_ledEBegin_editingFinished() {
    size_t begin = ui->ledEBegin->text().toUInt();
    if (ui->paintWidget->g.emptyCity(begin)) {
        QMessageBox::critical(this, "添加路径", "起点城市不存在！请检查是否正确输入");
        ui->ledEBegin->setText("");
        return;
    }
    changeAddPathButtonStyle();
}

void Widget::on_ledEEnd_editingFinished() {
    size_t end = ui->ledEEnd->text().toUInt();
    if (ui->paintWidget->g.emptyCity(end)) {
        QMessageBox::critical(this, "添加路径", "终点城市不存在！请检查是否正确输入");
        ui->ledEEnd->setText("");
        return;
    }
    changeAddPathButtonStyle();
}

void Widget::on_ledEBegin_textChanged(const QString &arg1) {
    changeAddPathButtonStyle();
}

void Widget::on_ledEEnd_textChanged(const QString &arg1) {
    changeAddPathButtonStyle();
}

void Widget::on_ledDistance_textChanged(const QString &arg1) {
    changeAddPathButtonStyle();
}

void Widget::on_btnDFS_clicked() {
    traversal(ST::DFS);
}

void Widget::on_btnBFS_clicked() {
    traversal(ST::BFS);
}

void Widget::on_ledSource_editingFinished() {
    if (!ui->ledSource->text().isEmpty() && ui->paintWidget->g.emptyCity(ui->ledSource->text().toUInt())) {
        QMessageBox::critical(this, "添加路径", "起点城市不存在！请检查是否正确输入");
        ui->ledSource->setText("");
        return;
    }
}

void Widget::on_btnClear_clicked() {
    tvsTimer->stop();
    ui->paintWidget->tvsStep = 0;
    ui->btnClear->setDisabled(true);
    ui->paintWidget->drawGraph();
}

void Widget::on_btnExecute_clicked() {
    if (ui->ledSBegin->text().isEmpty()) {
        QMessageBox::critical(this, "最短路径", "请输入起点");
        return;
    }
    if (!ui->chkToAllCity->checkState() && ui->ledSEnd->text().isEmpty()) {
        QMessageBox::critical(this, "最短路径", "请输入终点");
        return;
    }
    size_t begin = ui->ledSBegin->text().toUInt();
    tvsTimer->stop();
    ui->paintWidget->tvsStep = 0;
    ui->btnClear->setDisabled(true);
    ui->paintWidget->g.dijkstra(begin);
    if (ui->chkToAllCity->checkState()) {
        auto path = ui->paintWidget->g.getDijkResultPath();
        auto distance = ui->paintWidget->g.getDijkResultDistance();
        auto sz = path.size();
        if (ui->paintWidget->g[begin].neighbors.empty()) {
            QMessageBox::warning(this, "最短路径", "选中的城市没有邻居");
            return;
        }
        QString s;
        for (size_t i = 0; i < sz; ++i) {
            size_t end = *(path[i].rbegin());
            if (begin != end) {
                s += "到 [" + QString::number(end) + "]" + QString::fromStdString(ui->paintWidget->g[end].cityName);
                s += "    总长度: " + QString::number(distance[i]) + "    路径: [" + QString::number(begin) + "]";
                for (auto e : path[i]) {
                    if (e == begin) continue;
                    s += "→[" + QString::number(e) + "]";
                }
                s += '\n';
            }
        }
        QMessageBox::about(this, "从源点 [" + QString::number(begin) + "]"
            + QString::fromStdString(ui->paintWidget->g[begin].cityName) + " 到所有城市的最短路径", s);
    }
    else {
        auto end = ui->ledSEnd->text().toUInt();
        if (begin == end) {
            QMessageBox::critical(this, "？？？", "自己到自己的最短路径是0");
            return;
        }
        if (ui->paintWidget->g.getShortestDistance(end) == INT_MAX) {
            QMessageBox::critical(this, "最短路径", "[" + QString::number(begin) + "] 到 ["
                + QString::number(end) + "] 不连通，不存在最短路径");
            return;
        }
        if (end == ui->paintWidget->shortestPathEnd)
            ui->paintWidget->shortestPathEnd = (size_t)-1;
        else
            ui->paintWidget->shortestPathEnd = end;
        ui->paintWidget->drawGraph();
    }
}

void Widget::on_ledSBegin_editingFinished() {
    if (!ui->ledSBegin->text().isEmpty() && ui->paintWidget->g.emptyCity(ui->ledSBegin->text().toUInt())) {
        QMessageBox::critical(this, "最短路径", "起点城市不存在！请检查是否正确输入");
        ui->ledSBegin->setText("");
        return;
    }
}

void Widget::on_chkToAllCity_stateChanged(int arg1) {
    if (arg1)
        ui->ledSEnd->setDisabled(true);
    else
        ui->ledSEnd->setDisabled(false);
}

void Widget::on_spnSetR_valueChanged(int arg1) {
    ui->paintWidget->r = arg1;
    ui->paintWidget->drawGraph();
}

void Widget::on_ledSEnd_editingFinished() {
    if (!ui->ledSEnd->text().isEmpty() && ui->paintWidget->g.emptyCity(ui->ledSEnd->text().toUInt())) {
        QMessageBox::critical(this, "最短路径", "终点城市不存在！请检查是否正确输入");
        ui->ledSEnd->setText("");
        return;
    }
}

void Widget::on_spnSetSpeed_valueChanged(int arg1) {
    tvsTimer->setInterval(500 - (50 * arg1));
}
