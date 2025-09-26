#include "timesetdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimeEdit>
#include <QPushButton>
#include <QLabel>

TimeSetDialog::TimeSetDialog(const QTime &currentTime, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Set Alert Time");
    setModal(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *label = new QLabel("Set the time for the next alert:", this);
    label->setStyleSheet("font-size: 16px; font-weight: bold;");

    timeEdit = new QTimeEdit(this);
    timeEdit->setTime(currentTime);
    timeEdit->setDisplayFormat("hh:mm");
    timeEdit->setStyleSheet("font-size: 16px;");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Cancel", this);

    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addWidget(label);
    mainLayout->addWidget(timeEdit);
    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &TimeSetDialog::acceptTime);
    connect(cancelButton, &QPushButton::clicked, this, &TimeSetDialog::rejectTime);
}

TimeSetDialog::~TimeSetDialog()
{
}

QTime TimeSetDialog::selectedTime() const
{
    return timeEdit->time();
}

void TimeSetDialog::setTime(const QTime &time)
{
    timeEdit->setTime(time);
}

void TimeSetDialog::acceptTime()
{
    accept();
}

void TimeSetDialog::rejectTime()
{
    reject();
}
