#include "postwidget.h"
#include "ui_postwidget.h"
#include "commentwidget.h"

PostWidget::PostWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PostWidget)
{
    ui->setupUi(this);
    for (int i = 0; i < 5; i++)
        ui->scrollAreaWidgetContents->layout()->addWidget(new CommentWidget(ui->scrollAreaWidgetContents));
}

PostWidget::~PostWidget()
{
    delete ui;
}