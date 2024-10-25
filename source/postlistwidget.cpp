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
    for (PostListItemWidget* postListItemWidget : postListItemWidgetList) {
        postListItemWidget->deleteLater();
    }
}

// public
void PostListWidget::addPostListItem(PostListItemWidget *postListItemWidget) {
    postListItemWidgetList.append(postListItemWidget);
    ui->listWidget->layout()->addWidget(postListItemWidget);
    connect(postListItemWidget, &PostListItemWidget::clicked, this, [this](int postId) {
        emit postClicked(postId);
    });
}

void PostListWidget::enableClickEvent() {
    for (PostListItemWidget* postListItemWidget : postListItemWidgetList) {
        postListItemWidget->setClickable(true);
    }
}

void PostListWidget::disableClickEvent() {
    for (PostListItemWidget* postListItemWidget : postListItemWidgetList) {
        postListItemWidget->setClickable(false);
    }
}