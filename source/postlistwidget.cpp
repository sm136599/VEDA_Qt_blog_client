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
    ui->listWidget->setLayout(new QVBoxLayout());
}

PostListWidget::~PostListWidget()
{
    delete ui;
}

// public
void PostListWidget::addPostListItem(PostListItemWidget *postListItemWidget) {
    ui->listWidget->layout()->addWidget(postListItemWidget);
}