# HTTP Calculator Server

`GET /calc?query=2+10 HTTP/1.1`に対して、計算結果を返す HTTP Server

## 前提

- 計算は2つの自然数の加減乗除にのみに対応し、結果が少数になる場合は対応しない
- サーバーは1リクエストごとに終了する
- ポート番号は 1024～49151 の範囲で指定する

## ビルド方法

### サーバーのみをビルド

```bash
make server
```

### クライアントのみをビルド

```bash
make client
```

### テスト

```bash
make test
```


## 使用方法

### サーバー起動

```bash
./http_server 8080
```


## TODO

### 必須

- [x] clientからの接続を受け入れるserverを作成する
- [x] serverに接続できるclientを作成する
- [x] serverから文字列を送信して、clientが文字列を受け取れるようにする
- [x] clientがserverから受け取った文字列をserverに返せるようにする
- [x] 文字列"1+1"のような2つの文字の四則演算の結果を返す
- [x] serverがHTTPリクエスト情報をパースできるようにする
- [x] 計算結果をHTTPレスポンスで返却する


### 任意

- [ ] IPv4+v6 両対応
- [ ] non-blocking 化
- [ ] マルチスレッド化
- [ ] 通信タイムアウトの設定
- [ ] signal を受け取ったら、全コネクションが正常終了して終了
- [ ] SSL化
