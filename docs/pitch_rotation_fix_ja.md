# ピッチ回転不具合の解説

このプロジェクトでは、カメラを上向きや下向きに回転しても、その角度が描画結果に反映されない問題がありました。

## 原因

旧実装では頂点にヨー回転のみを適用し、ピッチ角による回転は計算していたものの描画に利用していませんでした。

```cpp
// 旧実装の一部
Vec2 screenP1 = projectPerspective(vertex1Rotated); // ヨー回転のみ反映
```

## 修正内容

ヨー角とピッチ角の両方を用いて頂点を回転させる `applyCameraRotation` 関数を作成し、描画前にすべての頂点に適用するよう修正しました。

```cpp
const float sp = sin(pitchAngle);
const float cp = cos(pitchAngle);
const float sy = sin(yawAngle);
const float cy = cos(yawAngle);

auto applyCameraRotation = [&](const MyPoint3D& v) -> MyPoint3D
{
    MyPoint3D tmp{ v.x, v.y * cp + v.z * sp, -v.y * sp + v.z * cp };
    return MyPoint3D{ tmp.x * cy + tmp.z * sy, tmp.y, -tmp.x * sy + tmp.z * cy };
};
```

この関数を通じてピッチ回転が正しく描画に反映されるようになりました。また、前方ベクトルを計算して視線方向へ移動できるようにするなど、カメラ操作全体を改善しています。

## 学生向けアドバイス

カメラの向きを使って移動量や描画を計算する際は、ヨー・ピッチの両方を考慮した回転行列を適用しましょう。複数の角度を一度に処理する関数を用意すると実装がシンプルになります。
