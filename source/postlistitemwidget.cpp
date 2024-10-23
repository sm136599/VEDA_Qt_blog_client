#include "postlistitemwidget.h"
#include "ui_postlistitemwidget.h"
#include <QDebug>
#include <QLabel>

PostListItemWidget::PostListItemWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PostListItemWidget)
{
    ui->setupUi(this);
}

PostListItemWidget::PostListItemWidget(const int &id, const QString &title, const QString &writer, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PostListItemWidget)
{
    ui->setupUi(this);
    ui->postIdLabel->setText(tr("%1").arg(id));
    ui->postTitleLabel->setText(title);
    ui->postWriterLabel->setText(writer);

}

PostListItemWidget::~PostListItemWidget()
{
    delete ui;
}

