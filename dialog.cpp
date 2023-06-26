#include "dialog.h"
#include "ui_dialog.h"

#include <thread>

#include <httplib.h>
#include <json.hpp>

//using namespace httplib;
//using json = nlohmann::json;

/*
json解析注意事项：
解析的类型和传递的须一致，否则无法继续，可能抛出异常。
如字段本应为int，但传入字符串，则会异常。
*/

/*
curl -s -X POST http://127.0.0.1:8080/foobar -H "Content-Type:application/json" -d  '{"name":"Late Lee","timestamp":"2023-01-01 00:12:34","version":"250","age":44,"sign":false,"data":{"type":100}}'  | json
{
  "a": "Late Lee",
  "b": "250",
  "cities": [
    [
      1,
      "南宁"
    ],
    [
      2,
      "梧州"
    ],
    [
      3,
      "岑溪"
    ]
  ],
  "data": {
    "age": 44
  },
  "ids": [
    "id1",
    "id2",
    "id100"
  ],
  "nums": [
    1,
    2,
    3,
    4,
    5
  ]
}
*/

std::string gVersion = "1.0";

void handlePostFoobar(const httplib::Request &req, httplib::Response &res)
{
    qDebug("start!!!!!\n");
    qDebug("req: %d\n", req.body.size()); //  get_param_value("encryptType").c_str());
    qDebug("req: [%s]\n", req.body.data()); // get_param_value("version").c_str());

    if (req.has_header("Content-Type")) {
        auto val = req.get_header_value("Content-Type");
        qDebug("value: %s", val.data());
        if (val.compare("application/json") != 0) {
            nlohmann::json outJson;
            outJson["err"] = "not json";
            std::string jsonStr = outJson.dump();
            res.set_content(jsonStr, "application/json");
            return;
        }
    }

    nlohmann::json myjson = nlohmann::json::parse(req.body);

    if (myjson == nullptr)
    {
        nlohmann::json outJson;
        outJson["err"] = "not json";
        std::string jsonStr = outJson.dump();
        res.set_content(jsonStr, "application/json");
        return;
    }

    std::string name;
    int age;
    std::string version;
    std::string timestamp;
    bool isSigned = false;
    int myType = 0;
    try {
        myjson.at("name").get_to(name);
        myjson.at("age").get_to(age);
        myjson.at("version").get_to(version);
        // this is ok
    //    myjson.at("timestamp").get_to(info.timestamp);
        // this is also ok
        timestamp = myjson.at("timestamp").get<std::string>();

        // for bool
        isSigned = myjson["sign"].get<bool>();

        // data字段也是一个json对象
        nlohmann::json datajson = myjson["data"];
        myType = datajson["type"].get<int>();
    }
    catch(std::exception& e) // 异常
    {
        qDebug("json parse failed %s", e.what());
    }


    qDebug("result: %s %d %s %s %d %d\n", name.data(), age, version.data(), timestamp.data(), isSigned, myType);

    // 输出
    nlohmann::json outJson;
    outJson["a"] = name;
    outJson["b"] = version;
    outJson["nums"] = {1, 2, 3, 4, 5};
//    outJson["ids"] = {"id1", "id2", "id3", "id4", "id5"};
    // for vector
    std::vector<std::string> vec;
    vec.push_back("id1");
    vec.push_back("id2");
    vec.push_back("id100");
    outJson["ids"] = vec;

    // for map
    std::map<int, std::string> m;
    m.insert({1, "南宁"});
    m.insert({2, "梧州"});
    m.insert({3, "岑溪"});
    outJson["cities"] = m;

    // 子对象
    nlohmann::json outData;
    outData["age"] = age;
    outJson["data"] = outData;

    std::string jsonStr = outJson.dump();

    qDebug("end: %s", jsonStr.data());

     res.set_content(jsonStr, "application/json");
}

void startHttpServer()
{
    httplib::Server svr;

    svr.Get("/version", [](const httplib::Request& /*req*/, httplib::Response& res) {
      res.set_content(gVersion, "text/plain");
    });

#if 0
    svr.Get(R"(/numbers/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
      auto numbers = req.matches[1];
      res.set_content(numbers, "text/plain");
    });

    svr.Get("/body-header-param", [](const httplib::Request& req, httplib::Response& res) {
      if (req.has_header("Content-Length")) {
        auto val = req.get_header_value("Content-Length");
      }
      if (req.has_param("key")) {
        auto val = req.get_param_value("key");
      }
    res.set_content(req.body, "text/plain");
  });

  svr.Get("/stop", [&](const httplib::Request& req, httplib::Response& res) {
    svr.stop();
  });
#endif

  svr.Post("/foobar", handlePostFoobar);

  int port = 8080;
  qDebug("start listen at %d\n", port);
  svr.listen("0.0.0.0", port);
}

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("MainWindow Demo"));
    setMinimumSize(480, 320);

    Qt::WindowFlags winFlags  = Qt::Dialog;
    winFlags = winFlags | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint;
    //winFlags = Qt::WindowFullscreenButtonHint;
    setWindowFlags(winFlags);
}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::on_btnStart_clicked()
{
    ui->btnStart->setEnabled(false);

    std::thread thttp(startHttpServer);
    thttp.detach();
}
