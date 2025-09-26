#ifndef INTERVALSETDIALOG_H
#define INTERVALSETDIALOG_H

#include <QDialog>

class QSpinBox;
class QPushButton;

class IntervalSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IntervalSetDialog(int currentInterval, QWidget *parent = nullptr);
    ~IntervalSetDialog();

    int selectedInterval() const;
    void setValue(int value);  // Add this method

private slots:
    void acceptInterval();
    void rejectInterval();

private:
    QSpinBox *intervalSpinBox;
    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif // INTERVALSETDIALOG_H
