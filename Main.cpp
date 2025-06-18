#include <Siv3D.hpp> // Siv3D v0.6.15

struct MyVec2 {
	float x;
	float y;
	MyVec2(float _x = 0.0f, float _y = 0.0f) : x(_x), y(_y) {}
};

struct MyPoint3D {
	float x;
	float y;
	float z;
	MyPoint3D(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}
};

struct Edge {
	int startIndex;
	int endIndex;
};

auto projectParallel = [](const MyPoint3D& p3d) -> Vec2 {
	float screenX = p3d.x + Scene::Center().x;
	float screenY = p3d.y + Scene::Center().y;
	return Vec2{ screenX, screenY };
	};

// Main関数の外、またはラムダ式として定義
const float SCREEN_DISTANCE = 250.0f; // 視点からスクリーンまでの距離 (この値を調整して効果を見る)

auto projectPerspective = [](const MyPoint3D& p3d) -> Vec2 {
	// worldZ が0に近い、または負の場合の対処
	// (視点と同じ位置、または視点より手前にある点は正しく投影できない)
	if (p3d.z <= 0.1f) { // 非常に小さい正の値を閾値として、それ以下は描画しない
		return Vec2{ -1.0e7, -1.0e7 }; // 画面外の非常に大きな/小さな値を返して描画を避ける
	}

	float screenX_projected = (p3d.x * SCREEN_DISTANCE) / p3d.z;
	float screenY_projected = (p3d.y * SCREEN_DISTANCE) / p3d.z; // Y軸の向きに注意

	// スクリーン中央を原点として投影された座標を、画面中央にオフセット
	float screenX_final = screenX_projected + Scene::Center().x;
	float screenY_final = screenY_projected + Scene::Center().y;
	// もし3D空間でY軸上向きを正とした場合、-screenY_projected + Scene::Center().y とする

	return Vec2{ screenX_final, screenY_final };
	};

void Main()
{
	Window::SetTitle(U"中級課題 Lv.3 - ステップ3");
	Scene::SetBackground(ColorF{ 0.1, 0.2, 0.3 });

	const float nearZ = 100.0f; // 手前の面
	const float farZ = 300.0f;  // 奥の面

	// 立方体の3D座標リスト
	Array<MyPoint3D> cubeVertices;
	const float halfSideLength = 75.0f;
	// 奥の面
	cubeVertices << MyPoint3D(-halfSideLength, -halfSideLength, farZ);  // 0
	cubeVertices << MyPoint3D(halfSideLength, -halfSideLength, farZ);   // 1
	cubeVertices << MyPoint3D(halfSideLength, halfSideLength, farZ);    // 2
	cubeVertices << MyPoint3D(-halfSideLength, halfSideLength, farZ);   // 3
	// 手前の面
	cubeVertices << MyPoint3D(-halfSideLength, -halfSideLength, nearZ); // 4
	cubeVertices << MyPoint3D(halfSideLength, -halfSideLength, nearZ);  // 5
	cubeVertices << MyPoint3D(halfSideLength, halfSideLength, nearZ);   // 6
	cubeVertices << MyPoint3D(-halfSideLength, halfSideLength, nearZ);  // 7


	// 辺のリスト
	Array<Edge> cubeEdges = {
		// 奥
		{0, 1}, {1, 2}, {2, 3}, {3, 0},
		// 手前
		{4, 5}, {5, 6}, {6, 7}, {7, 4},
		// 側面
		{0, 4}, {1, 5}, {2, 6}, {3, 7}
	};

	// Main関数内の、ループが始まる前の適切な場所に定義
	MyPoint3D cameraPosition{ 0.0f, 50.0f, -300.0f };
	float yawAngle = 0.0f; // カメラのY軸周りの回転角度 (ラジアン)
	// Main関数内の適切な場所 (yawAngle の近くなど)
	float pitchAngle = 0.0f; // カメラのX軸周りの回転角度 (ラジアン)
	// cameraRotationSpeed はヨー回転と共通で使える
	const float cameraMoveSpeed = 150.0f;
	const float cameraRotationSpeed = 1.5f; // カメラの回転速度 (ラジアン/秒)

	while (System::Update())
	{
		// メインループの先頭付近 (描画処理の前)
		const double deltaTime = Scene::DeltaTime(); // 1フレームの経過時間を取得

               // 以前はワールド座標系に沿って移動していたため、視線方向と移動方向が一致しなかった
               // 以下は旧実装
               /*
               if (KeyW.pressed()) { cameraPosition.z += cameraMoveSpeed * deltaTime; } // 前進 (ワールドZ+ 方向へ)
               if (KeyS.pressed()) { cameraPosition.z -= cameraMoveSpeed * deltaTime; } // 後退 (ワールドZ- 方向へ)
               if (KeyA.pressed()) { cameraPosition.x -= cameraMoveSpeed * deltaTime; } // 左移動 (ワールドX- 方向へ)
               if (KeyD.pressed()) { cameraPosition.x += cameraMoveSpeed * deltaTime; } // 右移動 (ワールドX+ 方向へ)
               if (KeySpace.pressed()) { cameraPosition.y += cameraMoveSpeed * deltaTime; } // 上昇 (ワールドY+ 方向へ)
               if (KeyLShift.pressed() || KeyLControl.pressed()) { cameraPosition.y -= cameraMoveSpeed * deltaTime; } // 下降 (ワールドY- 方向へ)
               // カメラの向きを反映させるため、ヨー角とピッチ角から前方ベクトルを計算して移動する

               {
                       const float cy = cos(yawAngle);
                       const float sy = sin(yawAngle);
                       const float cp = cos(pitchAngle);
                       const float sp = sin(pitchAngle);

                       MyPoint3D forward{
                               sy * cp,
                               sp,
                               cy * cp
                       };

                       MyPoint3D right{
                               cy,
                               0.0f,
                               -sy
                       };

                       if (KeyW.pressed())
                       {
                               cameraPosition.x += forward.x * cameraMoveSpeed * deltaTime;
                               cameraPosition.y += forward.y * cameraMoveSpeed * deltaTime;
                               cameraPosition.z += forward.z * cameraMoveSpeed * deltaTime;
                       }

                       if (KeyS.pressed())
                       {
                               cameraPosition.x -= forward.x * cameraMoveSpeed * deltaTime;
                               cameraPosition.y -= forward.y * cameraMoveSpeed * deltaTime;
                               cameraPosition.z -= forward.z * cameraMoveSpeed * deltaTime;
                       }

                       if (KeyA.pressed())
                       {
                               cameraPosition.x -= right.x * cameraMoveSpeed * deltaTime;
                               cameraPosition.z -= right.z * cameraMoveSpeed * deltaTime;
                       }

                       if (KeyD.pressed())
                       {
                               cameraPosition.x += right.x * cameraMoveSpeed * deltaTime;
                               cameraPosition.z += right.z * cameraMoveSpeed * deltaTime;
                       }

                       if (KeySpace.pressed())
                       {
                               cameraPosition.y += cameraMoveSpeed * deltaTime;
                       }

                       if (KeyLShift.pressed() || KeyLControl.pressed())
                       {
                               cameraPosition.y -= cameraMoveSpeed * deltaTime;
                       }
               }

		// メインループのカメラ位置更新処理の後など
		if (KeyLeft.pressed()) { yawAngle += cameraRotationSpeed * deltaTime; }
		if (KeyRight.pressed()) { yawAngle -= cameraRotationSpeed * deltaTime; }

		// メインループのヨー角更新処理の後など
		const float maxPitch = ToRadians(89.0f); // 最大ピッチ角 (約89度)
		const float minPitch = -ToRadians(89.0f); // 最小ピッチ角 (約-89度)

		if (KeyUp.pressed()) { pitchAngle += cameraRotationSpeed * deltaTime; }
		if (KeyDown.pressed()) { pitchAngle -= cameraRotationSpeed * deltaTime; }

		// ピッチ角を制限範囲内にクランプする
		pitchAngle = Math::Clamp(pitchAngle, minPitch, maxPitch);

		// 角度が2PIを超えたり負になったりしてもそのままで問題ないが、正規化しても良い
		// yawAngle = Math::Fmod(yawAngle, Math::TwoPi); // 0～2PIの範囲に正規化 (任意)

		/*for (const auto& edge : cubeEdges)
		{
			Vec2 p1 = projectParallel(cubeVertices[edge.startIndex]);
			Vec2 p2 = projectParallel(cubeVertices[edge.endIndex]);

			Line{ p1, p2 }.draw(3, Palette::Aqua);
		}*/

               // メインループ内での描画処理 (projectPerspectiveを使用)
               for (const auto& edge : cubeEdges)
               {
                       // 頂点のワールド座標
                       const MyPoint3D& worldVertex1 = cubeVertices[edge.startIndex];
                       const MyPoint3D& worldVertex2 = cubeVertices[edge.endIndex];

                       // 以前の実装では一つの頂点だけから相対座標を計算していた
                       // しかしこの方法ではピッチ回転を適用する際の処理が複雑になっていた
                       /*
                       const MyPoint3D& worldVertex = cubeVertices[edge.startIndex];
                       MyPoint3D vertexRelativeToCamera;
                       vertexRelativeToCamera.x = worldVertex.x - cameraPosition.x;
                       vertexRelativeToCamera.y = worldVertex.y - cameraPosition.y;
                       vertexRelativeToCamera.z = worldVertex.z - cameraPosition.z;
                       */

                       // カメラ基準の相対座標
                       MyPoint3D vertex1RelativeToCamera{
                               worldVertex1.x - cameraPosition.x,
                               worldVertex1.y - cameraPosition.y,
                               worldVertex1.z - cameraPosition.z
                       };

                       MyPoint3D vertex2RelativeToCamera{
                               worldVertex2.x - cameraPosition.x,
                               worldVertex2.y - cameraPosition.y,
                               worldVertex2.z - cameraPosition.z
                       };

                       // --- 旧実装: ピッチとヨーの回転を別々に適用しており複雑だった ---
                       /*
                       const float s = sin(yawAngle);
                       const float c = cos(yawAngle);

                       MyPoint3D vertex1Rotated{
                               vertex1RelativeToCamera.x * c + vertex1RelativeToCamera.z * s,
                               vertex1RelativeToCamera.y,
                               -vertex1RelativeToCamera.x * s + vertex1RelativeToCamera.z * c
                       };

                       MyPoint3D vertex2Rotated{
                               vertex2RelativeToCamera.x * c + vertex2RelativeToCamera.z * s,
                               vertex2RelativeToCamera.y,
                               -vertex2RelativeToCamera.x * s + vertex2RelativeToCamera.z * c
                       };

                       MyPoint3D vertexAfterPitch;
                       float sp = sin(pitchAngle);
                       float cp = cos(pitchAngle);
                       vertexAfterPitch.x = vertexRelativeToCamera.x;
                       vertexAfterPitch.y = vertexRelativeToCamera.y * cp + vertexRelativeToCamera.z * sp;
                       vertexAfterPitch.z = -vertexRelativeToCamera.y * sp + vertexRelativeToCamera.z * cp;

                       MyPoint3D vertexAfterYaw;
                       float sy = sin(yawAngle);
                       float cy = cos(yawAngle);
                       vertexAfterYaw.x = vertexAfterPitch.x * cy + vertexAfterPitch.z * sy;
                       vertexAfterYaw.y = vertexAfterPitch.y;
                       vertexAfterYaw.z = -vertexAfterPitch.x * sy + vertexAfterPitch.z * cy;

                       Vec2 screenPoint = projectPerspective(vertexAfterYaw);
                       ClearPrint();
                       Print << U"screenPoint " << screenPoint;

                       Vec2 screenP1 = projectPerspective(vertex1Rotated);
                       Vec2 screenP2 = projectPerspective(vertex2Rotated);
                       */

                       // 修正版: 1回の関数でピッチとヨーをまとめて適用する
                       const float sp = sin(pitchAngle);
                       const float cp = cos(pitchAngle);
                       const float sy = sin(yawAngle);
                       const float cy = cos(yawAngle);

                       auto applyCameraRotation = [&](const MyPoint3D& v) -> MyPoint3D
                       {
                               MyPoint3D tmp{
                                       v.x,
                                       v.y * cp + v.z * sp,
                                       -v.y * sp + v.z * cp
                               };

                               return MyPoint3D{
                                       tmp.x * cy + tmp.z * sy,
                                       tmp.y,
                                       -tmp.x * sy + tmp.z * cy
                               };
                       };

                       MyPoint3D vertex1Rotated = applyCameraRotation(vertex1RelativeToCamera);
                       MyPoint3D vertex2Rotated = applyCameraRotation(vertex2RelativeToCamera);

                       // 透視投影
                       Vec2 screenP1 = projectPerspective(vertex1Rotated);
                       Vec2 screenP2 = projectPerspective(vertex2Rotated);

			if (screenP1.x < -1.0e6 || screenP2.x < -1.0e6)
			{
				continue;
			}

			Line{ screenP1, screenP2 }.draw(2, Palette::Gold);
		}

	}
}
