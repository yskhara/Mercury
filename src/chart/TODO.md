# 方針

## Axis\<O>
- Axis\<O> の m_ticks_layouts はどうにかすべき。
  - layouts を保管しておくのが正しいかどうか。
保管しておくこと自体は正しいが、その内容の更新は draw() の中でのみ行われるべきではないか？今描画されている layout を保管する、という意味ならそう。
  - そもそもいつ使う？
    - update_axis_length() で最後の ticklabel の幅を測るのに使っている。
    allocation に対して、実際の軸の長さをどれだけ取れるか、にはたしかに必要。
    でも、これも推定でしかないはず。実際に draw() を呼ぶタイミングで pango layout (または pango context) は変更されうるという意味では、実際に描画される際の長さは正確にはわからない。
    draw() を呼ぶ前は m_allocated_length だけが正しくて、 draw() を呼んで初めて m_axis_length が正しい値をもつようになる。
    → draw() の中でのみ m_axis_length は更新されるべき。なので update_axis_length で m_tick_layouts は不要。 draw() で毎回 layout 作り直すので、それを使えば良い。
    - get_ticks_worst_dimensions() で最大の ticklabel 幅と最小中心間隔を測るのに使っている。
    多分これも前項と同じことだと思う。
    けど別の視点から見ると、get_ticks_worst_dimensions() でやりたいことは、今画面に表示されている ticklabel の寸法を測ることではないはず。暫定の ticks を仮に描画したらどんな寸法になるか、が知りたいだけ。なので、これも layouts としてキャッシュを利用する意味無し。
    例えば void Axis\<O>::try_draw_ticklabels_and_measure(std::vector\<AxisTick> &ticks, double &dim_height_max, double &dim_width_max) とかでもいいと思う。
    結局 ticklabel 同士の中心間距離は無くても良さそうだし。
    あるいは、 Chart （親）側が axis を 2 つ持っておくのも手。実際に描画される axis と、寸法測定用の axis。shadow axis みたいな。
    draw() に引数なし版のオーバーロードを用意して（つまり cairo_context を渡さない）、それが呼ばれたら実際には描画せず、 PangoLayout だけ作ってサイズ測定するだけ、みたいな感じ。


