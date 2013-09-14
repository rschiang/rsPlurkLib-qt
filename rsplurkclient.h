#ifndef RSPLURKCLIENT_H
#define RSPLURKCLIENT_H

#include <QtCore>

#include <QUrl>
#include <QPair>
#include <QString>
#include <QNetworkReply>

typedef QPair<QString,QString> QStringPair;
typedef QMap<QString,QString> QStringMap;

class RSPlurkClientPrivate;
class RSPlurkClient : QObject {
	Q_OBJECT
public:
    explicit RSPlurkClient(QObject *parent = 0);
    ~RSPlurkClient();

    // Application setup
    void setAppIdentity(const QString appKey, const QString appSecret);
    void setToken(QString tokenId, QString tokenSecret);

    // 3-step OAuth authentication
    void getRequestToken();
    void getAuthorizationUrl();
    const QStringPair getAccessToken(QString verifier);

    // OAuth method
    QNetworkReply* sendRequest(const QString endpointUrl, const QStringMap args);

private:
	RSPlurkClientPrivate* d_ptr;
	Q_DECLARE_PRIVATE(RSPlurkClient)
};

#endif // RSPLURKCLIENT_H
