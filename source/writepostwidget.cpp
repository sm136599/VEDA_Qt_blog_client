#include "writepostwidget.h"
#include "ui_writepostwidget.h"

WritePostWidget::WritePostWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WritePostWidget)
{
    ui->setupUi(this);

    connect(ui->backButton, &QPushButton::clicked, [this]() {
        emit back();
    });

    connect(ui->uploadButton, &QPushButton::clicked, [this]() {
        if (ui->titleLineEdit->text().isEmpty() || ui->descriptionTextEdit->toPlainText().isEmpty()) {
            return;
        }
        emit uploadPost(ui->titleLineEdit->text(), ui->descriptionTextEdit->toPlainText());
    });
}

WritePostWidget::~WritePostWidget()
{
    delete ui;
}