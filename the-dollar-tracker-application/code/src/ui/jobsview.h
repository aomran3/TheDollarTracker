#pragma once
#include <QDialog>

namespace Ui { class Jobsview; }

class Jobsview : public QDialog {
    Q_OBJECT
public:
    explicit Jobsview(QWidget* parent = nullptr);
    ~Jobsview();
private:
    Ui::Jobsview* ui;
};
