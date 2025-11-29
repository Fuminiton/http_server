# HTTP Calculator Server

`GET /calc?query=2+10 HTTP/1.1`に対して、計算結果を返す HTTP Server

## 前提

- 計算は2つの自然数の加減乗除にのみ対応する

## ビルド方法

```bash
make server
```

## 使用方法

### サーバー起動

```bash
./http_server 8080
```

## API仕様

### エンドポイント

**GET /calc**

クエリパラメータで指定された計算を実行する

**クエリパラメータ:**
- `query`: 計算式（形式: `<数値><演算子><数値>`）
  - 演算子: `+`, `-`, `*`, `/`
  - 例: `10+5`, `20-8`, `7*6`, `100/4`

**レスポンス**
- 成功時（200 OK）: 計算結果を平文で返す
- エラー時（400/404）: エラーメッセージを返す


## コード構造

```
├── データ構造
│   └── HTTPRequest - HTTPリクエスト
├── ユーティリティ関数
│   ├── ログ出力 (log_exit, log_error, log_info)
│   └── メモリ管理 (xmalloc, xstrdup)
├── HTTPリクエスト処理
│   ├── create_request - リクエスト作成
│   ├── parse_request - リクエストパース
│   └── free_request - リクエスト解放
├── 計算機能
│   ├── parse_calc_query - クエリパース
│   └── calculate - 計算実行
├── HTTPレスポンス生成
│   ├── build_response - 基本レスポンス構築
│   ├── build_success_response - 成功レスポンス
│   └── build_error_response - エラーレスポンス
├── リクエストハンドラ
│   ├── handle_calc_endpoint - /calc処理
│   └── handle_request - リクエストルーティング
├── ネットワーク処理
│   ├── receive_request - リクエスト受信
│   ├── service_client - クライアント処理
│   ├── create_server_socket - サーバーソケット作成
│   └── service - サーバーメインループ
└── メイン関数
```

## TODO
### 必須

- [*] clientからの接続を受け入れるserverを作成する
- [*] serverに接続できるclientを作成する
- [*] serverから文字列を送信して、clientが文字列を受け取れるようにする
- [*] clientがserverから受け取った文字列をserverに返せるようにする
- [*] 文字列"1+1"のような2つの文字の四則演算の結果を返すcalculatorを作成する
- [*] "+-/*()"に対応して、基本的な四則演算の結果を返せるようにする
- [*] serverがHTTPリクエスト情報をパースできるようにする
- [*] 計算結果をHTTPレスポンスで返却する


### 任意

- [ ] IPv4+v6 両対応
- [ ] non-blocking 化
- [ ] マルチスレッド化
- [ ] 通信タイムアウトの設定
- [ ] signal を受け取ったら、全コネクションが正常終了して終了
- [ ] SSL化
