# KKN(仮称) マニュアル
-----

森田 一 (hmorita@i.kyoto-u.ac.jp)
河原 大介 (dk@i.kyoto-u.ac.jp)
黒橋 禎夫 (kuro@i.kyoto-u.ac.jp)

## 1. 概要
**KKN(仮称)**は言語モデルを利用した高性能な形態素解析器です．言語モデルとして *Recurrent Neural Network Language Model*(RNNLM) を用いることにより，単語の並びの自然さを考慮した解析を行います．それにより JUMAN，MeCab に比べ大きく性能が向上しています．本システムは CREST「知識に基づく構造的言語処理の確立と知識インフラの構築」を通じて開発されました．

## 2. インストール
### 動作環境
- gcc (4.9.1 以降)
- git (2.4.2 で動作を確認)
- Boost C++ Libraries (1.57 以降)  
[](http://www.boost.org/users/history/version_1_57_0.html)
### 推奨ライブラリ 
導入することで，動作を高速化することができます．インストール方法は， A. 推奨ライブラリのインストールを参照してください
- gperftool  
[](https://github.com/gperftools/gperftools)
- libunwind (gperftool を64bit 環境で動作させるために必要)  
[](http://download.savannah.gnu.org/releases/libunwind/libunwind-0.99-beta.tar.gz)

### インストール手順
#### 形態素解析器のインストール. 
通常のソフトウェアと同様にmake，install できます．
```
% wget http://lotus.kuee.kyoto-u.ac.jp/nl-resource/kkn/kkn-20160325.tar.gz
% tar xzf kkn-20160325.tar.gz
% cd kkn-20160325
% ./autogen.sh
% ./configure
% make
% sudo make install
```

#### 辞書および訓練済みモデルのダウンロード
辞書およびモデル配置したパスを，設定ファイルに書き込む必要があります．
```
% wget http://lotus.kuee.kyoto-u.ac.jp/nl-resource/kkn/20160325/kkn-model-20160325.tar.gz
% tar xzf kkn-model-20160315.tar.gz
% cp -r kkn-model-20160315 /usr/local/share/kkn-model-20160325
% echo /usr/local/share/kkn-model-20160325 > ~/.kknrc
```
 
## 3. Quick Start
KKN を起動し，標準入力に UTF-8 のテキストを入力することで解析を行えます．
ただし，`\#`で始まる行はコメント行として扱い解析は行いません．
```
% echo "重み付けを変える" | kkn 
重 おも 重い 形容詞 3 * 0 イ形容詞アウオ段 18 語幹 1 "代表表記:重い/おもい 反義:形容詞:軽い/かるい"
み み み 接尾辞 14 名詞性述語接尾辞 1 * 0 * 0 "代表表記:み/み 準内容語 カテゴリ:抽象物"
付け つけ 付け 名詞 6 普通名詞 1 * 0 * 0 "代表表記:付け/つけ 補文ト 付属動詞候補（基本） 自他動詞:自:付く/つく 連用形名詞化:形態素解析"
を を を 助詞 9 格助詞 1 * 0 * 0 NIL
変える かえる 変える 動詞 2 * 0 母音動詞 1 基本形 2 "代表表記:変える/かえる 自他動詞:自:変わる/かわる"
EOS
```

出力の形式は JUMAN と同様ですが，JUMANでうまく解析できなかった文も解析できるようになっています．
```
% echo "重み付けを変える" | juman
重 じゅう 重 名詞 6 普通名詞 1 * 0 * 0 "代表表記:重/じゅう 漢字読み:音 カテゴリ:数量"
み付け みつけ み付ける 動詞 2 * 0 母音動詞 1 基本連用形 8 "代表表記:見付ける/みつける 自他動詞:自:見付かる/みつかる"
を を を 助詞 9 格助詞 1 * 0 * 0 NIL
変える かえる 変える 動詞 2 * 0 母音動詞 1 基本形 2 "代表表記:変える/かえる 自他動詞:自:変わる/かわる"
EOS
```

インストールなしで試したい場合には，Web上で形態素解析を試せるデモを用意しています．
[デモ](http://lotus.kuee.kyoto-u.ac.jp/DEMO/rnnlm.cgi)

## 4. オプション解説
KKN で利用する主なオプションは以下のとおりです．
```
オプション:
  -j, --juman                  解析結果を JUMAN 形式で出力 (default)
  -L, --lattice n              N-Best 解をラティス形式で出力
      --noambiguous            解析結果中の形態素と同じ位置に品詞が同一の複数の
                               形態素候補がある場合(曖昧性のある場合)も，形態素を
                               １つだけ表示する
  -B, --beam width             解析に利用する Beam 幅 (default: width = 5)
  -D, --dir                    モデルファイルのあるディレクトリを指定  
                               (default: .kknrc で指定したパス)
      --dynamic                RNNLM の読み込みを動的に行い，起動を高速化する (開発中)
  -v, --version                バージョンを表示
      --debug                  デバッグ出力を表示します
  -h, --help                   ヘルプを表示
```

## 5. 出力形式
### JUMAN 形式 (default)
各行が１つの形態素を表します．形態素の各項目は半角スペース区切りで表され，各項目が表す内容は以下の通りです．
```
出現形 読み 見出し語 品詞大分類 品詞大分類ID 品詞細分類 品詞細分類ID 活用形 活用形ID 活用型 活用型ID 意味情報
```

```
出力例: 
ここ ここ ここ 指示詞 7 名詞形態指示詞 1 * 0 * 0 NIL
に に に 助詞 9 格助詞 1 * 0 * 0 NIL
は は は 助詞 9 副助詞 2 * 0 * 0 NIL
いる いる いる 動詞 2 * 0 子音動詞ラ行 10 基本形 2 "代表表記:要る/いる"
@ いる いる いる 動詞 2 * 0 母音動詞 1 基本形 2 "代表表記:鋳る/いる"
@ いる いる いる 動詞 2 * 0 母音動詞 1 基本形 2 "代表表記:居る/いる"
@ いる いる いる 動詞 2 * 0 母音動詞 1 基本形 2 "代表表記:射る/いる"
@ いる いる いる 動詞 2 * 0 子音動詞ラ行 10 基本形 2 "代表表記:煎る/いる ドメイン:料理・食事"
EOS
```

解析結果中の形態素に曖昧性がある場合は，他の候補を先頭に`@`をつけて表示します．また，`EOS` は文末を表す記号です．

### ラティス形式 (-L)
後段の処理で形態素解析のN-best解を利用するため，解析結果をラティスとして出力します．各形態素が他のどの形態素と接続するかの情報が付随しているため，複数の解析結果をまとめた形式で表現することができます．JUMAN形式と同様に各行が１つの形態素をあらわしますが，形態素の各項目はタブ区切りで表されています．各項目が表す内容は以下の通りです．
```
行頭記号	形態素ID	前方向に接続する形態素IDの列	開始文字位置	終了文字位置	表層形	代表表記	読み	見出し語	品詞大分類	品詞大分類ID	品詞細分類	品詞細分類ID	活用型	活用型ID	活用形	活用形ID	意味情報
```
係り受け解析と共通のフォーマットになっているため，各行の先頭に形態素であることを表す行頭記号が付いています．行頭記号には以下の４種類があります．
* \# コメント行
* \+ フレーズ行 (形態素解析の出力では使用しません)
* \- 形態素行
* EOS 文末

```
出力例 (オプション: -L 5)
-       21      0       0       1       ここ    ここ/ここ       ここ    ここ    指示詞  7       名詞形態 指示詞  1       *       0       *       0       特徴量スコア:-1.94682|言語モデルスコア:-0.710878|形態素解析スコア:-2.6577|ランク:1;2;3;4;5
-       44      21      2       2       に      に/に   に      に      助詞    9       接続助詞        3    *       0       *       0       特徴量スコア:-0.306839|言語モデルスコア:-0.302036|形態素解析スコア:-0.608875|ランク:5
-       43      21      2       2       に      に/に   に      に      助詞    9       格助詞  1       *    0       *       0       特徴量スコア:0.572581|言語モデルスコア:-0.302036|形態素解析スコア:0.270545|ランク:1;2;3;4
-       93      44;43   3       5       はいる  入る/はいる     はいる  はいる  動詞    2       *       0子音動詞ラ行    10      基本形  2       自他動詞:他:入れる/いれる|反義:動詞:出る/でる|特徴量スコア:-0.693798|言語モデルスコア:-1.44292|形態素解析スコア:-2.13671|ランク:3
-       57      44;43   3       3       は      は/は   は      は      助詞    9       副助詞  2       *    0       *       0       特徴量スコア:0.498884|言語モデルスコア:-0.264319|形態素解析スコア:0.234565|ランク:1;2;4;5
-       137     57      4       5       いる    いる/いる       いる    いる    接尾辞  14      動詞性接 尾辞    7       母音動詞        1       基本形  2       特徴量スコア:-1.50223|言語モデルスコア:-1.30923| 形態素解析スコア:-2.81146|ランク:4
-       136     57      4       5       いる    鋳る/いる       いる    いる    動詞    2       *       0母音動詞        1       基本形  2       特徴量スコア:-1.05733|言語モデルスコア:-0.652887|形態素解析スコア:-1.71022|ランク:1;2;5
-       135     57      4       5       いる    居る/いる       いる    いる    動詞    2       *       0母音動詞        1       基本形  2       特徴量スコア:-1.05733|言語モデルスコア:-0.652887|形態素解析スコア:-1.71022|ランク:1;2;5
-       134     57      4       5       いる    射る/いる       いる    いる    動詞    2       *       0母音動詞        1       基本形  2       特徴量スコア:-1.05733|言語モデルスコア:-0.652887|形態素解析スコア:-1.71022|ランク:1;2;5
-       133     57      4       5       いる    要る/いる       いる    いる    動詞    2       *       0子音動詞ラ行    10      基本形  2       特徴量スコア:-1.05733|言語モデルスコア:-0.652887|形態素解析スコア:-1.71022|ランク:1;2;5
-       132     57      4       5       いる    煎る/いる       いる    いる    動詞    2       *       0子音動詞ラ行    10      基本形  2       ドメイン:料理・食事|特徴量スコア:-1.05733|言語モデルスコア:-0.652887|形態素解析スコア:-1.71022|ランク:1;2;5
EOS
``` 

ラティス形式では意味情報は`|`を区切りとしています．JUMANでは代表表記は意味情報の１つとして表示されていましたが，ラティス形式では意味情報とは独立に表示されます．また，ラティス形式の場合のみ意味情報に**ランク**が追加されており，その形態素が N-best 解のうち何番目の解に出現したかを表します．解析結果中の形態素に曖昧性がある場合は，同ランクの形態素が複数個表示されます．

意味情報中の**特徴量スコア**, **言語モデルスコア**, **形態素解析スコア**は, 解析時にその形態素に付与されたスコアを表します．特徴量スコアは学習した特徴量の重みによるスコア，言語モデルスコアはRNNLMによって計算されたスコア，形態素解析スコアはその合計値を表します．

## 6. 解析結果の仕様のうちJUMANと異なる点
動詞や形容詞の連用形は名詞として用いられることがあります．例をあげると，"音の響きを大切にする"という文の"響き"という言葉は，動詞の"響く"の連用形が名詞化して用いられています．JUMANではこの動詞や形容詞の名詞化を扱わず，名詞化している語も元の品詞の連用形として出力しており，名詞化は構文解析時におこなっていました．本システムでは，動詞や形容詞の連用形が名詞化して用いられている場合，名詞として出力を行います．JUMANの出力と本システムの出力を同時に扱う場合には注意が必要です．

JUMANでは "音の響きを大切にする"という文を以下の様に解析します．
```
音 おと 音 名詞 6 普通名詞 1 * 0 * 0 "代表表記:音/おと 漢字読み:訓 カテゴリ:抽象物"
@ 音 おん 音 名詞 6 普通名詞 1 * 0 * 0 "代表表記:音/おん 漢字読み:音 カテゴリ:抽象物"
の の の 助詞 9 格助詞 1 * 0 * 0 NIL
響き ひびき 響く 動詞 2 * 0 子音動詞カ行 2 基本連用形 8 "代表表記:響く/ひびく"
を を を 助詞 9 格助詞 1 * 0 * 0 NIL
大切に たいせつに 大切だ 形容詞 3 * 0 ナ形容詞 21 ダ列基本連用形 7 "代表表記:大切だ/たいせつだ 反義:形容詞:粗末だ/そまつだ"
する する する 接尾辞 14 動詞性接尾辞 7 サ変動詞 16 基本形 2 "代表表記:する/する"
EOS
```
一方，本システムでは "響き" を名詞として出力し，意味情報に形態素解析時に名詞化を行ったことを表す "連用形名詞化:形態素解析" という情報を追加します．
```
音 おん 音 名詞 6 普通名詞 1 * 0 * 0 "代表表記:音/おん 漢字読み:音 カテゴリ:抽象物"
@ 音 おと 音 名詞 6 普通名詞 1 * 0 * 0 "代表表記:音/おと 漢字読み:訓 カテゴリ:抽象物"
の の の 助詞 9 格助詞 1 * 0 * 0 NIL
響き ひびき 響き 名詞 6 普通名詞 1 * 0 * 0 "代表表記:響き/ひびき 連用形名詞化:形態素解析"
を を を 助詞 9 格助詞 1 * 0 * 0 NIL
大切に たいせつに 大切だ 形容詞 3 * 0 ナ形容詞 21 ダ列基本連用形 7 "代表表記:大切だ/たいせつだ 反義:形容 詞:粗末だ/そまつだ"
する する する 接尾辞 14 動詞性接尾辞 7 サ変動詞 16 基本形 2 "代表表記:する/する"
EOS
```

## 謝辞
言語モデルの学習には, [faster-rnnlm](https://github.com/yandex/faster-rnnlm) を利用しています．Double-Array を扱うため Taku Kudo 氏の [Darts](http://chasen.org/~taku/software/darts/)を利用しています．CDBの読み込みに [tinycdb](http://www.corpit.ru/mjt/tinycdb.html) のコードを利用しています．コマンドラインの解析に Hideyuki Tanaka 氏の [cmdline](https://github.com/tanakh/cmdline) を利用しています． 各ソフトウェア，ライブラリの製作者様に心から感謝申し上げます．

## 参考文献
"Morphological Analysis for Unsegmented Languages using Recurrent Neural Network Language Model. Hajime Morita, Daisuke Kawahara, Sadao Kurohashi. Proceedings of EMNLP 2015: Conference on Empirical Methods in Natural Language Processing,  pp.2292-2297" 
[pdf](http://nlp.ist.i.kyoto-u.ac.jp/local/pubdb/morita/morita_EMNLP2015_cameraready.pdf)
[poster](http://nlp.ist.i.kyoto-u.ac.jp/local/pubdb/morita/morita_EMNLP2015_poster.pptx)

"RNN 言語モデルを用いた日本語形態素解析の実用化. 森田一, 黒橋 禎夫. 情報処理学会 第78回全国大会"
[pdf](http://nlp.ist.i.kyoto-u.ac.jp/member/morita/paper/IPSJ2016_morita.pdf)

JUMAN マニュアル
[link](http://nlp.ist.i.kyoto-u.ac.jp/index.php?cmd=read&page=JUMAN)

## A. 推奨ライブラリのインストール方法
### libunwind
```
% wget http://download.savannah.gnu.org/releases/libunwind/libunwind-0.99-beta.tar.gz
% tar xzf libunwind-0.99-beta.tar.gz
% cd libunwind-0.99-beta
% ./configure --prefix=/path/to/somewhere/
% make 
% make install
```

### gperftools
```
% ./configure --prefx=/somewhere/local/
% make
\# 64bit 環境で，ld が32bit 用のライブラリをリンクしようとする場合には，
\# make のオプションに　UNWIND_LIBS=-lunwind-x86_64 と指定して下さい．
% make install
```

 
