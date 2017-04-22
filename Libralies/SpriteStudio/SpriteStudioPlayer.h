//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/
//! @file   SpriteStudioPlayer.h
//!
//! @brief  SpriteStudioの2Dアニメーションデータ再生関連のヘッダファイル
//!
//! @date   2016/6/15
//!
//! @author 高木 晋
//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/

// 多重インクルードの防止 ==================================================
#pragma once




// ヘッダファイルの読み込み ================================================
#include <ssbpLib\SS5Player.h>




// 型の定義 ================================================================
typedef int HAPACK;       //!< アニメーションパックハンドル
typedef int HASPRITE;     //!< アニメーションスプライトハンドル




// 関数の宣言 ==============================================================

// SpriteStudioプレーヤー関連 --------------------------------------------

// SpriteStudioプレーヤーの更新
int UpdateSpriteStudioPlayers();

// SpriteStudioプレーヤーの終了処理
int FinalizeSpriteStudioPlayers();



// アニメーションデータ関連 --------------------------------------------

// アニメーションパックファイルの読み込み
HAPACK LoadAPack(const char* FileName);

// アニメーションパックをメモリ上から削除する
int DeleteAPack(HAPACK APackHandle);

// アニメーションスプライトを作成
HASPRITE CreateASprite(HAPACK APackHandle);

// アニメーションスプライトをメモリ上から削除する
int DeleteASprite(HASPRITE ASpriteHandle);



// アニメーションの制御関連 ---------------------------------------------

// アニメーションスプライトのアニメーションを再生する
int PlayASprite(const char* AnimationName, int Loop, int StartFrameNo, HASPRITE ASpriteHandle);

// アニメーションスプライトのアニメーションを中断する
int PauseASprite(HASPRITE ASpriteHandle);

//アニメーションスプライトのアニメーションを再開する
int ResumeASprite(HASPRITE ASpriteHandle);

// アニメーションスプライトのアニメーションを停止する
int StopASprite(HASPRITE ASpriteHandle);

// アニメーションスプライトのアニメーションが再生中か調べる
int CheckASprite(HASPRITE ASpriteHandle);



// アニメーションスプライトの描画関連 -----------------------------------

// アニメーションスプライトの描画
int DrawASprite(int x, int y, HASPRITE ASpriteHandle, int TransFlag);

// アニメーションスプライトのLR反転描画
int DrawTurnASprite(int x, int y, HASPRITE ASpriteHandle, int TransFlag);

// アニメーションスプライトの拡大縮小描画
int DrawExtendASprite(int x, int y, double ExtRate, HASPRITE ASpriteHandle, int TransFlag);

// アニメーションスプライトの回転描画
int DrawRotaASprite(int x, int y, double ExtRate, double Angle, HASPRITE ASpriteHandle, int TransFlag, int TurnFlag);
