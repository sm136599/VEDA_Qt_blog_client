#include "postlistwidget.h"
#include "ui_postlistwidget.h"
#include "postlistitemwidget.h"
#include <QDebug>
#include <QLabel>


PostListWidget::PostListWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PostListWidget)
{
    ui->setupUi(this);
    qDebug() << "AllPostWidget constructor";
    for (int i = 0; i < 10; i++) ui->scrollAreaWidgetContents->layout()->addWidget(new PostListItemWidget);

    ui->scrollAreaWidgetContents->layout()->addItem(new QSpacerItem(20, 244, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

PostListWidget::~PostListWidget()
{
    delete ui;
}
