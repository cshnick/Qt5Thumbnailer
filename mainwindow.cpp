#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWebKitWidgets>


void MyPage::javaScriptConsoleMessage(const QString & message, int lineNumber, const QString & sourceID)
{
    Q_UNUSED(lineNumber);
    Q_UNUSED(sourceID);

    qDebug() << message;
    m_message_stack << message;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
  , m_tmr(0)
{
    ui->setupUi(this);
    setContentsMargins(0, 0, 0, 0);
    ui->menuBar->setVisible(false);
    ui->mainToolBar->setVisible(false);
    ui->statusBar->setVisible(false);
    m_view = new QWebView(this);
    m_page = new MyPage();
    m_view->setPage(m_page);
    m_page->setViewportSize(QSize(1024, 768));
    setCentralWidget(m_view);
    m_view->setVisible(true);
    //m_view->setGeometry(0, 0, 1024, 800);
    m_view->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
    m_view->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
//    m_view->load(QUrl("http://staging-ut.urturn.com/documents/C5E51960-EE40-0001-56F5-2E3F1D4D4990/pages/1"));
    m_view->load(QUrl("http://newevolutiondesigns.com/50-fresh-hd-wallpapers"));


    connect(m_view->page(), SIGNAL(loadFinished(bool)), this, SLOT(onPageloadFinished(bool)));
    connect(this, SIGNAL(jsFinished()), this , SLOT(onJsFinished()));
}

MyPage *MainWindow::myPage()
{
    return static_cast<MyPage*>(m_view->page());
}

void MainWindow::generateThumb()
{
    QWebFrame *frame = m_page->mainFrame();
    qDebug() << frame->geometry() << frame->contentsSize();
    QImage pix(frame->contentsSize(),  QImage::Format_ARGB32_Premultiplied);

    QPainter painter(this);
    painter.begin(&pix);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    int yOffset = 0;
    int spaceYLeft = frame->contentsSize().height() - yOffset;
    int translateOffset = frame->geometry().height();
    bool exit = false;
    for(;;) {
        frame->render(&painter);
        if (exit) {
            break;
        }
        frame->scroll(0, translateOffset);
        int iterOffset = translateOffset;
        if (spaceYLeft - translateOffset < 0) {
            iterOffset = spaceYLeft - translateOffset;
            exit = true;
        }
        painter.translate(0, iterOffset);
        yOffset += translateOffset;
        spaceYLeft = frame->contentsSize().height() - yOffset;
    }
    painter.end();

    pix.save("/home/ilia/Documents/tmp/savedframe.png");

    qDebug() << "MainWindow::generateThumb() generated";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onloadFinished(bool ok)
{
    if (!ok) {
        return;
    }

    ui->statusBar->setVisible(true);
    ui->statusBar->showMessage("Loading finished");

    QTimer::singleShot(2000, ui->statusBar, SLOT(hide()));
}

void MainWindow::onPageloadFinished(bool ok)
{
    if (!ok) {
        return;
    }

    ui->statusBar->setVisible(true);
    ui->statusBar->showMessage("Page load finished");

    QTimer::singleShot(2000, this, SLOT(assignTimer()));
    //QTimer::singleShot(2000, ui->statusBar, SLOT(hide()));
}

void MainWindow::checkPageStack()
{
    qDebug() << "MainWindow::checkPageStack(), messageStack" << myPage()->messageStack().count();
    if (myPage()->messageStack().isEmpty()) {
        m_tmr->stop();

        onJsFinished();
    }

    myPage()->clearMessageStack();
    qDebug() << "MainWindow::checkPageStack() , after clearing messageStack" << myPage()->messageStack().count();
}

void MainWindow::assignTimer()
{
    if (!m_tmr) {
        m_tmr = new QTimer(this);
        connect(m_tmr, SIGNAL(timeout()), this, SLOT(checkPageStack()));
        m_tmr->start(2000);
    }
}

void MainWindow::onJsFinished()
{
    qDebug() << "Attempting to generate thumbnail";
    generateThumb();
}
