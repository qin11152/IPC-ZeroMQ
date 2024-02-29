#include "widget.h"
#include "ui_widget.h"

#include "IPCDelegate.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    std::cout<< "result:" << IPCDelegate::get_mutable_instance().Init() << std::endl;
    connect(ui->pushButton,&QPushButton::clicked,this,&Widget::onBtnClicked);
    connect(ui->pushButton_2,&QPushButton::clicked,this,[=](){
        IPCDelegate::get_mutable_instance().sendMsg(TEST_TOPIC2,"qin11152");
    });
}

Widget::~Widget()
{
    IPCDelegate::get_mutable_instance().UnInit();
    delete ui;
}

void Widget::onBtnClicked()
{
    IPCDelegate::get_mutable_instance().sendMsg(TEST_TOPIC,"qin11152");
}
