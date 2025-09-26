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

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Set layout margins and spacing
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    messageLabel = new QLabel("It's time for your hourly break!", this);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setStyleSheet("font-size: 20px; font-weight: bold;");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    snoozeButton = new QPushButton("Snooze (5 min)", this);
    snoozeButton->setStyleSheet("font-size: 16px; padding: 10px;");
    snoozeButton->setMinimumHeight(50);

    dismissButton = new QPushButton("Dismiss", this);
    dismissButton->setStyleSheet("font-size: 16px; padding: 10px;");
    dismissButton->setMinimumHeight(50);

    buttonLayout->addWidget(snoozeButton);
    buttonLayout->addWidget(dismissButton);

    mainLayout->addWidget(messageLabel);
    mainLayout->addLayout(buttonLayout);

    connect(snoozeButton, &QPushButton::clicked, this, &ReminderDialog::snoozeClicked);
    connect(dismissButton, &QPushButton::clicked, this, &ReminderDialog::dismissClicked);

    // Adjust size to fit contents
    adjustSize();
    setFixedSize(size());
}

ReminderDialog::~ReminderDialog()
{
}

void ReminderDialog::setMessage(const QString &message)
{
    messageLabel->setText(message);
}
