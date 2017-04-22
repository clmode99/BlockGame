//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/
//! @file   SpriteStudioPlayer.cpp
//!
//! @brief  SpriteStudioの2Dアニメーションデータ再生関連のソースファイル
//!
//! @date   2016/6/15
//!
//! @author 高木 晋
//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/

// ヘッダファイルの読み込み ================================================
#include "SpriteStudioPlayer.h"

#include <memory>
#include <string>
#include <unordered_map>

#include <DxLibHelper\FrameTimer.h>




// ライブラリのリンク ======================================================
#if defined(_DEBUG)
	#pragma comment( lib, "ssbpLib\\ssbpLib_d.lib")
#else
	#pragma comment( lib, "ssbpLib\\ssbpLib.lib")
#endif




// クラス定義 ==============================================================
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//! @brief アニメーションスプライトクラス
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
class AnimationSprite
{
	// メンバ変数の宣言 --------------------------------------------
	private:
		std::unique_ptr<ss::Player> m_ssplayer;    //!< アニメーションを再生するプレーヤー
		float                       m_deltaTime;   //!< アニメーションを進めるフレーム時間[s]



	// メンバ関数の宣言および定義 ----------------------------------
	public:
		//------------------------------------------------------
		//! @brief コンストラクタ
		//!
		//! @param[in] animationPackName : アニメーションパック名
		//------------------------------------------------------
		AnimationSprite(const std::string& animationPackName)
			: m_ssplayer(ss::Player::create())
		{
			m_ssplayer->setData(animationPackName);
		}


		//------------------------------------------------------
		//! @brief デストラクタ
		//------------------------------------------------------
		~AnimationSprite() = default;



		//------------------------------------------------------
		//! @brief アニメーションの再生
		//!
		//! @param[in] animationName : 再生するアニメーション名
		//! @param[in] loop          : 再生ループ回数
		//! @param[in] startFrameNo  : 再生を開始するフレームNo
		//!
		//! @return なし
		//------------------------------------------------------
		void PlayAnimation(const std::string& animationName, int loop = 0, int startFrameNo = 0)
		{
			m_ssplayer->play(animationName, loop, startFrameNo);
		}


		//------------------------------------------------------
		//! @brief アニメーションの停止
		//!
		//! @param[in] なし
		//!
		//! @return なし
		//------------------------------------------------------
		void StopAnimation()
		{
			m_ssplayer->stop();
		}


		//------------------------------------------------------
		//! @brief アニメーションの中断
		//!
		//! @param[in] なし
		//!
		//! @return なし
		//------------------------------------------------------
		void PauseAnimation()
		{
			m_ssplayer->animePause();
		}


		//------------------------------------------------------
		//! @brief アニメーションの再開
		//!
		//! @param[in] なし
		//!
		//! @return なし
		//------------------------------------------------------
		void ResumeAnimation()
		{
			m_ssplayer->animeResume();
		}


		//------------------------------------------------------
		//! @brief 更新
		//!
		//! @param[in] dt : 進めるフレーム時間[s]
		//!
		//! @return なし
		//------------------------------------------------------
		void Update(float dt)
		{
			m_deltaTime = dt;
		}


		//------------------------------------------------------
		//! @brief 描画
		//!
		//! @param[in] x      : 描画する領域の座標
		//! @param[in] y      : 描画する領域の座標
		//! @param[in] scale  : 拡大率(1.0で等倍)
		//! @param[in] angle  : 描画角度(ラジアン指定)
		//!
		//! @return なし
		//------------------------------------------------------
		void Draw(int x, int y, double scaleX, double scaleY, double angle)
		{
			int screen_height;
			GetGraphSize(GetDrawScreen(), NULL, &screen_height);

			m_ssplayer->setPosition(static_cast<float>(x), static_cast<float>(screen_height - y));
			m_ssplayer->setScale(static_cast<float>(scaleX), static_cast<float>(scaleY));
			m_ssplayer->setRotation(0.0f, 0.0f, SSRadianToDegree(angle));

			m_ssplayer->update(m_deltaTime);
			m_deltaTime = 0.0f;

			m_ssplayer->draw();
		}


		//------------------------------------------------------
		//! @brief アニメーションの状態の取得
		//!
		//! @param[in] なし
		//!
		//! @retval 0: 停止中
		//! @retval 1: 再生中
		//! @retval 2: 中断中
		//------------------------------------------------------
		int GetAnimationState()
		{
			return m_ssplayer->getState();
		}
};




// 型の別名宣言 ============================================================
using ASpritePtr = std::unique_ptr<AnimationSprite>;




// ファイルスコープ変数の定義 ==============================================
static std::unordered_map<int, std::string> f_animationPackNameList;    //!< アニメーションパック名のリスト
static std::unordered_map<int, ASpritePtr>  f_animationSpriteList;      //!< アニメーションスプライトのリスト




// 関数の定義 ==============================================================

//----------------------------------------------------------------------
//! @brief SpriteStudioプレーヤーの更新
//!
//! @param[in] なし
//!
//! @retval  0：成功
//! @retval -1：エラー発生
//----------------------------------------------------------------------
int UpdateSpriteStudioPlayers()
{
	float dt = GetFrameDeltaTime();

	for (auto& e : f_animationSpriteList)
	{
		e.second->Update(dt);
	}

	return 0;
}



//----------------------------------------------------------------------
//! @brief SpriteStudioプレーヤー終了処理
//!
//! @param[in] なし
//!
//! @retval  0：成功
//! @retval -1：エラー発生
//----------------------------------------------------------------------
int FinalizeSpriteStudioPlayers()
{
	f_animationSpriteList.clear();


	auto resourceManager = ss::ResourceManager::getInstance();
	for (auto& e : f_animationPackNameList)
	{
		resourceManager->removeData(e.second);
	}


	f_animationPackNameList.clear();


	return 0;
}



//----------------------------------------------------------------------
//! @brief アニメーションパックファイルのメモリへの読みこみ
//!
//! @param[in] FileName : ロードするアニメーションパックファイルのファイルパス文字列へのポインタ
//!
//! @retval -1    ：エラー発生
//! @retval -1以外：アニメーションパックのハンドル
//----------------------------------------------------------------------
HAPACK LoadAPack(const char* FileName)
{
	auto resourceManager   = ss::ResourceManager::getInstance();
	auto animationPackName = resourceManager->addData(FileName);

	if (!animationPackName.empty())
	{
		int handle = std::hash<std::string>()(animationPackName)& 0x7FFFFFFF;
		f_animationPackNameList.insert(std::make_pair(handle, animationPackName));

		return handle;
	}
	else
	{
		return -1;
	}
}



//----------------------------------------------------------------------
//! @brief 指定のアニメーションパックをメモリ上から削除する
//!
//! @param[in] APackHandle : メモリ上から削除したいアニメーションパックのハンドル
//!
//! @retval  0：成功
//! @retval -1：エラー発生
//----------------------------------------------------------------------
int DeleteAPack(HAPACK APackHandle)
{
	auto it = f_animationPackNameList.find(APackHandle);

	if (it != f_animationPackNameList.end())
	{
		auto resourceManager = ss::ResourceManager::getInstance();
		resourceManager->removeData(it->second);

		f_animationPackNameList.erase(it);

		return 0;
	}
	else
	{
		return -1;
	}
}



//----------------------------------------------------------------------
//! @brief アニメーションスプライトを作成する
//!
//! @param[in] APackHandle : 作成するアニメーションパックの識別番号(アニメーションパックハンドル)
//!
//! @retval -1    ：エラー発生
//! @retval -1以外：アニメーションスプライトのハンドル
//----------------------------------------------------------------------
HASPRITE CreateASprite(HAPACK APackHandle)
{
	auto it = f_animationPackNameList.find(APackHandle);

	if (it != f_animationPackNameList.end())
	{
		ASpritePtr animationSprite(new AnimationSprite(it->second));

		int handle = std::hash<ASpritePtr>()(animationSprite) & 0x7FFFFFFF;
		f_animationSpriteList.insert(std::make_pair(handle, std::move(animationSprite)));

		return handle;
	}
	else
	{
		return -1;
	}
}



//----------------------------------------------------------------------
//! @brief アニメーションスプライトをメモリ上から削除する
//!
//! @param[in] APackHandle : メモリ上から削除したいアニメーションスプライトの識別番号(アニメーションスプライトハンドル)
//!
//! @retval  0：成功
//! @retval -1：エラー発生
//----------------------------------------------------------------------
int DeleteASprite(HASPRITE ASpriteHandle)
{
	auto a = f_animationSpriteList.erase(ASpriteHandle);

	return 0;
}



//----------------------------------------------------------------------
//! @brief アニメーションスプライトのアニメーションを再生する
//!
//! @param[in] AnimationName : 再生するアニメーション名へのポインタ
//! @param[in] Loop          : 再生ループ回数
//! @param[in] StartFrameNo  : 再生を開始するフレームNo
//! @param[in] ASpriteHandle : 再生するアニメーションスプライトの識別番号(アニメーションスプライトハンドル)
//!
//! @retval  0：成功
//! @retval -1：エラー発生
//----------------------------------------------------------------------
int PlayASprite(const char* AnimationName, int Loop, int StartFrameNo, HASPRITE ASpriteHandle)
{
	auto it = f_animationSpriteList.find(ASpriteHandle);

	if (it != f_animationSpriteList.end())
	{
		it->second->PlayAnimation(AnimationName, Loop, StartFrameNo);
		return 0;
	}
	else
	{
		return -1;
	}
}



//----------------------------------------------------------------------
//! @brief アニメーションスプライトのアニメーションを中断する
//!
//! @param[in] ASpriteHandle : 中断するアニメーションスプライトの識別番号(アニメーションスプライトハンドル)
//!
//! @retval  0：成功
//! @retval -1：エラー発生
//----------------------------------------------------------------------
int PauseASprite(HASPRITE ASpriteHandle)
{
	auto it = f_animationSpriteList.find(ASpriteHandle);

	if (it != f_animationSpriteList.end())
	{
		it->second->PauseAnimation();
		return 0;
	}
	else
	{
		return -1;
	}
}



//----------------------------------------------------------------------
//! @brief アニメーションスプライトのアニメーションを再開する
//!
//! @param[in] ASpriteHandle : 再開するアニメーションスプライトの識別番号(アニメーションスプライトハンドル)
//!
//! @retval  0：成功
//! @retval -1：エラー発生
//----------------------------------------------------------------------
int ResumeASprite(HASPRITE ASpriteHandle)
{
	auto it = f_animationSpriteList.find(ASpriteHandle);

	if (it != f_animationSpriteList.end())
	{
		it->second->ResumeAnimation();
		return 0;
	}
	else
	{
		return -1;
	}
}



//----------------------------------------------------------------------
//! @brief アニメーションスプライトのアニメーションを停止する
//!
//! @param[in] ASpriteHandle : 停止するアニメーションスプライトの識別番号(アニメーションスプライトハンドル)
//!
//! @retval  0：成功
//! @retval -1：エラー発生
//----------------------------------------------------------------------
int StopASprite(HASPRITE ASpriteHandle)
{
	auto it = f_animationSpriteList.find(ASpriteHandle);

	if (it != f_animationSpriteList.end())
	{
		it->second->StopAnimation();
		return 0;
	}
	else
	{
		return -1;
	}
}



//----------------------------------------------------------------------
//! @brief アニメーションスプライトのアニメーションが再生中か調べる
//!
//! @param[in] ASpriteHandle : 再生状態を調べるアニメーションスプライトの識別番号(アニメーションスプライトハンドル)
//!
//! @retval  0：停止中
//! @retval  1：再生中
//! @retval  2：中断中
//! @retval -1：エラー発生
//----------------------------------------------------------------------
int CheckASprite(HASPRITE ASpriteHandle)
{
	auto it = f_animationSpriteList.find(ASpriteHandle);

	if (it != f_animationSpriteList.end())
	{
		return it->second->GetAnimationState();
	}
	else
	{
		return -1;
	}
}



//----------------------------------------------------------------------
//! @brief アニメーションスプライトの描画
//!
//! @param[in] x             : アニメーションスプライトを描画する領域の座標
//! @param[in] y             : アニメーションスプライトを描画する領域の座標
//! @param[in] ASpriteHandle : 描画するアニメーションスプライトの識別番号(アニメーションスプライトハンドル)
//! @param[in] TransFlag     : 画像の透明度を有効にするかどうか(未使用)
//!
//! @retval  0：成功
//! @retval -1：エラー発生
//----------------------------------------------------------------------
int DrawASprite(int x, int y, HASPRITE ASpriteHandle, int TransFlag)
{
	auto it = f_animationSpriteList.find(ASpriteHandle);

	if (it != f_animationSpriteList.end())
	{
		it->second->Draw(x, y, 1.0, 1.0, 0.0);
		return 0;
	}
	else
	{
		return -1;
	}
}



//----------------------------------------------------------------------
//! @brief アニメーションスプライトのLR反転描画
//!
//! @param[in] x             : アニメーションスプライトを描画する領域の座標
//! @param[in] y             : アニメーションスプライトを描画する領域の座標
//! @param[in] ASpriteHandle : 描画するアニメーションスプライトの識別番号(アニメーションスプライトハンドル)
//! @param[in] TransFlag     : 画像の透明度を有効にするかどうか(未使用)
//!
//! @retval  0：成功
//! @retval -1：エラー発生
//----------------------------------------------------------------------
int DrawTurnASprite(int x, int y, HASPRITE ASpriteHandle, int TransFlag)
{
	auto it = f_animationSpriteList.find(ASpriteHandle);

	if (it != f_animationSpriteList.end())
	{
		it->second->Draw(x, y, -1.0, 1.0, 0.0);
		return 0;
	}
	else
	{
		return -1;
	}
}



//----------------------------------------------------------------------
//! @brief アニメーションスプライトの拡大縮小描画
//!
//! @param[in] x             : アニメーションスプライトを描画する領域の座標
//! @param[in] y             : アニメーションスプライトを描画する領域の座標
//! @param[in] ExtRate       : 拡大率(1.0で等倍)
//! @param[in] ASpriteHandle : 描画するアニメーションスプライトの識別番号(アニメーションスプライトハンドル)
//! @param[in] TransFlag     : 画像の透明度を有効にするかどうか(未使用)
//!
//! @retval  0：成功
//! @retval -1：エラー発生
//----------------------------------------------------------------------
int DrawExtendASprite(int x, int y, double ExtRate, HASPRITE ASpriteHandle, int TransFlag)
{
	auto it = f_animationSpriteList.find(ASpriteHandle);

	if (it != f_animationSpriteList.end())
	{
		it->second->Draw(x, y, ExtRate, ExtRate, 0.0);
		return 0;
	}
	else
	{
		return -1;
	}
}



//----------------------------------------------------------------------
//! @brief アニメーションスプライトの回転描画
//!
//! @param[in] x             : アニメーションスプライトを描画する領域の座標
//! @param[in] y             : アニメーションスプライトを描画する領域の座標
//! @param[in] ExtRate       : 拡大率(1.0で等倍)
//! @param[in] Angle         : 描画角度(ラジアン指定)
//! @param[in] ASpriteHandle : 描画するアニメーションスプライトの識別番号(アニメーションスプライトハンドル)
//! @param[in] TransFlag     : 画像の透明度を有効にするかどうか(未使用)
//! @param[in] TurnFlag      : 画像の左右反転を行うか、のフラグ(FALSEで普通に描画 TRUEで反転)
//!
//! @retval  0：成功
//! @retval -1：エラー発生
//----------------------------------------------------------------------
int DrawRotaASprite(int x, int y, double ExtRate, double Angle, HASPRITE ASpriteHandle, int TransFlag, int TurnFlag)
{
	auto it = f_animationSpriteList.find(ASpriteHandle);

	if (it != f_animationSpriteList.end())
	{
		double ExtRateX = TurnFlag ? -ExtRate : ExtRate;
		it->second->Draw(x, y, ExtRateX, ExtRate, Angle);
		return 0;
	}
	else
	{
		return -1;
	}
}
