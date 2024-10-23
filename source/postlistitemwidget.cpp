#include "postlistitemwidget.h"
#include "ui_postlistitemwidget.h"
#include <QDebug>
#include <QLabel>
#include <QMouseEvent>

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
// public
void PostListItemWidget::setClickable(bool clickable) {
    if (clickable) {
        setCursor(Qt::PointingHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
    this->clickable = clickable;
}

// protected
void PostListItemWidget::mousePressEvent(QMouseEvent *event) {
    if (clickable && event->button() == Qt::LeftButton) {
        qDebug() << ui->postIdLabel->text() << "번 게시물 클릭";
        emit clicked(ui->postIdLabel->text().toInt());
    }
    QWidget::mousePressEvent(event);
}
