//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/
//! @file   GameLib.h
//!
//! @brief  ゲーム制作用ライブラリのヘッダファイル
//!
//! @date   2015/6/10
//!
//! @author 高木 晋
//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/

// 多重インクルードの防止 ==================================================
#pragma once




// ヘッダファイルの読み込み ================================================
#include <DxLib.h>

#include "DxLibHelper\FrameTimer.h"
#include "DxLibHelper\Mathematics.h"
#include "SpriteStudio\SpriteStudioPlayer.h"

#if defined(_DEBUG)
	#include "Utilities\MemoryLeakDetector.h"
#endif




// 型の定義 ================================================================
typedef int HGRP;     //!< グラフィックハンドル
typedef int HSND;     //!< サウンドハンドル
typedef int COLOR;    //!< カラーコード
