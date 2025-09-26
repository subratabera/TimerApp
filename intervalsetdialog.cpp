#include "intervalsetdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>

IntervalSetDialog::IntervalSetDialog(int currentInterval, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Set Alert Interval");
    setModal(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *label = new QLabel("Set the interval between alerts (in minutes):", this);
    label->setStyleSheet("font-size: 16px; font-weight: bold;");

    intervalSpinBox = new QSpinBox(this);
    intervalSpinBox->setRange(1, 1440); // 1 minute to 24 hours
    intervalSpinBox->setValue(currentInterval);
    intervalSpinBox->setSuffix(" minutes");
    intervalSpinBox->setStyleSheet("font-size: 16px;");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Cancel", this);

    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addWidget(label);
    mainLayout->addWidget(intervalSpinBox);
    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &IntervalSetDialog::acceptInterval);
    connect(cancelButton, &QPushButton::clicked, this, &IntervalSetDialog::rejectInterval);
}

IntervalSetDialog::~IntervalSetDialog()
{
}

int IntervalSetDialog::selectedInterval() const
{
    return intervalSpinBox->value();
}

void IntervalSetDialog::setValue(int value)
{
    intervalSpinBox->setValue(value);
}

void IntervalSetDialog::acceptInterval()
{
    accept();
}

void IntervalSetDialog::rejectInterval()
{
    reject();
}
