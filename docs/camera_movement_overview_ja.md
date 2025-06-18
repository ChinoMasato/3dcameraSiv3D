# カメラ移動の実装と理解

このサンプルでは、ピッチ角とヨー角を使ってカメラを操作します。ゲームプログラマーを目指す学生向けに、主要なポイントを簡潔にまとめます。

## 基本方針

1. プレイヤー入力からカメラのヨー角(`yawAngle`)とピッチ角(`pitchAngle`)を更新する。
2. `yawAngle`と`pitchAngle`から前方ベクトルと右方向ベクトルを計算する。
3. 計算したベクトルに速度と`deltaTime`を掛けてカメラ位置を更新する。

## 前方ベクトルの計算例

```cpp
const float cy = cos(yawAngle);
const float sy = sin(yawAngle);
const float cp = cos(pitchAngle);
const float sp = sin(pitchAngle);

MyPoint3D forward{ sy * cp, sp, cy * cp };
MyPoint3D right{ cy, 0.0f, -sy };
```

ここで求めた`forward`を移動量に掛ければ、カメラの向いている方向に進むことができます。

## 実装時の注意

- 角度はラジアンで扱うため、度から変換するときは`ToRadians()`を使います。
- 毎フレームの時間差によって移動速度が変わらないよう、`deltaTime`を掛けてください。
- ピッチ角は90度付近で問題が起こりやすいので、上限を設けておくと安全です。

このように、カメラの向きをベースに移動ベクトルを求めることで、ピッチ方向を含めた自由な移動が可能になります。ゲーム制作に役立ててください。

