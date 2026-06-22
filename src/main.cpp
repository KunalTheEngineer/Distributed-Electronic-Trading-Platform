#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>
#include <QElapsedTimer>
#include <QMetaObject>

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>

struct Order
{
    int id;
    std::string side;
    int price;
    int qty;
};

struct Trade
{
    int price;
    int qty;
    int buyId;
    int sellId;
};

class TradingDashboard : public QMainWindow
{
public:

    TradingDashboard()
    {
        setWindowTitle("Trading Dashboard");

        resize(1700, 950);

        QWidget *central = new QWidget();

        setCentralWidget(central);

        central->setStyleSheet("background-color:black; color:white;");

        QVBoxLayout *mainLayout = new QVBoxLayout();

        central->setLayout(mainLayout);

        // ================= TITLE =================

        QLabel *title = new QLabel("TRADING DASHBOARD");

        title->setStyleSheet(
            "font-size:48px;"
            "font-weight:bold;"
            "color:white;"
        );

        mainLayout->addWidget(title);

        // ================= STATS =================

        QHBoxLayout *statsLayout = new QHBoxLayout();

        bestBidLabel = new QLabel("BEST BID : -");
        bestAskLabel = new QLabel("BEST ASK : -");
        spreadLabel = new QLabel("SPREAD : -");
        pnlLabel = new QLabel("PnL : 0");
        positionLabel = new QLabel("Position : 0");
        avgPriceLabel = new QLabel("Avg Price : 0");
        latencyLabel = new QLabel("Execution : 0 us");

        bestBidLabel->setStyleSheet("font-size:18px; font-weight:bold; color:lime;");
        bestAskLabel->setStyleSheet("font-size:18px; font-weight:bold; color:red;");
        spreadLabel->setStyleSheet("font-size:18px; font-weight:bold; color:cyan;");
        pnlLabel->setStyleSheet("font-size:18px; font-weight:bold; color:yellow;");
        positionLabel->setStyleSheet("font-size:18px; font-weight:bold; color:orange;");
        avgPriceLabel->setStyleSheet("font-size:18px; font-weight:bold; color:white;");
        latencyLabel->setStyleSheet("font-size:18px; font-weight:bold; color:magenta;");

        statsLayout->addWidget(bestBidLabel);
        statsLayout->addWidget(bestAskLabel);
        statsLayout->addWidget(spreadLabel);
        statsLayout->addWidget(pnlLabel);
        statsLayout->addWidget(positionLabel);
        statsLayout->addWidget(avgPriceLabel);
        statsLayout->addWidget(latencyLabel);

        mainLayout->addLayout(statsLayout);

        // ================= INPUTS =================

        QHBoxLayout *inputLayout = new QHBoxLayout();

        priceInput = new QLineEdit();
        qtyInput = new QLineEdit();
        cancelInput = new QLineEdit();

        priceInput->setPlaceholderText("Enter Price");
        qtyInput->setPlaceholderText("Enter Qty");
        cancelInput->setPlaceholderText("Cancel Order ID");

        priceInput->setStyleSheet("font-size:18px; padding:10px;");
        qtyInput->setStyleSheet("font-size:18px; padding:10px;");
        cancelInput->setStyleSheet("font-size:18px; padding:10px;");

        QPushButton *buyBtn = new QPushButton("BUY");
        QPushButton *sellBtn = new QPushButton("SELL");
        QPushButton *mktBuyBtn = new QPushButton("MKT BUY");
        QPushButton *mktSellBtn = new QPushButton("MKT SELL");
        QPushButton *cancelBtn = new QPushButton("CANCEL");

        buyBtn->setStyleSheet("background:green; color:white; font-size:16px;");
        sellBtn->setStyleSheet("background:red; color:white; font-size:16px;");
        mktBuyBtn->setStyleSheet("background:blue; color:white; font-size:16px;");
        mktSellBtn->setStyleSheet("background:purple; color:white; font-size:16px;");
        cancelBtn->setStyleSheet("background:orange; color:black; font-size:16px;");

        inputLayout->addWidget(priceInput);
        inputLayout->addWidget(qtyInput);
        inputLayout->addWidget(buyBtn);
        inputLayout->addWidget(sellBtn);
        inputLayout->addWidget(mktBuyBtn);
        inputLayout->addWidget(mktSellBtn);
        inputLayout->addWidget(cancelInput);
        inputLayout->addWidget(cancelBtn);

        mainLayout->addLayout(inputLayout);

        // ================= ORDERBOOK =================

        QHBoxLayout *bookLayout = new QHBoxLayout();

        // BUY TABLE

        QVBoxLayout *buyLayout = new QVBoxLayout();

        QLabel *buyTitle = new QLabel("BUY ORDERS");

        buyTitle->setStyleSheet(
            "font-size:28px;"
            "font-weight:bold;"
            "color:lime;"
        );

        buyTable = new QTableWidget();

        buyTable->setColumnCount(3);

        buyTable->setHorizontalHeaderLabels(
            {"ID", "PRICE", "QUANTITY"}
        );

        styleTable(buyTable);

        buyLayout->addWidget(buyTitle);
        buyLayout->addWidget(buyTable);

        // SELL TABLE

        QVBoxLayout *sellLayout = new QVBoxLayout();

        QLabel *sellTitle = new QLabel("SELL ORDERS");

        sellTitle->setStyleSheet(
            "font-size:28px;"
            "font-weight:bold;"
            "color:red;"
        );

        sellTable = new QTableWidget();

        sellTable->setColumnCount(3);

        sellTable->setHorizontalHeaderLabels(
            {"ID", "PRICE", "QUANTITY"}
        );

        styleTable(sellTable);

        sellLayout->addWidget(sellTitle);
        sellLayout->addWidget(sellTable);

        bookLayout->addLayout(buyLayout);
        bookLayout->addLayout(sellLayout);

        mainLayout->addLayout(bookLayout);

        // ================= TRADE HISTORY =================

        QLabel *tradeTitle = new QLabel("TRADE HISTORY");

        tradeTitle->setStyleSheet(
            "font-size:20px;"
            "font-weight:bold;"
            "color:white;"
        );

        tradeTable = new QTableWidget();

        tradeTable->setColumnCount(4);

        tradeTable->setHorizontalHeaderLabels(
            {"PRICE", "QTY", "BUY ID", "SELL ID"}
        );

        styleTable(tradeTable);

        mainLayout->addWidget(tradeTitle);
        mainLayout->addWidget(tradeTable);

        // ================= MARKET DEPTH =================

        QLabel *depthTitle = new QLabel("MARKET DEPTH");

        depthTitle->setStyleSheet(
            "font-size:20px;"
            "font-weight:bold;"
            "color:cyan;"
        );

        depthTable = new QTableWidget();

        depthTable->setColumnCount(3);

        depthTable->setHorizontalHeaderLabels(
            {"BUY QTY", "PRICE", "SELL QTY"}
        );

        styleTable(depthTable);

        mainLayout->addWidget(depthTitle);
        mainLayout->addWidget(depthTable);

        // ================= LOGS =================

        QLabel *logTitle = new QLabel("ENGINE LOGS");

        logTitle->setStyleSheet(
            "font-size:24px;"
            "font-weight:bold;"
            "color:yellow;"
        );

        logs = new QTextEdit();

        logs->setReadOnly(true);

        logs->setStyleSheet(
            "background:black;"
            "color:lime;"
            "font-size:14px;"
        );

        mainLayout->addWidget(logTitle);
        mainLayout->addWidget(logs);

        // ================= THREAD =================

        matchingThread = std::thread(
            &TradingDashboard::processOrders,
            this
        );

        // ================= BUTTONS =================

        connect(buyBtn, &QPushButton::clicked, [=]() {

            if(priceInput->text().isEmpty() ||
               qtyInput->text().isEmpty())
                return;

            Order order;

            order.id = nextOrderId++;
            order.side = "BUY";
            order.price = priceInput->text().toInt();
            order.qty = qtyInput->text().toInt();

            logMessage(
                "ORDER QUEUED -> BUY " +
                QString::number(order.qty) +
                " @ " +
                QString::number(order.price)
            );

            {
                std::lock_guard<std::mutex> lock(queueMutex);

                orderQueue.push(order);
            }

            cv.notify_one();
        });

        connect(sellBtn, &QPushButton::clicked, [=]() {

            if(priceInput->text().isEmpty() ||
               qtyInput->text().isEmpty())
                return;

            Order order;

            order.id = nextOrderId++;
            order.side = "SELL";
            order.price = priceInput->text().toInt();
            order.qty = qtyInput->text().toInt();

            logMessage(
                "ORDER QUEUED -> SELL " +
                QString::number(order.qty) +
                " @ " +
                QString::number(order.price)
            );

            {
                std::lock_guard<std::mutex> lock(queueMutex);

                orderQueue.push(order);
            }

            cv.notify_one();
        });

        connect(mktBuyBtn, &QPushButton::clicked, [=]() {

            if(qtyInput->text().isEmpty())
                return;

            Order order;

            order.id = nextOrderId++;
            order.side = "MKT_BUY";
            order.price = 0;
            order.qty = qtyInput->text().toInt();

            logMessage(
                "ORDER QUEUED -> MARKET BUY " +
                QString::number(order.qty)
            );

            {
                std::lock_guard<std::mutex> lock(queueMutex);

                orderQueue.push(order);
            }

            cv.notify_one();
        });

        connect(mktSellBtn, &QPushButton::clicked, [=]() {

            if(qtyInput->text().isEmpty())
                return;

            Order order;

            order.id = nextOrderId++;
            order.side = "MKT_SELL";
            order.price = 0;
            order.qty = qtyInput->text().toInt();

            logMessage(
                "ORDER QUEUED -> MARKET SELL " +
                QString::number(order.qty)
            );

            {
                std::lock_guard<std::mutex> lock(queueMutex);

                orderQueue.push(order);
            }

            cv.notify_one();
        });
    }

    ~TradingDashboard()
    {
        running = false;

        cv.notify_all();

        if(matchingThread.joinable())
            matchingThread.join();
    }

private:

    // ================= UI =================

    QLabel *bestBidLabel;
    QLabel *bestAskLabel;
    QLabel *spreadLabel;
    QLabel *pnlLabel;
    QLabel *positionLabel;
    QLabel *avgPriceLabel;
    QLabel *latencyLabel;

    QLineEdit *priceInput;
    QLineEdit *qtyInput;
    QLineEdit *cancelInput;

    QTextEdit *logs;

    QTableWidget *buyTable;
    QTableWidget *sellTable;
    QTableWidget *tradeTable;
    QTableWidget *depthTable;

    // ================= DATA =================

    std::vector<Order> buyOrders;
    std::vector<Order> sellOrders;
    std::vector<Trade> trades;

    int nextOrderId = 1;

    int position = 0;
    int pnl = 0;
    int avgPrice = 0;

    // ================= THREAD =================

    std::thread matchingThread;

    std::queue<Order> orderQueue;

    std::mutex queueMutex;

    std::condition_variable cv;

    bool running = true;

    // ================= PROCESS ENGINE =================

    void processOrders()
    {
        while(running)
        {
            std::unique_lock<std::mutex> lock(queueMutex);

            cv.wait(lock, [this] {

                return !orderQueue.empty() || !running;
            });

            if(!running)
                break;

            Order order = orderQueue.front();

            orderQueue.pop();

            lock.unlock();

            QMetaObject::invokeMethod(this, [=]() {

                if(order.side == "BUY")
                {
                    addBuyOrder(order);
                }

                else if(order.side == "SELL")
                {
                    addSellOrder(order);
                }

                else if(order.side == "MKT_BUY")
                {
                    marketBuy(order.qty);
                }

                else if(order.side == "MKT_SELL")
                {
                    marketSell(order.qty);
                }

            }, Qt::QueuedConnection);
        }
    }

    // ================= STYLE =================

    void styleTable(QTableWidget *table)
    {
        table->horizontalHeader()->setStretchLastSection(true);

        table->horizontalHeader()->setSectionResizeMode(
            QHeaderView::Stretch
        );

        table->setStyleSheet(
            "QTableWidget {"
            "background:#111;"
            "color:white;"
            "font-size:14px;"
            "gridline-color:gray;"
            "}"
            "QHeaderView::section {"
            "background:#333;"
            "color:white;"
            "font-size:14px;"
            "padding:6px;"
            "}"
        );
    }

    // ================= LOG =================

    void logMessage(QString msg)
    {
        logs->append(msg);
    }

    // ================= BUY =================

    void addBuyOrder(Order order)
    {
        QElapsedTimer timer;

        timer.start();

        buyOrders.push_back(order);

        std::sort(
            buyOrders.begin(),
            buyOrders.end(),
            [](Order a, Order b) {

                if(a.price == b.price)
                    return a.id < b.id;

                return a.price > b.price;
            }
        );

        logMessage(
            "BUY ORDER ADDED -> " +
            QString::number(order.qty) +
            " @ " +
            QString::number(order.price)
        );

        matchOrders();

        refreshTables();

        latencyLabel->setText(
            "Execution : " +
            QString::number(timer.nsecsElapsed()/1000) +
            " us"
        );
    }

    // ================= SELL =================

    void addSellOrder(Order order)
    {
        QElapsedTimer timer;

        timer.start();

        sellOrders.push_back(order);

        std::sort(
            sellOrders.begin(),
            sellOrders.end(),
            [](Order a, Order b) {

                if(a.price == b.price)
                    return a.id < b.id;

                return a.price < b.price;
            }
        );

        logMessage(
            "SELL ORDER ADDED -> " +
            QString::number(order.qty) +
            " @ " +
            QString::number(order.price)
        );

        matchOrders();

        refreshTables();

        latencyLabel->setText(
            "Execution : " +
            QString::number(timer.nsecsElapsed()/1000) +
            " us"
        );
    }

    // ================= MATCH =================

    void matchOrders()
    {
        while(!buyOrders.empty() &&
              !sellOrders.empty())
        {
            Order &buy = buyOrders.front();

            Order &sell = sellOrders.front();

            if(buy.price < sell.price)
                break;

            int qty = std::min(
                buy.qty,
                sell.qty
            );

            Trade t;

            t.price = sell.price;
            t.qty = qty;
            t.buyId = buy.id;
            t.sellId = sell.id;

            trades.push_back(t);

            logMessage(
                "TRADE EXECUTED -> " +
                QString::number(qty) +
                " @ " +
                QString::number(sell.price)
            );

            position += qty;

            pnl -= qty * sell.price;

            avgPrice = sell.price;

            buy.qty -= qty;
            sell.qty -= qty;

            if(buy.qty == 0)
                buyOrders.erase(buyOrders.begin());

            if(sell.qty == 0)
                sellOrders.erase(sellOrders.begin());
        }

        updateStats();
    }

    // ================= MARKET BUY =================

    void marketBuy(int qty)
    {
        while(qty > 0 &&
              !sellOrders.empty())
        {
            Order &sell = sellOrders.front();

            int traded = std::min(
                qty,
                sell.qty
            );

            Trade t;

            t.price = sell.price;
            t.qty = traded;
            t.buyId = -1;
            t.sellId = sell.id;

            trades.push_back(t);

            logMessage(
                "MARKET BUY matched " +
                QString::number(traded) +
                " @ " +
                QString::number(sell.price)
            );

            qty -= traded;

            sell.qty -= traded;

            position += traded;

            pnl -= traded * sell.price;

            avgPrice = sell.price;

            if(sell.qty == 0)
                sellOrders.erase(sellOrders.begin());
        }

        refreshTables();

        updateStats();
    }

    // ================= MARKET SELL =================

    void marketSell(int qty)
    {
        while(qty > 0 &&
              !buyOrders.empty())
        {
            Order &buy = buyOrders.front();

            int traded = std::min(
                qty,
                buy.qty
            );

            Trade t;

            t.price = buy.price;
            t.qty = traded;
            t.buyId = buy.id;
            t.sellId = -1;

            trades.push_back(t);

            logMessage(
                "MARKET SELL matched " +
                QString::number(traded) +
                " @ " +
                QString::number(buy.price)
            );

            qty -= traded;

            buy.qty -= traded;

            position -= traded;

            pnl += traded * buy.price;

            avgPrice = buy.price;

            if(buy.qty == 0)
                buyOrders.erase(buyOrders.begin());
        }

        refreshTables();

        updateStats();
    }

    // ================= TABLES =================

    void refreshTables()
    {
        buyTable->setRowCount(buyOrders.size());

        for(int i=0;i<buyOrders.size();i++)
        {
            buyTable->setItem(i,0,
                new QTableWidgetItem(
                    QString::number(buyOrders[i].id)));

            buyTable->setItem(i,1,
                new QTableWidgetItem(
                    QString::number(buyOrders[i].price)));

            buyTable->setItem(i,2,
                new QTableWidgetItem(
                    QString::number(buyOrders[i].qty)));
        }

        sellTable->setRowCount(sellOrders.size());

        for(int i=0;i<sellOrders.size();i++)
        {
            sellTable->setItem(i,0,
                new QTableWidgetItem(
                    QString::number(sellOrders[i].id)));

            sellTable->setItem(i,1,
                new QTableWidgetItem(
                    QString::number(sellOrders[i].price)));

            sellTable->setItem(i,2,
                new QTableWidgetItem(
                    QString::number(sellOrders[i].qty)));
        }

        tradeTable->setRowCount(trades.size());

        for(int i=0;i<trades.size();i++)
        {
            tradeTable->setItem(i,0,
                new QTableWidgetItem(
                    QString::number(trades[i].price)));

            tradeTable->setItem(i,1,
                new QTableWidgetItem(
                    QString::number(trades[i].qty)));

            tradeTable->setItem(i,2,
                new QTableWidgetItem(
                    QString::number(trades[i].buyId)));

            tradeTable->setItem(i,3,
                new QTableWidgetItem(
                    QString::number(trades[i].sellId)));
        }

        refreshDepth();
    }

    // ================= DEPTH =================

    void refreshDepth()
    {
        depthTable->setRowCount(0);

        int rows = std::max(
            buyOrders.size(),
            sellOrders.size()
        );

        depthTable->setRowCount(rows);

        for(int i=0;i<rows;i++)
        {
            if(i < buyOrders.size())
            {
                depthTable->setItem(i,0,
                    new QTableWidgetItem(
                        QString::number(
                            buyOrders[i].qty)));

                depthTable->setItem(i,1,
                    new QTableWidgetItem(
                        QString::number(
                            buyOrders[i].price)));
            }

            if(i < sellOrders.size())
            {
                depthTable->setItem(i,2,
                    new QTableWidgetItem(
                        QString::number(
                            sellOrders[i].qty)));
            }
        }
    }

    // ================= STATS =================

    void updateStats()
    {
        if(!buyOrders.empty())
        {
            bestBidLabel->setText(
                "BEST BID : " +
                QString::number(
                    buyOrders.front().price));
        }
        else
        {
            bestBidLabel->setText(
                "BEST BID : -");
        }

        if(!sellOrders.empty())
        {
            bestAskLabel->setText(
                "BEST ASK : " +
                QString::number(
                    sellOrders.front().price));
        }
        else
        {
            bestAskLabel->setText(
                "BEST ASK : -");
        }

        if(!buyOrders.empty() &&
           !sellOrders.empty())
        {
            int spread =
                sellOrders.front().price -
                buyOrders.front().price;

            spreadLabel->setText(
                "SPREAD : " +
                QString::number(spread));
        }
        else
        {
            spreadLabel->setText(
                "SPREAD : -");
        }

        pnlLabel->setText(
            "PnL : " +
            QString::number(pnl));

        positionLabel->setText(
            "Position : " +
            QString::number(position));

        avgPriceLabel->setText(
            "Avg Price : " +
            QString::number(avgPrice));
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TradingDashboard w;

    w.show();

    return app.exec();
}