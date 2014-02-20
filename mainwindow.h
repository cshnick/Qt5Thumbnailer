#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebPage>

namespace Ui {
class MainWindow;
}
class QWebView;

class MyPage : public QWebPage {
    Q_OBJECT

public:
    QStringList messageStack() const {return m_message_stack;}
    void clearMessageStack() {m_message_stack.clear();}


protected:
    void javaScriptConsoleMessage(const QString & message, int lineNumber, const QString & sourceID);
private:
    QStringList m_message_stack;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    MyPage *myPage();
    void generateThumb();
    ~MainWindow();

private Q_SLOTS:
    void onloadFinished(bool ok);
    void onPageloadFinished(bool ok);
    void checkPageStack();
    void assignTimer();
    void onJsFinished();

Q_SIGNALS:
    void jsFinished();

private:
    Ui::MainWindow *ui;
    QWebView *m_view;
    QWebPage *m_page;
    QTimer *m_tmr;
};

#endif // MAINWINDOW_H
