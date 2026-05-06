#pragma once
#include <QDialog>
namespace Ui { class Schdule; }
class Schdule : public QDialog {
    Q_OBJECT
public:
    explicit Schdule(QWidget* parent=nullptr);
    ~Schdule();
private:
    Ui::Schdule* ui;
};
