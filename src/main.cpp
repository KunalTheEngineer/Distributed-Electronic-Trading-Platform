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
#include <QMessageBox>

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <sstream>

#include <boost/asio.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <iostream>

#include "network/TcpServer.h"
#include "network/TcpClient.h"
#include "shared/Order.h"
#include "shared/Trade.h"

class TradingDashboard : public QMainWindow
{
    public:

            QFrame* createStatCard(const QString& title, QLabel*& valueLabel, const QString& color)
                {
                    QFrame* card = new QFrame();

                    card->setFrameShape(QFrame::StyledPanel);

                    card->setStyleSheet(QString(R"(
                    
                     QFrame{
                        background:#1F1F1F;
                        border:2px solid %1;
                        border-radius:12px;
                    }
                    )").arg(color));

                    QVBoxLayout* layout = new QVBoxLayout(card);

                    layout->setContentsMargins(10, 10, 10, 10);
                    layout->setSpacing(4);

                    QLabel* titleLabel = new QLabel(title);

                    titleLabel->setAlignment(Qt::AlignCenter);

                    titleLabel->setStyleSheet(R"(
                    font-size:11px;
                    font-weight:bold;
                    color:#CFCFCF;
                    border:none;
                )");       

                    valueLabel = new QLabel("-");

                    valueLabel->setAlignment(Qt::AlignCenter);

                    valueLabel->setFixedHeight(35);

                    valueLabel->setStyleSheet(QString(R"(
                    font-size:28px;
                    font-weight:bold;
                    color:%1;
                    border:none;
                )").arg(color));

                    layout->addWidget(titleLabel);
                    layout->addWidget(valueLabel);

                    card->setFixedSize(200,80);
                    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

                    return card;
                }

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

                QVBoxLayout *statsLayout = new QVBoxLayout();

                QHBoxLayout *statsRow1 = new QHBoxLayout();

                QHBoxLayout *statsRow2 = new QHBoxLayout();

                statsRow1->setSpacing(10);
                statsRow2->setSpacing(10);

                statsRow1->setContentsMargins(0,0,0,0);
                statsRow2->setContentsMargins(0,0,0,0);

               
                bestAskLabel = new QLabel("BEST ASK : -");
                spreadLabel = new QLabel("SPREAD : -");
                pnlLabel = new QLabel("PnL : 0");
                positionLabel = new QLabel("Position : 0");
                totalTradesLabel = new QLabel("TOTAL TRADES : 0");
                avgPriceLabel = new QLabel("Avg Price : 0");
                latencyLabel = new QLabel("Execution : 0 us");
                totalVolumeLabel = new QLabel("TOTAL VOLUME : 0");
                vwapLabel = new QLabel("VWAP : 0.00");
                
                QFrame *bestAskCard = createStatCard("BEST ASK", bestAskLabel, "red");

                QFrame* bestBidCard = createStatCard("BEST BID", bestBidLabel, "lime");

                QFrame *spreadCard = createStatCard("SPREAD", spreadLabel, "cyan");

                QFrame *pnlCard = createStatCard("P&L", pnlLabel, "yellow");

                QFrame *positionCard = createStatCard("POSITION", positionLabel, "orange");

                QFrame *avgPriceCard = createStatCard("AVG PRICE", avgPriceLabel, "white");

                QFrame *vwapCard = createStatCard("VWAP", vwapLabel, "springgreen");

                QFrame *tradesCard = createStatCard("TOTAL TRADES", totalTradesLabel, "deepskyblue");

                QFrame *volumeCard = createStatCard("TOTAL VOLUME", totalVolumeLabel, "gold");

                QFrame *latencyCard = createStatCard("LATENCY", latencyLabel, "magenta");

             
                // Row 1
                statsRow1->addWidget(bestBidCard);
                statsRow1->addWidget(bestAskCard);
                statsRow1->addWidget(spreadCard);
                statsRow1->addWidget(positionCard);
                statsRow1->addWidget(avgPriceCard);
            
                // Row 2
                statsRow2->addWidget(pnlCard);
                statsRow2->addWidget(vwapCard);
                statsRow2->addWidget(tradesCard);
                statsRow2->addWidget(volumeCard);
                statsRow2->addWidget(latencyCard);

                statsLayout->addLayout(statsRow1);
                statsLayout->addLayout(statsRow2);

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

               buyBtn->setFixedSize(90,45);

                buyBtn->setStyleSheet(
                "QPushButton{"
                "background:#00AA00;"
                "color:white;"
                "font-size:16px;"
                "font-weight:bold;"
                "border:none;"
                "border-radius:8px;"
                "}"
                "QPushButton:hover{background:#00CC00;}"
                );
                                sellBtn->setFixedSize(90,45);

                sellBtn->setStyleSheet(
                "QPushButton{"
                "background:#CC0000;"
                "color:white;"
                "font-size:16px;"
                "font-weight:bold;"
                "border:none;"
                "border-radius:8px;"
                "}"
                "QPushButton:hover{background:#FF2222;}"
                );
                            mktBuyBtn->setFixedSize(110, 45);

                mktBuyBtn->setStyleSheet(
                "QPushButton{"
                "background:#005CFF;"
                "color:white;"
                "font-size:15px;"
                "font-weight:bold;"
                "border:none;"
                "border-radius:8px;"
                "}"
                "QPushButton:hover{"
                "background:#1E7CFF;"
                "}"
                "QPushButton:pressed{"
                "background:#0045C7;"
                "}"
                );
                                mktSellBtn->setFixedSize(110, 45);

                mktSellBtn->setStyleSheet(
                "QPushButton{"
                "background:#8A00C8;"
                "color:white;"
                "font-size:15px;"
                "font-weight:bold;"
                "border:none;"
                "border-radius:8px;"
                "}"
                "QPushButton:hover{"
                "background:#A020F0;"
                "}"
                "QPushButton:pressed{"
                "background:#7000A5;"
                "}"
                );
                                cancelBtn->setFixedSize(100, 45);

                cancelBtn->setStyleSheet(
                "QPushButton{"
                "background:#F39C12;"
                "color:black;"
                "font-size:15px;"
                "font-weight:bold;"
                "border:none;"
                "border-radius:8px;"
                "}"
                "QPushButton:hover{"
                "background:#FFB733;"
                "}"
                "QPushButton:pressed{"
                "background:#D68910;"
                "}"
                );

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
                    "font-size:22px;"
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
                    "font-size:22px;"
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

                tradeTable->setColumnCount(2);

                tradeTable->setHorizontalHeaderLabels(
                    {"PRICE", "QTY"}
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

                depthTable->setColumnCount(4);

                depthTable->setHorizontalHeaderLabels(
                    {"BUY QTY", "BID PRICE", "ASK PRICE", "SELL QTY"}
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

                if (!client.connect("127.0.0.1", 50000))
                {
                    QMessageBox::critical(
                        this,
                        "Connection Error",
                        "Failed to connect to Trading Server."
                    );
                }

                listenerThread = std::thread(&TradingDashboard::listenToServer, this);

                // ================= BUTTONS =================

                connect(buyBtn, &QPushButton::clicked, [=]() 
                {

                    if(priceInput->text().isEmpty() ||
                    qtyInput->text().isEmpty())
                        return;

                    Order order(
                        nextOrderId++,
                        1,
                        "AAPL",
                        priceInput->text().toDouble(),
                        qtyInput->text().toInt(),
                        OrderType::BUY
                    );

                    logMessage(
                        "ORDER SENT -> BUY " +
                        QString::number(order.quantity) +
                        " @ " +
                        QString::number(order.price)
                    );

                    client.sendOrder(order);
                });

                connect(sellBtn, &QPushButton::clicked, [=]() 
                {

                    if(priceInput->text().isEmpty() ||
                    qtyInput->text().isEmpty())
                        return;

                    Order order(
                        nextOrderId++,
                        1,
                        "AAPL",
                        priceInput->text().toDouble(),
                        qtyInput->text().toInt(),
                        OrderType::SELL
                    );

                    logMessage
                    (
                        "ORDER SENT -> SELL " +
                        QString::number(order.quantity) +
                        " @ " +
                        QString::number(order.price)
                    );

                    client.sendOrder(order);
                });

                connect(mktBuyBtn, &QPushButton::clicked, [=]() 
                {

                    if(qtyInput->text().isEmpty())
                        return;

                    Order order(
                        nextOrderId++,
                        1,
                        "AAPL",
                        0,
                        qtyInput->text().toInt(),
                        OrderType::MARKET_BUY
                    );

                        logMessage(
                            "ORDER QUEUED -> MARKET BUY " +
                            QString::number(order.quantity)
                        );

                       client.sendOrder(order);
                });

                connect(mktSellBtn, &QPushButton::clicked, [=]() 
                {

                    if(qtyInput->text().isEmpty())
                        return;

                    Order order(
                        nextOrderId++,
                        1,
                        "AAPL",
                        0,
                        qtyInput->text().toInt(),
                        OrderType::MARKET_SELL
                    );

                    logMessage(
                        "ORDER QUEUED -> MARKET SELL " +
                        QString::number(order.quantity)
                    );

                    client.sendOrder(order);
                });

                connect(cancelBtn, &QPushButton::clicked, [=]()
                {
                    if (cancelInput->text().isEmpty())
                        return;

                    int orderId = cancelInput->text().toInt();

                    logMessage(
                        "CANCEL REQUEST -> Order " +
                        QString::number(orderId)
                    );

                    client.sendCancelOrder(orderId);
                });
            } // CONSTRCUTOR END

            ~TradingDashboard()
            {
                running = false;

                if (listenerThread.joinable())
                {
                    listenerThread.join();
                }
            } // DESTRUCTOR END

    private:

            // ================= UI =================

            QLabel *bestBidLabel;
            QLabel *bestAskLabel;
            QLabel *spreadLabel;
            QLabel *pnlLabel;
            QLabel *positionLabel;
            QLabel *avgPriceLabel;
            QLabel *latencyLabel;
            QLabel* totalTradesLabel;
            QLabel* totalVolumeLabel;
            QLabel* vwapLabel;

            QLineEdit *priceInput;
            QLineEdit *qtyInput;
            QLineEdit *cancelInput;

            QTextEdit *logs;

            QTableWidget *buyTable;
            QTableWidget *sellTable;
            QTableWidget *tradeTable;
            QTableWidget *depthTable;

            TcpClient client;
            std::thread listenerThread;

            bool running = true;

            // ================= DATA =================

            int nextOrderId = 1;

            int position = 0;
            int pnl = 0;
            int avgPrice = 0;

            // ================= STYLE =================

            void styleTable(QTableWidget *table)
            {
                table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

                table->verticalHeader()->setVisible(false);

                table->setEditTriggers(QAbstractItemView::NoEditTriggers);

                table->setSelectionBehavior(QAbstractItemView::SelectRows);

                table->setSelectionMode(QAbstractItemView::SingleSelection);

                table->setCornerButtonEnabled(false);

                table->verticalHeader()->setDefaultSectionSize(30);

                table->setAlternatingRowColors(true);

                table->setStyleSheet(R"(

            QTableWidget
            {
                background-color:#151515;
                alternate-background-color:#1E1E1E;
                color:white;
                gridline-color:#404040;
                border:2px solid #3A3A3A;
                font-size:15px;
                selection-background-color:#0078D7;
                selection-color:white;
            }

            QHeaderView::section
            {
                background-color:#2A2A2A;
                color:white;
                font-size:14px;
                font-weight:bold;
                border:none;
                height:35px;
            }

            )");
            }

            // ================= LOG =================

            void logMessage(QString msg)
            {
                logs->append(msg);
            }
            
            void updateBuyTable(const std::string& snapshot);

            void updateSellTable(const std::string& snapshot);

            void updateTradeTable(const std::string& snapshot);

            void updateDepthTable(const std::string& snapshot);

            void updateMarketInfo(const std::string& snapshot);

            void updateStats(const std::string& snapshot);

            void listenToServer();
};

            void TradingDashboard::updateBuyTable(const std::string& snapshot)
            {
                buyTable->setRowCount(0);

                std::stringstream ss(snapshot);
                std::string line;

                bool readingBuy = false;
                int row = 0;

                while (std::getline(ss, line))
                {
                    if (line == "BUY")
                    {
                        readingBuy = true;
                        continue;
                    }

                    if (line == "END_BUY")
                        break;

                    if (!readingBuy)
                        continue;

                    std::stringstream rowStream(line);

                    std::string id;
                    std::string price;
                    std::string qty;

                    std::getline(rowStream, id, ',');
                    std::getline(rowStream, price, ',');
                    std::getline(rowStream, qty, ',');

                    buyTable->insertRow(row);

                    buyTable->setItem(row,0,new QTableWidgetItem(QString::fromStdString(id)));
                    buyTable->setItem(row,1,new QTableWidgetItem(QString::fromStdString(price)));
                    buyTable->setItem(row,2,new QTableWidgetItem(QString::fromStdString(qty)));

                    row++;
                }
            }

            void TradingDashboard::updateSellTable(const std::string& snapshot)
            {
                sellTable->setRowCount(0);

                std::stringstream ss(snapshot);
                std::string line;

                bool readingSell = false;
                int row = 0;

                while (std::getline(ss, line))
                {
                    if (line == "SELL")
                    {
                        readingSell = true;
                        continue;
                    }

                    if (line == "END_SELL")
                        break;

                    if (!readingSell)
                        continue;

                    std::stringstream rowStream(line);

                    std::string id;
                    std::string price;
                    std::string qty;

                    std::getline(rowStream, id, ',');
                    std::getline(rowStream, price, ',');
                    std::getline(rowStream, qty, ',');

                    sellTable->insertRow(row);

                    sellTable->setItem(row,0,new QTableWidgetItem(QString::fromStdString(id)));
                    sellTable->setItem(row,1,new QTableWidgetItem(QString::fromStdString(price)));
                    sellTable->setItem(row,2,new QTableWidgetItem(QString::fromStdString(qty)));

                    row++;
                }
            }

            void TradingDashboard::updateTradeTable(const std::string& snapshot)
            {
                tradeTable->setRowCount(0);

                std::stringstream ss(snapshot);
                std::string line;

                bool readingTrade = false;
                int row = 0;

                while (std::getline(ss, line))
                {
                    if (line == "TRADES")
                    {
                        readingTrade = true;
                        continue;
                    }

                    if (line == "END_TRADES")
                        break;

                    if (!readingTrade)
                        continue;

                    std::stringstream rowStream(line);

                    std::string price;
                    std::string qty;

                    std::getline(rowStream, price, ',');
                    std::getline(rowStream, qty, ',');

                    tradeTable->insertRow(row);

                    tradeTable->setItem(
                    row, 0,
                    new QTableWidgetItem(QString::fromStdString(price))
                         );

                    tradeTable->setItem(
                        row, 1,
                        new QTableWidgetItem(QString::fromStdString(qty))
                    );

                    row++;
                }
            }

            void TradingDashboard::updateDepthTable(const std::string& snapshot)
            {
                depthTable->setRowCount(0);

                std::stringstream ss(snapshot);
                std::string line;

                bool readingDepth = false;
                int row = 0;

                while (std::getline(ss, line))
                {
                    if (line == "DEPTH")
                    {
                        readingDepth = true;
                        continue;
                    }

                    if (line == "END_DEPTH")
                        break;

                    if (!readingDepth)
                        continue;

                    if (line.empty())
                        continue;

                    std::stringstream rowStream(line);

                    std::string buyQty;
                    std::string bidPrice;
                    std::string askPrice;
                    std::string sellQty;

                    std::getline(rowStream, buyQty, ',');
                    std::getline(rowStream, bidPrice, ',');
                    std::getline(rowStream, askPrice, ',');
                    std::getline(rowStream, sellQty, ',');

                    depthTable->insertRow(row);

                    depthTable->setItem(
                        row,
                        0,
                        new QTableWidgetItem(QString::fromStdString(buyQty))
                    );

                    depthTable->setItem(
                        row,
                        1,
                        new QTableWidgetItem(QString::fromStdString(bidPrice))
                    );

                    depthTable->setItem(
                        row,
                        2,
                        new QTableWidgetItem(QString::fromStdString(askPrice))
                    );

                    depthTable->setItem(
                        row,
                        3,
                        new QTableWidgetItem(QString::fromStdString(sellQty))
                    );

                    row++;
                }
            }

            void TradingDashboard::updateMarketInfo(const std::string& snapshot)
            {
                    std::stringstream ss(snapshot);
                    std::string line;

                    bool readingMarket = false;

                    while (std::getline(ss, line))
                    {
                        if (line == "MARKET")
                        {
                            readingMarket = true;
                            continue;
                        }

                        if (line == "END_MARKET")
                            break;

                        if (!readingMarket)
                            continue;

                        std::stringstream rowStream(line);

                        std::string bestBid;
                        std::string bestAsk;
                        std::string spread;
                        std::string position;

                        std::getline(rowStream, bestBid, ',');
                        std::getline(rowStream, bestAsk, ',');
                        std::getline(rowStream, spread, ',');
                        std::getline(rowStream, position, ',');

                    double bid = std::stod(bestBid);
                    bestBidLabel->setText(QString::number(bid, 'f', 2));

                    bestAskLabel->setText(QString::number(std::stod(bestAsk), 'f', 2));

                    spreadLabel->setText(QString::number(std::stod(spread), 'f', 2));

                    positionLabel->setText(QString::fromStdString(position));

                        break;
                    }
            }

           void TradingDashboard::updateStats(const std::string& snapshot)
            {
                std::stringstream ss(snapshot);
                std::string line;

                bool readingStats = false;

                while (std::getline(ss, line))
                {
                    if (line == "STATS")
                    {
                        readingStats = true;
                        continue;
                    }

                    if (line == "END_STATS")
                        break;

                    if (!readingStats)
                        continue;

                    std::stringstream rowStream(line);

                    std::string totalTrades;
                    std::string totalVolume;
                    std::string vwap;
                    std::string avgPrice;
                    std::string latency;

                    std::getline(rowStream, totalTrades, ',');
                    std::getline(rowStream, totalVolume, ',');
                    std::getline(rowStream, vwap, ',');
                    std::getline(rowStream, avgPrice, ',');
                    std::getline(rowStream, latency, ',');

                    totalTradesLabel->setText(QString::fromStdString(totalTrades));

                    totalVolumeLabel->setText(QString::fromStdString(totalVolume));

                    vwapLabel->setText(QString::number(std::stod(vwap), 'f', 2));

                    avgPriceLabel->setText(QString::number(std::stod(avgPrice), 'f', 2));

                    latencyLabel->setText(QString::fromStdString(latency) + " μs");

                    break;
                }
            }
            
            void TradingDashboard::listenToServer()
            {
                while (running)
                {
                    try
                    {
                        std::string reply = client.receiveMessage();

                        QMetaObject::invokeMethod(
                            this,
                            [this, reply]()
                            {
                                updateBuyTable(reply);
                                updateSellTable(reply);
                                updateTradeTable(reply);
                                updateDepthTable(reply);
                                updateMarketInfo(reply);
                                updateStats(reply);
                            },
                            Qt::QueuedConnection
                        );
                    }
                    catch (...)
                    {
                        break;
                    }
                }
            }


    int main(int argc, char *argv[])
    {
        
        QApplication app(argc, argv);

        TradingDashboard w;

        w.show();

        int result = app.exec();

        return result;

    }