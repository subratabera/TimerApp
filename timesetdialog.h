#ifndef TIMESETDIALOG_H
#define TIMESETDIALOG_H

#include <QDialog>
#include <QTime>

class QTimeEdit;
class QPushButton;

class TimeSetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TimeSetDialog(const QTime &currentTime, QWidget *parent = nullptr);
    ~TimeSetDialog();

    QTime selectedTime() const;
    void setTime(const QTime &time);  // Add this method

private slots:
    void acceptTime();
    void rejectTime();

private:
    QTimeEdit *timeEdit;
    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif // TIMESETDIALOG_H
