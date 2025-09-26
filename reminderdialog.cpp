#include "reminderdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTime>

ReminderDialog::ReminderDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Hourly Reminder");
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    resize(300, 150);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    messageLabel = new QLabel("It's time for your hourly break!", this);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin: 10px;");

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    snoozeButton = new QPushButton("Snooze (5 min)", this);
    dismissButton = new QPushButton("Dismiss", this);

    buttonLayout->addWidget(snoozeButton);
    buttonLayout->addWidget(dismissButton);

    mainLayout->addWidget(messageLabel);
    mainLayout->addLayout(buttonLayout);

    connect(snoozeButton, &QPushButton::clicked, this, &ReminderDialog::snoozeClicked);
    connect(dismissButton, &QPushButton::clicked, this, &ReminderDialog::dismissClicked);
}

ReminderDialog::~ReminderDialog()
{
}

void ReminderDialog::setMessage(const QString &message)
{
    messageLabel->setText(message);
}
