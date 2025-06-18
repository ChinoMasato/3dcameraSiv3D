# ピッチ移動不具合の解説

このプロジェクトでは、カメラを上下（ピッチ）方向へ回転した状態で前進・後退しても上下方向へ移動できない問題がありました。

## 原因

以前の実装では `W` キーや `S` キーによる移動をワールド座標系の Z 軸方向に固定していました。そのためカメラを上向きにしても、移動は常に水平面上で行われ、プレイヤーの視線方向とは一致しませんでした。

```cpp
// 旧実装
if (KeyW.pressed()) { cameraPosition.z += cameraMoveSpeed * deltaTime; }
```

## 修正内容

現在の実装ではヨー（左右回転）とピッチ（上下回転）の両方を考慮して、カメラの前方ベクトルを計算しています。そのベクトルに沿って移動することで、視線方向へ進めるようになりました。

```cpp
// 新しい実装
const float cy = cos(yawAngle);
const float sy = sin(yawAngle);
const float cp = cos(pitchAngle);
const float sp = sin(pitchAngle);

MyPoint3D forward{ sy * cp, sp, cy * cp };

if (KeyW.pressed())
{
    cameraPosition.x += forward.x * cameraMoveSpeed * deltaTime;
    cameraPosition.y += forward.y * cameraMoveSpeed * deltaTime;
    cameraPosition.z += forward.z * cameraMoveSpeed * deltaTime;
}
```

## 学生向けアドバイス

3D カメラを扱う際は、移動方向をワールド座標ではなくカメラの向きから求めることが重要です。三角関数を使って前方・右方向ベクトルを計算し、そこに速度を掛けて位置を更新する方法を覚えておきましょう。
