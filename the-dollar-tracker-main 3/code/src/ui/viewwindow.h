#pragma once
#include <QDialog>
namespace Ui { class Viewwindow; }
class Viewwindow : public QDialog {
    Q_OBJECT
public:
    explicit Viewwindow(QWidget* parent=nullptr);
    ~Viewwindow();
private:
    Ui::Viewwindow* ui;
};
