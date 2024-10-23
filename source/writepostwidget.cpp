#include "writepostwidget.h"
#include "ui_writepostwidget.h"

WritePostWidget::WritePostWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WritePostWidget)
{
    ui->setupUi(this);
}

WritePostWidget::~WritePostWidget()
{
    delete ui;
}