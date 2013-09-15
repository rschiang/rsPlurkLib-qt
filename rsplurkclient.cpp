#include "rsplurkclient.h"

#include <QUrl>
#include <QStringBuilder>
#include <QCryptographicHash>

const QString REQUEST_TOKEN_URL = QString("https://www.plurk.com/OAuth/request_token");
const QString AUTH_URL_BASE = QString("https://www.plurk.com/OAuth/authorize?oauth_token=%1&deviceid=%2");
const QString AUTH_URL_MOBILE_BASE = QString("https://www.plurk.com/m/authorize?oauth_token=%1&deviceid=%2");
const QString EXCHANGE_TOKEN_URL = QString("https://www.plurk.com/OAuth/access_token");
const QString API_URL_BASE = QString("https://www.plurk.com/APP/%1");
const QString ARG_OAUTH_TOKEN = QString("oauth_token");
const QString ARG_OAUTH_TOKEN_SECRET = QString("oauth_token_secret");
const QString ARG_OAUTH_KEY = QString("oauth_consumer_key");
const QString ARG_OAUTH_NONCE = QString("oauth_nonce");
const QString ARG_OAUTH_TIMESTAMP = QString("oauth_timestamp");
const QString ARG_OAUTH_VERSION = QString("oauth_version");
const QString ARG_OAUTH_SIG = QString("oauth_signature");
const QString ARG_OAUTH_SIG_METHOD = QString("oauth_signature_method");
const QString ARG_OAUTH_CALLBACK = QString("oauth_callback");

const QString OAUTH_VERSION = QString("1.0");
const QString OAUTH_SIG_METHOD = QString("HMAC-SHA1");
const QString OAUTH_CALLBACK = QString("oob");

const QString AUTH_HEADER_TEMPLATE = QString("OAuth realm=\"\"");
const QString AUTH_HEADER_ITEM = QString(", %1=\"%2\"");

const QByteArray HEADER_CONTENT_TYPE = QByteArray("Content-Type");
const QByteArray HEADER_AUTHORIZATION = QByteArray("Authorization");
const QByteArray MIME_FORM_URLENCODED = QByteArray("application/x-www-form-urlencoded");

class RSPlurkClientPrivate {
public:
	RSPlurkClientPrivate() {
		manager = 0;
		reqTokenReply = 0;
		accTokenReply = 0;
	}

	~RSPlurkClientPrivate() {
	}

	const QString appKey;
	const QString appSecret;
	QString tokenId;
	QString tokenSecret;
	QNetworkAccessManager* manager;

	QNetworkReply* reqTokenReply, accTokenReply;
};

explicit RSPlurkClient::RSPlurkClient(QObject *parent = 0)
	: QObject(parent), d_ptr(new RSPlurkClientPrivate()) {
	//
}
RSPlurkClient::~RSPlurkClient() {
	delete d_ptr;
	d_ptr = 0;
}

void RSPlurkClient::setAppIdentity(const QString appKey, const QString appSecret) {
	Q_D(RSPlurkClient);
	d->appKey = appKey;
	d->appSecret = appSecret;
}
void RSPlurkClient::setToken(QString tokenId, QString tokenSecret) {
	Q_D(RSPlurkClient);
	d->tokenId = tokenId;
	d->tokenSecret = tokenSecret;
}

void RSPlurkClient::setNetworkAccessManager(QNetworkAccessManager* manager) {
	Q_D(RSPlurkClient);
	d->manager = manager;
}

void QNetworkAccessManager* RSPlurkClient::getNetworkAccessManager() {
	Q_D(RSPlurkClient);
	if (!d->manager)
		d->manager = new QNetworkAccessManager(this);
	return d->manager;
}

void RSPlurkClient::getRequestToken() {
	Q_D(RSPlurkClient);

	const QStringMap args;
	args[ARG_OAUTH_CALLBACK] = OAUTH_CALLBACK;

	QNetworkRequest request = createRequest(REQUEST_TOKEN_URL, args);
	QNetworkReply* reply = getNetworkAccessManager()->post(request, QByteArray());

	connect(reply, SIGNAL(finished()), this, SLOT(requestTokenCallback()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkError(QNetworkReply::NetworkError)));

	d->reqTokenReply = reply;
}

void RSPlurkClient::requestTokenCallback() {
	Q_D(RSPlurkClient);

	QStringMap values = parseQueryString(QString(d->reqTokenReply->readAll()));
	d->tokenId = values[ARG_OAUTH_TOKEN];
	d->tokenSecret = values[ARG_OAUTH_TOKEN_SECRET];

	const QStringPair token(d->tokenId, d->tokenSecret);
	tokenReceived(token);
}

const QString RSPlurkClient::getAuthorizationUrl(QString deviceName) {
	Q_D(RSPlurkClient);
	const QString url = ((deviceName.length() > 0) ? AUTH_URL_MOBILE_BASE : AUTH_URL_BASE).arg(d->tokenId).arg(deviceName);
	return url;
}

void RSPlurkClient::getAccessToken(QString verifier) {
	//
}

void RSPlurkClient::accessTokenCallback() {
	//
}

QNetworkReply* RSPlurkClient::sendRequest(const QString endpointUri, const QStringMap args) {
	
	const QString uri(API_URL_BASE.arg(endpointUri));
	
	QNetworkRequest request = createRequest(uri, args);
	return getNetworkAccessManager()->post(request, QByteArray());
}

QString RSPlurkClient::urlEncode(QString str) {
	return QUrl::toPercentEncoding(str).constData();
}

QString RSPlurkClient::urlDecode(QString str) {
	return QUrl::fromPercentEncoding(str.toAscii());
}

QStringMap RSPlurkClient::parseQueryString(QString queryString) {
	QStringMap values;
	QString buffer = urlDecode(queryString);
	
	int len = buffer.size();
	for (int pos = 0; pos < len; pos++) {
		int start_pos = pos;
		int equ_pos = -1;

		for (; pos < len; pos++) {
			char cur = buffer[pos];
			if ((cur == '=') && (equ_pos < 0)) equ_pos = pos;
			else if (cur == '&') break; 
		}

		QString name = buffer.mid(start_pos, (equ_pos >= 0) ? equ_pos - start_pos : pos - start_pos);
		QString value = (equ_pos >= 0) ? buffer.mid(equ_pos + 1, pos - equ_pos - 1) : QString();

		values[urlDecode(name)] = urlDecode(value);
	}

	return values;
}

QString RSPlurkClient::computeSignature(const QString uri, QStringMap args) {
	Q_D(RSPlurkClient);

	QByteArray baseString;
	baseString.append("POST&").append(urlEncode(uri)).append("&");

	bool usePrefix = false;
	foreach (QString key, args.keys()) {
		if (usePrefix) baseString.append("%26");
		baseString.append(key).append("%3D").append(urlEncode(urlEncode(args.value(key))));
		usePrefix = true;
	}

	QByteArray keyString;
	keyString.append(urlEncode(d->appSecret)).append("&").append(urlEncode(d->tokenSecret));

	// HMAC
	int blockSize = 64;
	QByteArray base(baseString);
	QByteArray ipad(blockSize, 0x5c);
	QByteArray opad(blockSize, 0x36);
	if (base.size() > blockSize)
		base = QCryptographicHash::hash(base, QCryptographicHash::Sha1);

	if (base.size() < blockSize)
		base += QByteArray(blockSize - base.size(), 0x00);

	for (int i = 0; i < base.size() - 1; i++) {
		ipad[i] = (char) (ipad[i] ^ base[i]);
		opad[i] = (char) (opad[i] ^ base[i]);
	}

	ipad += keyString;
	opad += QCryptographicHash::hash(ipad, QCryptographicHash::Sha1);
	return QString(QCryptographicHash::hash(opad, QCryptographicHash::Sha1).toBase64());
}

QNetworkRequest RSPlurkClient::createRequest(const QString uri, const QStringMap args) {
	Q_D(RSPlurkClient);

	QString nonce, timestamp, sig;
	args[ARG_OAUTH_KEY] = d->appKey;
	args[ARG_OAUTH_TOKEN] = d->tokenId;
	args[ARG_OAUTH_NONCE] = (nonce = QString("%1").arg(qrand()));
	args[ARG_OAUTH_TIMESTAMP] = (timestamp = QString("%1").arg(QDateTime::currentDateTime().toTime_t()));
	args[ARG_OAUTH_SIG_METHOD] = OAUTH_SIG_METHOD;
	args[ARG_OAUTH_VERSION] = OAUTH_VERSION;
	args[ARG_OAUTH_SIG] = computeSignature(endpointUri, args);

	QNetworkRequest request;

	QByteArray authHeader;
	authHeader.append(AUTH_HEADER_TEMPLATE);
	foreach (QString key, args.keys())
		if (key.startsWith("oauth_"))
			authHeader.append(AUTH_HEADER_ITEM.arg(key).arg(args.value(key)));

	request.setRawHeader(HEADER_CONTENT_TYPE, MIME_FORM_URLENCODED);
	request.setRawHeader(HEADER_AUTHORIZATION, authHeader);
	request.setUrl(endpointUrl);

	return request;
}
