# http_server

## 概要

`GET /calc?query=2+10 HTTP/1.1`に対して、<br>

`HTTP/1.1 200 OK` <br>
`Content-Length: 2` <br>

と返すHTTP Server


## TODO
### must
#### システムコールを用いたソケット通信の実現
- [*] clientからの接続を受け入れるserverを作成する
- [*] serverに接続できるclientを作成する
- [*] serverから文字列を送信して、clientが文字列を受け取れるようにする
- [*] clientがserverから受け取った文字列をserverに返せるようにする

#### 計算機の構築
- [*] 文字列"1+1"のような2つの文字の四則演算の結果を返すcalculatorを作成する
- [ ] "+-/*()"に対応して、基本的な四則演算の結果を返せるようにする

#### HTTP化
- [ ] serverがHTTPリクエスト情報をパースできるようにする
- [ ] 計算結果をHTTPレスポンスで返却する


### want
- [ ] IPv4+v6 両対応
- [ ] non-blocking 化
- [ ] マルチスレッド化
- [ ] 通信タイムアウトの設定
- [ ] signal を受け取ったら、全コネクションが正常終了して終了
- [ ] SSL化
