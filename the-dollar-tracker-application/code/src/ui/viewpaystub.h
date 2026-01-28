#pragma once
#include <QDialog>
namespace Ui { class Viewpaystub; }
class Viewpaystub : public QDialog {
    Q_OBJECT
public:
    explicit Viewpaystub(QWidget* parent=nullptr);
    ~Viewpaystub();
private:
    Ui::Viewpaystub* ui;
};
