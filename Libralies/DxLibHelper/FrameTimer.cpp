//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/
//! @file   FrameTimer.cpp
//!
//! @brief  フレームタイマー関連のソースファイル
//!
//! @date   2015/5/1
//!
//! @author 高木 晋
//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/

// ヘッダファイルの読み込み ================================================
#include <DxLib.h>




// ファイルスコープ変数の定義 ==============================================
static float f_frameDeltaTime = 0.0f;    //!< フレームの経過時間[s]
static float f_frameRate      = 0.0f;    //!< フレームレート[fps]




// 関数の定義 ==============================================================

//----------------------------------------------------------------------
//! @brief フレームタイマーの更新
//!
//! @param[in] なし
//!
//! @return なし
//----------------------------------------------------------------------
void UpdateFrameTimer(void)
{
	static LONGLONG previousTime = 0;    // 前回の計測時間
	static LONGLONG currentTime  = 0;    // 現在の計測時間
	static LONGLONG startTime    = 0;    // フレームレート計測開始時間
	static int      frameCount   = 0;    // フレーム数


	// 時間の計測
	previousTime = (currentTime != 0) ? currentTime : GetNowHiPerformanceCount();
	currentTime  = GetNowHiPerformanceCount();


	// フレームの経過時間の更新
	f_frameDeltaTime = (float)(currentTime - previousTime) / 1000000;


	// 1秒毎にフレームレートの更新
	frameCount++;
	if ((currentTime - startTime) > 1000000)
	{
		f_frameRate = (float)(1000000 * frameCount) / (currentTime - startTime);
		startTime   = currentTime;
		frameCount  = 0;
	}
}



//------------------------------------------------------------------
//! @brief フレームの経過時間の取得
//!
//! @param[in] なし
//!
//! @return フレームの経過時間[s]
//------------------------------------------------------------------
float GetFrameDeltaTime(void)
{
	return f_frameDeltaTime;
}



//------------------------------------------------------------------
//! @brief フレームレートの取得
//!
//! @param[in] なし
//!
//! @return フレームレート[fps]
//------------------------------------------------------------------
float GetFrameRate(void)
{
	return f_frameRate;
}
