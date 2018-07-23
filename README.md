## ICFPC 2018 Submission of Team manarimo

## マナリモ
ナリモ科の藻。東北地方以北に分布し、水質のきれいな湖にしか生息しないという性質から「自然の水質管理官」と呼ばれる。

(English translation follows)
## Manarimo 
Manarimo is an Algae in the family Narimo. It distributes Tohoku and northward in Japan.
It is also known as "Natural water quality administrator" for its characteristic that inhabits only in clean lakes.


## Members
(Same as Team Adlersprung on ICFPC 2017)
* Osamu Koga (@osa_k)
* Yuki Kawata (@kawatea03)
* Yosuke Yano (@y3eadgbe)
* mkut (@mirai_towa)
* Yu Fujikake (@yuusti)
* Shunsuke Ohashi (@pepsin_amylase)


## Solvers
* kawatea/simple_division: x 軸か z 軸でブロックの数の最大値を抑えるように空間を分割して、それぞれを独立に下から作っていく。Grounded はチェックして切り替えるが、それを維持するために他と協調したりはしない。
* kawatea/mutation: simple_division に近い動きをするが、ブロックの状況が大きく変わるような y 座標で全体が揃うのを待って空間の分割の仕方を切り替える。
* kawatea/yufmodoki: mutation のパラメータを変えて各 y 座標で切り替えるようにしたもの。
* kawatea/shiva: 全てのブロックを消し去る。GVoid が使える幅でロボット全体で並列に上から穴を開け、順番に GVoid を使ってブロックを消していく。
* kawatea/cube: 中身が詰まった立方体を構成する。GFill を使って周囲を立方体で埋め、残りを面で埋めていく。
* kawatea/hand: 簡単かつ特徴的な面を手で解くためのサポートツール。
* yuusti/大体正の点数を取る (Get Positive Score in Most Cases): 20台で下から貪欲に作成、groundedならharmonicsをlowに切り替える。y軸ごとにFillの必要回数を計算してロボット割り当てるx軸を変える。
* yuusti/ジーフィル速いんだけど微妙にバグる: 正の点数を取るにgfillを加えた版。gfillはz軸方向への線分にのみ使用、x軸と長さ30で分割して均等に配分するというのをy軸ごとにやる。
* yuusti/ハローワーク: 配置が終わって暇になった人が次に置くべきとこをクエリするやつ。労災対策はしていない。
* yuusti/ハロワ雑魚狩りver: harmonicを使いたくないので下から置いて完成するやつだけを対象にしたやつ。10000stepでできなければ潔く死ぬ。狩るか狩られるか。
* mkut/splattershotjr: (lightning) 全体では Grounded 状態を維持をして、分割した空間をそれぞれロボットに担当させてほぼ貪欲に構成した。
* mkut/bamboozler14: (lightning/full) 移動経路を管理して衝突しないように制限なくロボットを飛ばして待ち時間を減らそうとしたが、労災が絶えなかった。
* mkut/splattershot: (full) bamboozler14の経験を生かしてほどほど制約のもとでgfillに対応した。gfillの性質の考察が足りなかったので実際には1次元に絞ったcustom-splattershotを使った。
* kawatea: solved problems by hand (ai/kawatea/hands)
* y3: solved problems by hand (assembly)
* amylase/bondrewd: 後処理器その1。とにかく命令を前に寄せて圧縮。数%から最大10%程度の効果があった。
* amylase/prushka: 後処理器その2。命令の依存性を解析して矛盾の内容に再構成する。競合の発生を対処できず死亡。

## Tools
* autoscorer：評価器。自動的に採点するからオートスコアラー。Voidの実装がつらかった。
* hissen: trace assembler
* model-analyzer
* official-tools：公式から落としてきて魔改造されたブラウザ評価器。
* saverwing：公式のモデルビューワからdata URIで飛んでくる画像を保存してサムネイルにするやつ。保存するからSaver Wing。
* nanachi: なんでもwebapp。よく死んでたけど再起動すると直る。アビスがもたらした不死の力かな？？
* mitty: 逆アセンブラ


## Note
Yosuke tried to develop a solver which generates a command sequence from backwards. It was actually equivalent to solve *Deassebly Problem* on full contest!
