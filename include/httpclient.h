#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "post.h"

// #define SERVER_URL "http://192.168.0.85:8000"
#define SERVER_URL "http://192.168.56.101:8000"
#define httpclient HttpClient::getInstance()

class HttpClient : public QObject
{
    Q_OBJECT

public:
    static HttpClient* getInstance();    

    void fetchAllPosts();
    void fetchPostById(int postId);
    void join(const QString &loginId, const QString &password, const QString &name);
    void login(const QString &loginId, const QString &password);
    void uploadPost(const QString &subject, const QString &writer, const QString &description);
    void editPost(int postId, const QString &subject, const QString &description);
    void uploadComment(int postId, const QString &writer, const QString &description);
    void editComment(int commentId, const QString &description);
    void deletePost(int postId);
    void deleteComment(int commentId);
    void deleteUser(const QString &user);
    
    void uploadFile(int postId, const QString &filePath);
    void downloadFile(int fileId, const QString &savePath);

signals:
    void allPostsFetched(QList<Post> postList);
    void postFetched(Post post);
    void joinSucceed();
    void joinFailed(QString message);
    void loginSucceed(QString username);
    void loginFailed();
    void uploadPostResponse(QByteArray data);
    void editPostResponse(QByteArray data);
    void uploadCommentResponse(QByteArray data);
    void editCommentResponse(QByteArray data);
    void deletePostResponse(QByteArray data);
    void deleteCommentResponse(QByteArray data);
    void deleteUserResponse(QByteArray data);

    void uploadFileProgress(qint64 bytesSent, qint64 bytesTotal);
    void uploadFileComplete(const QByteArray &response);
    void uploadFileFailed(const QString &error);
    void downloadFileProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFileComplete(const QString &filePath);
    void downloadFileFailed(const QString &error);


private slots:
    void onAllPostsFetched(QNetworkReply *reply);
    void onPostFetched(QNetworkReply *reply);
    void onJoinResponse(QNetworkReply *reply);
    void onLoginResponse(QNetworkReply *reply);
    void onUploadPostResponse(QNetworkReply *reply);
    void onEditPostResponse(QNetworkReply *reply);
    void onUploadCommentResponse(QNetworkReply *reply);
    void onEditCommentResponse(QNetworkReply *reply);
    void onDeletePostResponse(QNetworkReply *reply);
    void onDeleteCommentResponse(QNetworkReply *reply);
    void onDeleteUserResponse(QNetworkReply *reply);

    void onUploadFileProgress(qint64 bytesSent, qint64 bytesTotal);
    void onUploadFileComplete(QNetworkReply *reply);
    void onDownloadFileProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadFileComplete(QNetworkReply *reply);


private:
    explicit HttpClient(QObject *parent = nullptr);
    static HttpClient* instance;
    
    QNetworkAccessManager *fetchAllPostsManager;
    QNetworkAccessManager *fetchPostByIdManager;
    QNetworkAccessManager *joinManager;
    QNetworkAccessManager *loginManager;
    QNetworkAccessManager *uploadPostManager;
    QNetworkAccessManager *editPostManager;
    QNetworkAccessManager *uploadCommentManager;
    QNetworkAccessManager *editCommentManager;
    QNetworkAccessManager *deletePostManager;
    QNetworkAccessManager *deleteCommentManager;
    QNetworkAccessManager *deleteUserManager;
    QNetworkAccessManager *uploadFileManager;
    QNetworkAccessManager *downloadFileManager; 

    QJsonObject byteArrayToJsonObject(const QByteArray& data);
};

#endif // HTTPCLIENT_H