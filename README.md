# plotinc ver.1.3
                                                by Zhidao
                                                2025.02.07. 作成
                                                2025.02.10. 最終更新
--------------------------------------------------------------------
### 【説明】

C言語プログラムの中から呼べるグラフ描画ライブラリ。
Cairoを使います。

また、platex、dvips、pstopnm、ImageMagickがあれば、TeX形式で書いた数式
をラベル中に埋め込めます。

--------------------------------------------------------------------
### 【インストール】

```sh
% make && make install
```
して下さい。
デフォルトでは、libplotinc.soを~/usr/libの下に、ヘッダファイルを~/usr/include/plotinc
の下にそれぞれコピーします。コピー先を変えたい場合は makefile の頭にある
変数をいじって下さい。
また、LD_LIBRARY_PATHを適切に設定して下さい。ご参考までに、Bourne shell
の場合は
```sh
% export LD_LIBRARY_PATH=~/usr/lib:$LD_LIBRARY_PATH
```
などとします。

--------------------------------------------------------------------
### 【アンインストール】

```sh
% make uninstall
```
して下さい。

--------------------------------------------------------------------
### 【使い方】

X11上でのグラフ描画の例 example/x11_test.c
SVGへのグラフ出力の例 example/svg_test.c
を見て下さい。

makefileの書き方は example/makefile を見て下さい。

--------------------------------------------------------------------
### 【免責事項】

本ライブラリを利用した結果起こったいかなる事態に対しても作者は責任を負いません。

--------------------------------------------------------------------
### 【著作権など】

MITライセンスを適用します。LICENSEを読んで下さい。
機能追加のご要望はお気軽に下さい。

--------------------------------------------------------------------
### 【履歴】

2025.02.10. ver.1.3 TeX形式数式をラベルに埋め込む機能追加。
2025.02.09. ver.1.2 描画領域クリッピング機能追加。パラメトリック関数描画
            機能追加。関数描画機能再実装。
2025.02.09. ver.1.1 y2tics追加。grid表示分離。タイトル表示機能追加。
2025.02.07. ver.1.0 公開。
