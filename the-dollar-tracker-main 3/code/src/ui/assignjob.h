#pragma once
#include <QDialog>
namespace Ui { class Assignjob; }
class Assignjob : public QDialog {
    Q_OBJECT
public:
    explicit Assignjob(QWidget* parent=nullptr);
    ~Assignjob();
private:
    Ui::Assignjob* ui;
};
