#include "commentwidget.h"
#include "ui_commentwidget.h"

CommentWidget::CommentWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommentWidget)
{
    ui->setupUi(this);
}

CommentWidget::CommentWidget(Comment comment, QString user, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommentWidget)
{
    ui->setupUi(this);
    this->isEditing = false;
    this->comment = comment;
    ui->writerLabel->setText(comment.writer);
    ui->commentTextEdit->setText(comment.description);

    if (user == comment.writer) {
        ui->editButton->show();
        ui->deleteButton->show();
    } else if (user == "admin") {
        ui->editButton->hide();
    } else {
        ui->editButton->hide();
        ui->deleteButton->hide();
    }
    connect(ui->editButton, &QPushButton::clicked, [this]() {
        if (this->isEditing) {
            ui->editButton->setText("수정");
            ui->commentTextEdit->setReadOnly(true);
            this->isEditing = false;
            emit editComment(this->comment.commentNumber, ui->commentTextEdit->toPlainText());
        } else {
            ui->editButton->setText("수정 완료");
            ui->commentTextEdit->setReadOnly(false);
            this->isEditing = true;
            ui->commentTextEdit->setFocus(); 
        }
    });
    connect(ui->deleteButton, &QPushButton::clicked, [this]() {
        qDebug() << this->comment.commentNumber << "번 댓글 삭제";
        emit deleteComment(this->comment.commentNumber);
    });
}

CommentWidget::~CommentWidget()
{
    delete ui;
}