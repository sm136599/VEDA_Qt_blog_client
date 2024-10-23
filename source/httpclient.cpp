#include "httpclient.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>

HttpClient::HttpClient(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
}

QJsonObject HttpClient::byteArrayToJsonObject(const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    return doc.object();
}

void HttpClient::fetchAllPosts()
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/get-all-post")));
    connect(networkManager, &QNetworkAccessManager::finished, this, &HttpClient::onAllPostsFetched);
    networkManager->get(request);
}

void HttpClient::fetchPostById(int postId)
{
    QNetworkRequest request(QUrl(QString(tr(SERVER_URL) + tr("/get-post/%1").arg(postId))));
    connect(networkManager, &QNetworkAccessManager::finished, this, &HttpClient::onPostFetched);
    networkManager->get(request);
}

void HttpClient::join(const QString &loginId, const QString &password, const QString &passwordCheck, const QString &name)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/join")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["loginId"] = loginId;
    json["password"] = password;
    json["passwordCheck"] = passwordCheck;
    json["name"] = name;

    connect(networkManager, &QNetworkAccessManager::finished, this, &HttpClient::onJoinResponse);
    networkManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::login(const QString &loginId, const QString &password)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/login")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["loginId"] = loginId;
    json["password"] = password;

    connect(networkManager, &QNetworkAccessManager::finished, this, &HttpClient::onLoginResponse);
    networkManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::uploadPost(const QString &subject, const QString &writer, const QString &description)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/upload-post")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["subject"] = subject;
    json["writer"] = writer;
    json["description"] = description;

    connect(networkManager, &QNetworkAccessManager::finished, this, &HttpClient::onUploadPostResponse);
    networkManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::editPost(int postId, const QString &subject, const QString &description)
{
    QNetworkRequest request(QUrl(QString(tr(SERVER_URL) + tr("/edit-post/%1").arg(postId))));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["postNumber"] = postId;
    json["subject"] = subject;
    json["description"] = description;

    connect(networkManager, &QNetworkAccessManager::finished, this, &HttpClient::onEditPostResponse);
    networkManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::uploadComment(int postId, const QString &writer, const QString &description)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/upload-comment")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["postNumber"] = postId;
    json["writer"] = writer;
    json["description"] = description;

    connect(networkManager, &QNetworkAccessManager::finished, this, &HttpClient::onUploadCommentResponse);
    networkManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::editComment(int commentId, const QString &description)
{
    QNetworkRequest request(QUrl(QString(tr(SERVER_URL) + tr("/edit-comment/%1").arg(commentId))));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["commentNumber"] = commentId;
    json["description"] = description;

    connect(networkManager, &QNetworkAccessManager::finished, this, &HttpClient::onEditCommentResponse);
    networkManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::deletePost(int postId)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/delete-post")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["postNumber"] = postId;

    connect(networkManager, &QNetworkAccessManager::finished, this, &HttpClient::onDeletePostResponse);
    networkManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::deleteComment(int commentId)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/delete-comment")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["commentNumber"] = commentId;

    connect(networkManager, &QNetworkAccessManager::finished, this, &HttpClient::onDeleteCommentResponse);
    networkManager->post(request, QJsonDocument(json).toJson());
}

void HttpClient::deleteUser(const QString &user)
{
    QNetworkRequest request(QUrl(tr(SERVER_URL) + tr("/delete-user")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["user"] = user;

    connect(networkManager, &QNetworkAccessManager::finished, this, &HttpClient::onDeleteUserResponse);
    networkManager->post(request, QJsonDocument(json).toJson());
}

// slots

void HttpClient::onAllPostsFetched(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonObject json = byteArrayToJsonObject(reply->readAll());
        
        emit allPostsFetched(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onPostFetched(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        emit postFetched(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onJoinResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        emit joinResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onLoginResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        emit loginResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onUploadPostResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        emit uploadPostResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onEditPostResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        emit editPostResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onUploadCommentResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        emit uploadCommentResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onEditCommentResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        emit editCommentResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onDeletePostResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        emit deletePostResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onDeleteCommentResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        emit deleteCommentResponse(reply->readAll());
    }
    reply->deleteLater();
}

void HttpClient::onDeleteUserResponse(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        emit deleteUserResponse(reply->readAll());
    }
    reply->deleteLater();
}