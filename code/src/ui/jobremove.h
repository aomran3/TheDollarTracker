#pragma once
#include <QDialog>
namespace Ui { class Jobremove; }
class Jobremove : public QDialog {
    Q_OBJECT
public:
    explicit Jobremove(QWidget* parent=nullptr);
    ~Jobremove();
private:
    Ui::Jobremove* ui;
};
