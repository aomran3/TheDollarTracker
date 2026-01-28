#pragma once
#include <QDialog>
namespace Ui { class Dialog2; }
class Dialog2 : public QDialog {
    Q_OBJECT
public:
    explicit Dialog2(QWidget* parent=nullptr);
    ~Dialog2();
private:
    Ui::Dialog2* ui;
};
