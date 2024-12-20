#include "httpclient.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QHttpPart>
#include <QHttpMultiPart>
#include <QFileInfo>
#include <QIODevice>

HttpClient* HttpClient::instance = nullptr;

HttpClient::HttpClient(QObject *parent) : QObject(parent)
{
    fetchAllPostsManager = new QNetworkAccessManager(this);
    fetchPostByIdManager = new QNetworkAccessManager(this);
    joinManager = new QNetworkAccessManager(this);
    loginManager = new QNetworkAccessManager(this);
    uploadPostManager = new QNetworkAccessManager(this);
    editPostManager = new QNetworkAccessManager(this);
    uploadCommentManager = new QNetworkAccessManager(this);
    editCommentManager = new QNetworkAccessManager(this);
    deletePostManager = new QNetworkAccessManager(this);
    deleteCommentManager = new QNetworkAccessManager(this);
    deleteUserManager = new QNetworkAccessManager(this);

    // connect 모아두기
    connect(fetchAllPostsManager, &QNetworkAccessManager::finished, this, &HttpClient::onAllPostsFetched);
    connect(fetchPostByIdManager, &QNetworkAccessManager::finished, this, &HttpClient::onPostFetched);
    connect(joinManager, &QNetworkAccessManager::finished, this, &HttpClient::onJoinResponse);
    connect(loginManager, &QNetworkAccessManager::finished, this, &HttpClient::onLoginResponse);
    connect(uploadPostManager, &QNetworkAccessManager::finished, this, &HttpClient::onUploadPostResponse);
    connect(editPostManager, &QNetworkAccessManager::finished, this, &HttpClient::onEditPostResponse);
    connect(uploadCommentManager, &QNetworkAccessManager::finished, this, &HttpClient::onUploadCommentResponse);
    connect(editCommentManager, &QNetworkAccessManager::finished, this, &HttpClient::onEditCommentResponse);
    connect(deletePostManager, &QNetworkAccessManager::finished, this, &HttpClient::onDeletePostResponse);
    connect(deleteCommentManager, &QNetworkAccessManager::finished, this, &HttpClient::onDeleteCommentResponse);
    connect(deleteUserManager, &QNetworkAccessManager::finished, this, &HttpClient::onDeleteUserResponse);

}

HttpClient* HttpClient::getInstance()
{
    if (instance == nullptr) {
        instance = new HttpClient();
    }
    return instance;
}

QJsonObject HttpClient::byteArrayToJsonObject(const QByteArray &data)
{   
    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.object();
}

void HttpClient::fetchAllPosts()
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/get-all-post")));
    fetchAllPostsManager->get(request);
}

void HttpClient::fetchPostById(int postId)
{
    QNetworkRequest request(QUrl(QString(tr(SERVER_URL) + tr("/get-post/%1").arg(postId))));
    fetchPostByIdManager->get(request);
}

void HttpClient::join(const QString &loginId, const QString &password, const QString &name)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/join")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["loginId"] = loginId;
    json["password"] = password;
    json["name"] = name;

    joinManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::login(const QString &loginId, const QString &password)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/login")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["loginId"] = loginId;
    json["password"] = password;

    loginManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::uploadPost(const QString &subject, const QString &writer, const QString &description)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/upload-post")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["subject"] = subject;
    json["writer"] = writer;
    json["description"] = description;

    uploadPostManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::editPost(int postId, const QString &subject, const QString &description)
{
    QNetworkRequest request(QUrl(QString(tr(SERVER_URL) + tr("/edit-post"))));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    qDebug() << "editPost" << postId << subject << description;
    QJsonObject json;
    json["postNumber"] = postId;
    json["subject"] = subject;
    json["description"] = description;

    editPostManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::uploadComment(int postId, const QString &writer, const QString &description)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/upload-comment")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["postNumber"] = postId;
    json["writer"] = writer;
    json["description"] = description;

    uploadCommentManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::editComment(int commentId, const QString &description)
{
    QNetworkRequest request(QUrl(QString(tr(SERVER_URL) + tr("/edit-comment"))));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["commentNumber"] = commentId;
    json["description"] = description;

    editCommentManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::deletePost(int postId)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/delete-post")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["postNumber"] = postId;

    deletePostManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::deleteComment(int commentId)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/delete-comment")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["commentNumber"] = commentId;

    deleteCommentManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::deleteUser(const QString &user)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/delete-user")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["user"] = user;

    deleteUserManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::uploadFile(int postId, const QString &filePath) {
    QFile *file = new QFile(filePath);
    if (!file->open(QIODevice::ReadOnly)) {
        emit uploadFileFailed("파일을 열 수 없습니다.");
        return;
    }

    QFileInfo fileInfo(*file);
    QString fileName = fileInfo.fileName();

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    // postId 추가
    QHttpPart postIdPart;
    postIdPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"postId\""));
    postIdPart.setBody(QString::number(postId).toUtf8());
    multiPart->append(postIdPart);

    // 파일 추가
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"" + fileName + "\""));
    filePart.setBodyDevice(file);
    file->setParent(multiPart);
    multiPart->append(filePart);

    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/upload-file")));
    QNetworkReply *reply = uploadFileManager->post(request, multiPart);
    multiPart->setParent(reply);

    connect(reply, &QNetworkReply::uploadProgress, this, &HttpClient::uploadFileProgress);
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        this->onUploadFileComplete(reply);
    });
}   

void HttpClient::downloadFile(int fileId, const QString &savePath) {
    QNetworkRequest request(QUrl(QString(tr(SERVER_URL) + tr("/download-file/%1").arg(fileId))));
    QNetworkReply *reply = downloadFileManager->get(request);

    reply->setProperty("savePath", savePath);

    QFile *file = new QFile(savePath);
    if (!file->open(QIODevice::WriteOnly)) {
        emit downloadFileFailed("파일을 열 수 없습니다.");
        return;
    }

    connect(reply, &QNetworkReply::downloadProgress, this, &HttpClient::downloadFileProgress);
    connect(reply, &QNetworkReply::finished, [this, reply, file]() {
        this->onDownloadFileComplete(reply);
    });
}

// slots

void HttpClient::onAllPostsFetched(QNetworkReply *reply)
{
    qDebug() << "onAllPostsFetched";
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        QJsonArray posts = response["posts"].toArray();

        QList<Post> postList;

        for (const QJsonValue &post : posts) {
            Post postStruct;
            postStruct.postNumber = post.toObject()["postNumber"].toInt();
            postStruct.subject = post.toObject()["subject"].toString();
            postStruct.writer = post.toObject()["writer"].toString();
            postList.push_back(postStruct);
        }

        emit allPostsFetched(postList);
    }
    reply->deleteLater();
}

void HttpClient::onPostFetched(QNetworkReply *reply)
{
    qDebug() << "onPostFetched";
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        Post post;
        post.postNumber = response["postNumber"].toInt();
        post.subject = response["subject"].toString();
        post.writer = response["writer"].toString();
        post.description = response["description"].toString();
        for (const QJsonValue &comment : response["comments"].toArray()) {
            Comment commentStruct;
            commentStruct.commentNumber = comment.toObject()["commentNumber"].toInt();
            commentStruct.writer = comment.toObject()["writer"].toString();
            commentStruct.description = comment.toObject()["description"].toString();
            post.comments.append(commentStruct);
        }

        emit postFetched(post);
    }
    reply->deleteLater();
}

void HttpClient::onJoinResponse(QNetworkReply *reply)
{
    qDebug() << "onJoinResponse";
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        if (response["result"].toString() == "success") {
            emit joinSucceed();
        } else {
            emit joinFailed(response["message"].toString());
        }
    }
    reply->deleteLater();
}

void HttpClient::onLoginResponse(QNetworkReply *reply)
{
    qDebug() << "onLoginResponse";
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        if (response["user"].toString() != "") {
            emit loginSucceed(response["user"].toString());
        } else {
            emit loginFailed();
        }
    }
    reply->deleteLater();
}

void HttpClient::onUploadPostResponse(QNetworkReply *reply)
{
    qDebug() << "onUploadPostResponse";
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        emit uploadPostResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onEditPostResponse(QNetworkReply *reply)
{
    qDebug() << "onEditPostResponse";
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        emit editPostResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onUploadCommentResponse(QNetworkReply *reply)
{
    qDebug() << "onUploadCommentResponse";
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        emit uploadCommentResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onEditCommentResponse(QNetworkReply *reply)
{
    qDebug() << "onEditCommentResponse";
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        emit editCommentResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onDeletePostResponse(QNetworkReply *reply)
{
    qDebug() << "onDeletePostResponse";
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        emit deletePostResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onDeleteCommentResponse(QNetworkReply *reply)
{
    qDebug() << "onDeleteCommentResponse";
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        emit deleteCommentResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onDeleteUserResponse(QNetworkReply *reply)
{
    qDebug() << "onDeleteUserResponse";
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject response = byteArrayToJsonObject(reply->readAll());
        emit deleteUserResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onUploadFileProgress(qint64 bytesSent, qint64 bytesTotal)
{
    emit uploadFileProgress(bytesSent, bytesTotal);
}

void HttpClient::onUploadFileComplete(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        emit uploadFileComplete(reply->readAll());
    } else {
        emit uploadFileFailed(reply->errorString());
    }
    reply->deleteLater();
}

void HttpClient::onDownloadFileProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadFileProgress(bytesReceived, bytesTotal);
}

void HttpClient::onDownloadFileComplete(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QString savePath = reply->property("savePath").toString();
        QFile file(savePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
            emit downloadFileComplete(savePath);
        } else {
            emit downloadFileFailed("파일을 열 수 없습니다.");
        }
    } else {
        emit downloadFileFailed(reply->errorString());
    }
    reply->deleteLater();
}