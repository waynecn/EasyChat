#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QtWebSockets/QWebSocket>

const int               BUFF_SIZE = 1024 * 256;

const QString SETTING_SERVER_HOST_LIST = "SETTING_SERVER_HOST_LIST";
const QString SETTING_CURRENT_SERVER_HOST = "SETTING_CURRENT_SERVER_HOST";
const QString SETTING_WEBSOCKET_SERVER_HOST = "WEBSOCKET_SERVER_HOST";
const QString SETTING_SERVER_PORT = "SETTING_SERVER_PORT";
const QString SETTING_USERNAME = "SETTING_USERNAME";
const QString SETTING_USERPASSWORD = "SETTING_USERPASSWORD";
const QString SETTING_REMEMBER_PASSWORD = "SETTING_REMEMBER_PASSWORD";
const QString SETTING_SAVE_FILE_DIR = "SETTING_SAVE_FILE_DIR";
const QString SETTING_USE_TCP = "SETTING_USE_TCP";

static QString APPLICATION_VERSION = "1.2.5";

extern QString g_userID;        //current logined user info
extern QString g_userName;      //current logined user info
extern QString g_password;      //current user password
extern QString g_loginTime;     //
extern QString g_toUserID;      //if current chat widget tab index is not zero, then it cann't be empty
extern QString g_toUserName;    //if current chat widget tab index is not zero, then it cann't be empty
extern QString g_serverHost;
extern QString g_serverPort;
extern QString g_Token;

extern QWebSocket g_WebSocket;

extern QString APPLICATION_DIR;

extern QString APPLICATION_IMAGE_DIR;

extern QString APPLICATION_TMPIMAGE_DIR;  //when we send a clipboard image data, we save image to this temporary dir, after send over, delete the tmp file

enum MsgType {
    NORMAL_MSG,
    FILE_MSG,
    URL_MSG,
    IMG_MSG,
    VIDEO_MSG
};

enum HttpRequest {
    REQUEST_UNKNOWN,
    REQUEST_LOGIN,
    REQUEST_REGISTER,
    REQUEST_UPLOAD_FILE,
    REQUEST_UPLOAD_TMPFILE,
    REQUEST_DOWNLOAD_FILE,
    REQUEST_DOWNLOAD_IMAGE,
    REQUEST_GET_UPLOAD_FILES,
    REQUEST_DELETE_FILE,
    REQUEST_UPLOAD_CLIENT,
    REQUEST_DOWNLOAD_CLIENT,
    REQUEST_UPLOAD_FILE_BY_TCP,
    REQUEST_UPLOAD_TMPFILE_BY_TCP,
    REQUEST_DOWNLOAD_FILE_BY_TCP
};

typedef struct _messageStruct{
    MsgType msgType;
    QString msg;
    QString userID;
    QString userName;
    QString sendTime;
    QString fileName;
    QString fileUrl;
    QString imgUrl;
    QString toUserID;
    QString toUserName;
    bool uploadToPersonalSpace;
} MessageStruct, *PMessageStruct;

typedef struct _user_info {
    QString userName;
    QString userID;
    QString password;
    QString loginTime;
    QString email;
    QString phoneNumber;
} UserInfo, *PUserInfo;

typedef struct _network_param {
    QString         paramID;        //random id
    QString         userName;     //current login userName
    QString         password;       //only used when login
    QString         phoneNumber;    //only used when register
    QString         userID;       //as top
    QString         toUserName;   //send to userName
    QString         toUserID;     //send to userID
    QString         filePath;     //file absolute path
    QString         fileName;     //fileName
    QString         fileLink;     //fileUrl
    HttpRequest     httpRequestType;
    QString         requestTime;    //request time
    QString         requestEndTime; //request end time
    QString         saveFileName;   //when saving file, user given file name
    QString         saveFileDir;    //file save directory

    int             itemRow;        //this param row index in upload or download tablewidget
    quint64          totalSize;      //this item is only used when processing file
    quint64          recved;         //this item is only used when processing file
    qint64          timeLeft;       //this item is only used when processing file
    qint64          speed;          //this item is only used when processing file
    bool            msgSend;
    bool            uploadToPersonalSpace;

    _network_param () {
        paramID = "";
        userName = "";
        password = "";
        phoneNumber = "";
        userID = "";
        toUserName = "";
        toUserID = "";
        filePath = "";
        fileName = "";
        fileLink = "";
        httpRequestType = REQUEST_UNKNOWN;
        requestTime = "";
        requestEndTime = "";
        saveFileName = "";
        saveFileDir = "";

        itemRow = -1;
        totalSize = 0;
        recved = 0;
        timeLeft = -1;
        speed = -1;
        msgSend = false;
        uploadToPersonalSpace = false;
    }

    _network_param &operator = (const _network_param &other) {
        if (this == &other) {
            return *this;
        }
        paramID = other.paramID;
        userName = other.userName;
        password = other.password;
        phoneNumber = other.phoneNumber;
        userID = other.userID;
        toUserName = other.toUserName;
        toUserID = other.toUserID;
        filePath = other.filePath;
        fileName = other.fileName;
        fileLink = other.fileLink;
        httpRequestType = other.httpRequestType;
        requestTime = other.requestTime;
        requestEndTime = other.requestEndTime;
        saveFileName = other.saveFileName;
        saveFileDir = other.saveFileDir;

        itemRow = other.itemRow;
        totalSize = other.totalSize;
        recved = other.recved;
        timeLeft = other.timeLeft;
        speed = other.speed;
        msgSend = other.msgSend;
        uploadToPersonalSpace = other.uploadToPersonalSpace;
        return *this;
    }
} NetworkParams, *PNetworkParams;

const QString WEBSOCKET_ERROR_STRINGS[24] = {
    "An unidentified error occurred.",
    "The connection was refused by the peer (or timed out).",
    "The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent.",
    "The host address was not found.",
    "The socket operation failed because the application lacked the required privileges.",
    "The local system ran out of resources (e.g., too many sockets).",
    "The socket operation timed out.",
    "The datagram was larger than the operating system's limit (which can be as low as 8192 bytes).",
    "An error occurred with the network (e.g., the network cable was accidentally plugged out).",
    "The address specified to QAbstractSocket::bind() is already in use and was set to be exclusive.",
    "The address specified to QAbstractSocket::bind() does not belong to the host.",
    "The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support).",
    "The socket is using a proxy, and the proxy requires authentication.",
    "The SSL/TLS handshake failed, so the connection was closed (only used in QSslSocket)",
    "Used by QAbstractSocketEngine only, The last operation attempted has not finished yet (still in progress in the background).",
    "Could not contact the proxy server because the connection to that server was denied",
    "The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established)",
    "The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase.",
    "The proxy address set with setProxy() (or the application proxy) was not found.",
    "The connection negotiation with the proxy server failed, because the response from the proxy server could not be understood.",
    "An operation was attempted while the socket was in a state that did not permit it.",
    "The SSL library being used reported an internal error. This is probably the result of a bad installation or misconfiguration of the library.",
    "Invalid data (certificate, key, cypher, etc.) was provided and its use resulted in an error in the SSL library.",
    "A temporary error occurred (e.g., operation would block and socket is non-blocking)."};

const QString SEND_FILE_BUTTON_STYLE_SHEET = "QPushButton { border: 2px solid #8f8f91; border-radius: 6px; \
        background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, \
        stop: 0 #f6f7fa, stop: 1 #dadbde); \
        min-width: 60px; } \
    QPushButton:pressed { \
        background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, \
        stop: 0 #dadbde, stop: 1 #f6f7fa); \
    } \
    QPushButton:flat { \
        border: none; \
    } \
    QPushButton:default { \
        border-color: navy; \
    }";
const QString FILE_LIST_BUTTON_STYLE_SHEET = "QPushButton { border: 2px solid #8f8f91; border-radius: 6px; \
        background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, \
        stop: 0 #f6f7fa, stop: 1 #dadbde); \
        min-width: 75px; } \
    QPushButton:pressed { \
        background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, \
        stop: 0 #dadbde, stop: 1 #f6f7fa); \
    } \
    QPushButton:flat { \
        border: none; \
    } \
    QPushButton:default { \
        border-color: navy; \
    }";
const QString SEND_MESSAGE_BUTTON_STYLE_SHEET = "QPushButton { border: 2px solid #8f8f91; border-radius: 6px; \
        background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, \
        stop: 0 #33A1C9, stop: 1 #dadbde); \
        min-width: 75px; } \
    QPushButton:pressed { \
        background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, \
        stop: 0 #33A1C9, stop: 1 #4169E1); \
    } \
    QPushButton:flat { \
        border: none; \
    } \
    QPushButton:default { \
        border-color: navy; \
    }";
const QString TAB_BAR_STYLE_SHEET = "QTabWidget::pane { border-top: 2px solid #C2C7CB;  }\
        QTabWidget::tab-bar {left: 5px; }\
        QTabBar::tab {      background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                        stop: 0 #E1E1E1, stop: 0.4 #DDDDDD,\
                                        stop: 0.5 #D8D8D8, stop: 1.0 #D3D3D3);\
            border: 2px solid #C4C4C3;\
            border-bottom-color: #C2C7CB; \
            border-top-left-radius: 4px;\
            border-top-right-radius: 4px;\
            min-width: 8ex;\
            padding: 2px;\
        }\
        QTabBar::tab:selected, QTabBar::tab:hover {\
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\
                                        stop: 0 #fafafa, stop: 0.4 #f4f4f4,\
                                        stop: 0.5 #e7e7e7, stop: 1.0 #fafafa);\
        }\
        QTabBar::tab:selected {\
            border-color: #9B9B9B;\
            border-bottom-color: #C2C7CB;\
        }\
        QTabBar::tab:!selected {\
            margin-top: 2px; \
            }";

void RestartApp();

#endif // COMMON_H
