//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/
//! @file   GameMain.cpp
//!
//! @brief  ゲーム関連のソースファイル
//!
//! @date   2016.06.17
//!
//! @author Miyu Hara
//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/

/*一時的コメント
・当たり判定の精度をよくしたい(当たってないのに反射したりを失くす)
・アイテム取得できるようにする(特定のブロック壊すと使えるようにする)
・
・１をTRUEにする(可読性)
*/

// ヘッダファイルの読み込み ================================================
#include "GameMain.h"

//記号定数
#define BLOCK_WIDTH (8)		//ブロックの数の幅
#define BLOCK_HEIGHT (4)	//ブロックの数の高さ
#define MAX_BLOCK (BLOCK_WIDTH*BLOCK_HEIGHT)		//ブロックの数
#define BLOCK_START_X (5)	//ブロックのスタート位置
#define BLOCK_START_Y (40)	//ブロックのスタート位置
#define PADDLE_SPD_X (8.0f)

//構造体宣言
typedef struct tag_object
{
	HGRP handle;	//グラフィックハンドル
	int grp_x;		//画像X
	int grp_y;		//画像Y
	int grp_w;		//画像の幅
	int grp_h;		//画像の高さ
	float pos_x;	//座標X
	float pos_y;	//座標Y
	float spd_x;	//速度X
	float spd_y;	//速度Y
	int state;		//描画フラグ(0=表示しない、1=ふつう、2=かたい、3=アイテム)
}OBJECT;

//列挙体宣言
enum SCENE
{
	LOGO,	//ロゴ画面
	TITLE,	//タイトル画面
	PLAY,	//プレイ画面
	CLEAR,	//クリア画面
	OVER	//ゲームオーバー画面
};

// グローバル変数の定義 ====================================================
OBJECT g_block[MAX_BLOCK];	//ブロック
OBJECT g_ball;				//ボール
OBJECT g_paddle;			//パドル
HGRP g_PongImage;			//グラフィックハンドル(PongImage.png)
HGRP g_Logo_handle;			//グラフィックハンドル(ロゴ)
HGRP g_title_handle;		//グラフィックハンドル(タイトル)
int g_key_code = 0;			//キー情報
int g_key_old = 0;			//１フレーム前のキー情報
int g_key_enter = 0;		//Enterキーを押されてるか(GetJoypad～だとEnterキー扱えないため)
int g_key_enter_old = 0;	//g_key_oldのEnterキーばーじょん
int g_block_cnt = 0;		//ブロックカウンタ
int g_scene = PLAY;			//シーン管理
bool g_init = false;		//初期化フラグ
int g_nextScene = g_scene;	//次のシーン
int g_cnt = 0;				//カウンタ
bool start_flag = false;	//スタートの発射フラグ(false:スタート前、true:発射した)
int g_block_soft_handle = 0;	//通常状態のブロック
int g_block_hard_handle = 0;	//かためのブロック
int g_block_item_handle = 0;	//グラフィックハンドル(ブロックのアイテム)
int g_block_type = 0;	//ブロックタイプ(0:ソフト、1:ハード、2:アイテム)
int g_item = 0;
bool g_item_flag = false;

/*関数のプロトタイプ宣言*/
bool Collision(OBJECT o1, OBJECT o2);	//当たり判定
void GameLogo(void);					//ロゴ画面
void RenderLogo(void);					//ロゴ画面の描画
void GameTitle(void);					//タイトル画面
void RenderTitle(void);					//タイトル画面の描画
void GamePlay(void);					//プレイ画面
void RenderPlay(void);					//プレイ画面の描画
void GameClear(void);					//クリア画面
void RenderClear(void);					//クリア画面の描画
void GameOver(void);					//ゲームオーバー画面
void RenderOver(void);					//ゲームオーバー画面の描画


// 関数の定義 ==============================================================

//----------------------------------------------------------------------
//! @brief ゲームの初期化処理
//!
//! @param[in] なし
//!
//! @return なし
//----------------------------------------------------------------------
void InitializeGame(void)
{
	g_PongImage = LoadGraph("Resources\\images\\PongImage.png");	//PongImage.pngのグラフィックハンドル
	g_Logo_handle = LoadGraph("Resources\\images\\my_logo.png");	//ロゴのグラフィックハンドル
	g_block_soft_handle = LoadGraph("Resources\\images\\block_2_2.png");
	g_block_hard_handle = LoadGraph("Resources\\images\\block_1.png");
	g_block_item_handle = LoadGraph("Resources\\images\\block_item.png");
	g_title_handle = LoadGraph("Resources\\images\\title.png");

	g_key_code = 0;
	g_key_old = 0;
	g_block_cnt = 0;
	start_flag = false;
	g_item = 0;
	g_item_flag = false;

	//ブロック初期化
	for (int i = 0; i < BLOCK_HEIGHT; i++)			//高さ
	{
		for (int j = 0; j < BLOCK_WIDTH; j++)		//幅
		{
			//ふつー(16個)
			if ((i * BLOCK_WIDTH) + j < 16)
			{
				g_block[(i * BLOCK_WIDTH) + j].handle = g_block_soft_handle;	//グラフィックハンドル
				g_block[(i * BLOCK_WIDTH) + j].state = 1;
			}
			//かたいやーつ(12個)
			else if ((i * BLOCK_WIDTH) + j < 28)
			{
				g_block[(i * BLOCK_WIDTH) + j].handle = g_block_hard_handle;	//グラフィックハンドル
				g_block[(i * BLOCK_WIDTH) + j].state = 2;
			}
			//アイテムなやーつ(4個)
			else if ((i * BLOCK_WIDTH) + j < 32)
			{
				g_block[(i * BLOCK_WIDTH) + j].handle = g_block_item_handle;	//グラフィックハンドル
				g_block[(i * BLOCK_WIDTH) + j].state = 3;
			}

			g_block[(i * BLOCK_WIDTH) + j].grp_x = 0;						//画像X
			g_block[(i * BLOCK_WIDTH) + j].grp_y = 0;						//画像Y
			g_block[(i * BLOCK_WIDTH) + j].grp_w = 64;						//幅
			g_block[(i * BLOCK_WIDTH) + j].grp_h = 32;						//高さ
			g_block[(i * BLOCK_WIDTH) + j].pos_x = j*(g_block[i].grp_w) + BLOCK_START_X ;		//座標X
			g_block[(i * BLOCK_WIDTH) + j].pos_y = i*(g_block[i].grp_h) + BLOCK_START_Y;		//座標Y
			g_block[(i * BLOCK_WIDTH) + j].spd_x = 1.0f;						//速度X
			g_block[(i * BLOCK_WIDTH) + j].spd_y = 0.0f;						//速度Y
		}
	}

	static int temp_n1 = 0;
	static int temp_n2 = 0;
	static int temp_state = 0;
	static int temp_handle = 0;

	//ブロックシャッフル
	for (int i = 0; i < 1000; i++)
	{
		temp_n1 = GetRand(MAX_BLOCK - 1);
		temp_n2 = GetRand(MAX_BLOCK - 1);

		temp_state = g_block[temp_n1].state;
		g_block[temp_n1].state = g_block[temp_n2].state;
		g_block[temp_n2].state = temp_state;

		temp_handle = g_block[temp_n1].handle;
		g_block[temp_n1].handle = g_block[temp_n2].handle;
		g_block[temp_n2].handle = temp_handle;
	}
	


	//パドル初期化
	{
		g_paddle.handle = LoadGraph("Resources\\images\\paddle.png");	//グラフィックハンドル
		g_paddle.grp_x = 0;				//画像X
		g_paddle.grp_y = 0;				//画像Y
		g_paddle.grp_w = 64;			//幅
		g_paddle.grp_h = 16;			//高さ
		g_paddle.pos_x = 200.0f;		//座標X
		g_paddle.pos_y = 400.0f;		//座標Y
		g_paddle.spd_x = 0.0f;			//速度X
		g_paddle.spd_y = 0.0f;			//速度Y
	}

	//ボール初期化
	{
		g_ball.handle = LoadGraph("Resources\\images\\ball_4.png");			//グラフィックハンドル
		g_ball.grp_x = 0;				//画像X
		g_ball.grp_y = 0;				//画像Y
		g_ball.grp_w = 16;				//幅
		g_ball.grp_h = 16;				//高さ
		g_ball.pos_x = g_paddle.pos_x + 20.0f;			//座標X
		g_ball.pos_y = g_paddle.pos_y-g_ball.grp_h;			//座標Y
		g_ball.spd_x = 0.0f;			//速度X
		g_ball.spd_y = 0.0f;			//速度Y
	}

}



//----------------------------------------------------------------------
//! @brief ゲームの更新処理
//!
//! @param[in] なし
//!
//! @return なし
//----------------------------------------------------------------------
void UpdateGame(void)
{
	//キー情報取得
	g_key_code = GetJoypadInputState(DX_INPUT_KEY_PAD1);
	g_key_enter = CheckHitKey(KEY_INPUT_RETURN);

	if (g_scene != g_nextScene)
	{
		g_scene = g_nextScene;
		g_init = false;
	}

	//シーン管理
	switch (g_scene)
	{
	case LOGO:
		GameLogo();
		break;

	case TITLE:
		GameTitle();
		break;

	case PLAY:
		GamePlay();
		break;

	case CLEAR:
		GameClear();
		break;

	case OVER:
		GameOver();
		break;
	}
	//終わりにキー情報入れる
	g_key_old = g_key_code;
	g_key_enter_old = g_key_enter;

}



//----------------------------------------------------------------------
//! @brief ゲームの描画処理
//!
//! @param[in] なし
//!
//! @return なし
//----------------------------------------------------------------------
void RenderGame(void)
{
	//シーン管理
	switch (g_scene)
	{
	case LOGO:
		RenderLogo();
		break;

	case TITLE:
		RenderTitle();
		break;

	case PLAY:
		RenderPlay();
		break;

	case CLEAR:
		RenderClear();
		break;

	case OVER:
		RenderOver();
		break;
	}

}



//----------------------------------------------------------------------
//! @brief ゲームの終了処理
//!
//! @param[in] なし
//!
//! @return なし
//----------------------------------------------------------------------
void FinalizeGame(void)
{

}

//----------------------------------------------------------------------
//! @brief 当たり判定
//!
//! @param[in] 当たり判定したい２つのOBJECT型の構造体
//!
//! @return true(当たってる)、false(当たってない)
//----------------------------------------------------------------------
bool Collision(OBJECT o1, OBJECT o2)
{
	/*四角形の当たり判定*/
	if ((o1.pos_x <= (o2.pos_x + o2.grp_w)) &&
		((o1.pos_x + o1.grp_w) >= o2.pos_x) &&
		(o1.pos_y <= (o2.pos_y + o2.grp_h)) &&
		((o1.pos_y + o1.grp_h) >= o2.pos_y))
	{
		return true;
	}
	else
	{
		return false;
	}

	/*円の判定*/
	//float x1 = o1.pos_x + (o1.grp_w / 2.0f), y1 = o1.pos_y + (o1.grp_h / 2.0f);	//o1の中心座標
	//float x2 = o2.pos_x + (o2.grp_w / 2.0f), y2 = o2.pos_y + (o2.grp_h / 2.0f);	//o2の中心座標

	//float r1 = o1.grp_w / 2.0f;	//o1の半径
	//float r2 = o2.grp_w / 2.0f;	//o2の半径

	////判定
	//if (((x1 - x2)*(x1 - x2)) + ((y1 - y2)*(y1 - y2)) <= ((r1 + r2)*(r1 + r2)))
	//{
	//	//当たってる
	//	return true;
	//}
	//else
	//{
	//	//当たってない
	//	return false;
	//}

}


/*-------------------------------------------------------------------------*/

//----------------------------------------------------------------------
//! @brief ロゴ画面
//!
//! @param[in] なし(void)
//!
//! @return なし(void)
//----------------------------------------------------------------------
void GameLogo(void)
{
	/*ローカル変数*/
	static int color = 0, color_b = 0;

	//フェードインして一定時間経ったらフェードアウト

	//初期化
	if (g_init == false)
	{
		g_cnt = 0;
		g_init = true;
	}
	g_cnt++;

	/*なんかソース見ると名前とかおかしいけど、気にしなーい*/
	/*徐々に明るく(２秒)*/
	if ((color >= 0) && (color < 60))
	{
		SetDrawBright((int)color*4.25, (int)color*4.25, (int)color*4.25);
	}
	/*維持(２秒)*/
	if ((color >= 60) && (color < 180))
	{
		SetDrawBright(255, 255, 255);
	}
	/*徐々に暗く(２秒)*/
	if ((color >= 180) && (color <= 240))
	{
		SetDrawBright((int)255 - (color_b*4.25), (int)255 - (color_b*4.25), (int)255 - (color_b*4.25));
		color_b++;
	}

	//２秒たったらタイトルへ
	if (color >= 300)
	{
		g_nextScene = TITLE;
	}

	color++;

}

void RenderLogo(void)
{
	DrawGraph(0, 0, g_Logo_handle, TRUE);
}

//----------------------------------------------------------------------
//! @brief ゲームのタイトル画面
//!
//! @param[in] なし(void)
//!
//! @return なし(void)
//----------------------------------------------------------------------
void GameTitle(void)
{
	//初期化
	if (g_init == false)
	{
		SetDrawBright(255, 255, 255);	//明るさ戻す
		g_init = true;
	}

	//Enterキー押されたら
	if ((g_key_enter == 1) && (!(g_key_enter_old == 1)))
	{
		g_nextScene = PLAY;
	}
}

void RenderTitle(void)
{
	DrawGraph(0, 0, g_title_handle, TRUE);
	//DrawFormatString(0, 0, GetColor(255, 255, 255), "タイトルだおー\nEnterキーではじまるお\nEscキーで終わるお\n");
}

//----------------------------------------------------------------------
//! @brief ゲームのプレイ画面
//!
//! @param[in] なし(void)
//!
//! @return なし(void)
//----------------------------------------------------------------------
void GamePlay(void)
{
	//初期化
	if (g_init == false)
	{
		InitializeGame();
		g_init = true;
	}

	/*クリア判定*/
	if (g_block_cnt >= MAX_BLOCK)
	{
		g_nextScene = CLEAR;
	}

	/*ゲームオーバー*/
	if ((g_ball.pos_y + g_ball.grp_h) >= SCREEN_HEIGHT)
	{
		g_nextScene = OVER;
	}

	/*ボールの運動*/
	if (start_flag == false)
	{
		g_ball.spd_x = 0.0f;
		g_ball.spd_y = 0.0f;

		//パドルにくっつくように動く
		if (g_key_code & PAD_INPUT_LEFT)	//左キーを押されたら
		{
			if (g_paddle.pos_x > 0.0f)
			{
				g_ball.spd_x = -PADDLE_SPD_X;
			}
		}
		if (g_key_code & PAD_INPUT_RIGHT)	//右キーを押されたら
		{
			if ((g_paddle.pos_x + g_paddle.grp_w) < SCREEN_WIDTH)
			{
				g_ball.spd_x = PADDLE_SPD_X;
			}
		}

		//スペース押されたら発射
		if (g_key_code&PAD_INPUT_10)
		{
			g_ball.spd_x = 3.0f;
			g_ball.spd_y = 3.0f;
			start_flag = true;
		}
	}
	g_ball.pos_x += g_ball.spd_x;
	g_ball.pos_y += g_ball.spd_y;


	/*パドルの運動*/
	g_paddle.pos_x += g_paddle.spd_x;

	/*パドルの移動*/
	g_paddle.spd_x = 0.0f;
	if (g_key_code & PAD_INPUT_LEFT)	//左キーを押されたら
	{
		if (g_paddle.pos_x > 0.0f)
		{
			g_paddle.spd_x = -PADDLE_SPD_X;
		}
	}
	if (g_key_code & PAD_INPUT_RIGHT)	//右キーを押されたら
	{
		if ((g_paddle.pos_x + g_paddle.grp_w) < SCREEN_WIDTH)
		{
			g_paddle.spd_x = PADDLE_SPD_X;
		}
	}
	
	/*ブロックの運動*/
	float temp_pos_x = g_block[0].pos_x;

	for (int i = 0; i < MAX_BLOCK; i++)
	{
		if (temp_pos_x <= 20.0f)
		{
			g_block[i].spd_x = 1.0f;
		}
		if (g_block[MAX_BLOCK - 1].pos_x + g_block[MAX_BLOCK - 1].grp_w >= SCREEN_WIDTH - 20)
		{
			g_block[i].spd_x = -1.0f;
		}
		g_block[i].pos_x += g_block[i].spd_x;
	}

	/*ボールと壁の反射*/
	if ((g_ball.pos_x <= 0.0f) || (g_ball.pos_x >= 640.0f - g_ball.grp_w))	//左右の端にきたら
	{
		g_ball.spd_x *= -1.0f;
	}
	if (g_ball.pos_y <= 0.0f)	//上の端にきたら
	{
		g_ball.spd_y *= -1.0f;
	}

	/*ボールとパドルの当たり判定*/
	if (start_flag == true)
	{
		if ((g_ball.pos_x <= (g_paddle.pos_x + g_paddle.grp_w)) &&
			((g_ball.pos_x + g_ball.grp_w) >= g_paddle.pos_x) &&
			(g_ball.pos_y <= (g_paddle.pos_y + g_paddle.grp_h)) &&
			((g_ball.pos_y + g_ball.grp_h) >= g_paddle.pos_y))
		{
			/*ボールが下に動いてるときのみ反転する*/
			if (g_ball.spd_y > 0.0f)
			{

				///*ボールの位置で角度を変える*/
				////右端で当たったら
				//if ((g_ball.pos_x >= (g_paddle.pos_x + (g_paddle.grp_w - 3.0f))) && (g_ball.pos_x <= (g_paddle.pos_x + g_paddle.grp_w)))
				//{
				//	g_ball.spd_x = 5.0f;
				//}
				////右寄りで当たったら
				//if ((g_ball.pos_x >= g_paddle.pos_x + 46.0f) && (g_ball.pos_x <= (g_paddle.pos_x + (g_paddle.grp_w - 3.0f))))
				//{
				//	g_ball.spd_x = 3.0f;
				//}
				////中央右よりで当たったら
				//if ((g_ball.pos_x >= g_paddle.pos_x + 32.0f) && (g_ball.pos_x <= (g_paddle.pos_x + 46.0f)))
				//{
				//	g_ball.spd_x = 2.0f;
				//}
				////中央左よりで当たったら
				//if ((g_ball.pos_x >= g_paddle.pos_x + 18.0f) && (g_ball.pos_x <= (g_paddle.pos_x + 32.0f)))
				//{
				//	g_ball.spd_x = -2.0f;
				//}
				////左寄りで当たったら
				//if ((g_ball.pos_x >= g_paddle.pos_x + 3.0f) && (g_ball.pos_x <= (g_paddle.pos_x + 18.0f)))
				//{
				//	g_ball.spd_x = -3.0f;
				//}
				////左端で当たったら
				//if ((g_ball.pos_x >= g_paddle.pos_x) && (g_ball.pos_x <= (g_paddle.pos_x + 3.0f)))
				//{
				//	g_ball.spd_x = -5.0f;
				//}

				//反転
				//g_ball.spd_x *= -1.0f;
				g_ball.spd_y *= -1.0f;

			}

			/*パドルの側面の判定*/
			if (g_paddle.pos_x >= (g_ball.pos_x + g_ball.grp_w))
			{
				if (g_ball.spd_x > 0.0f)
				{
					g_ball.spd_x *= -1.0f;
				}
			}
			if ((g_paddle.pos_x + g_paddle.grp_w) <= g_ball.pos_x)
			{
				if (g_ball.spd_x < 0.0f)
				{
					g_ball.spd_x *= -1.0f;
				}
			}

		}
	}

	int collsion_trigger = 0;

	/*ボールとブロックの当たり判定*/
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		//当たったらブロック消す
		if ((Collision(g_ball, g_block[i])) && (g_block[i].state !=0))
		{
			//アイテムブロックは壊したら即さようならー
			if (g_block[i].state == 3)
			{
				if (g_item == 0)
				{
					g_item_flag = true;
				}
				g_item++;
				g_block[i].state = 0;
			}
			//かたいやつはやわらかくする
			if (g_block[i].state == 2)
			{
				g_block[i].handle = g_block_soft_handle;
				g_block[i].state--;
			}
			else if (g_block[i].state == 1)
			{
				g_block[i].state--;
			}
			if (g_block[i].state == 0)
			{
				g_block_cnt++;
			}

			//反射(複数のブロックに当たっても、反射は１回）
			if (collsion_trigger == 0)
			{
				//横に当たったなら
				if ((g_ball.pos_x + 2 >= (g_block[i].pos_x + g_block[i].grp_w)) ||
					((g_ball.pos_x + g_ball.grp_w) - 2 <= g_block[i].pos_x))
				{
					g_ball.spd_x *= -1.0f;
					collsion_trigger++;
				}
				//縦に当たったなら
				if ((g_ball.pos_y + 2 >= (g_block[i].pos_y + g_block[i].grp_h)) ||
					((g_ball.pos_y + g_ball.grp_h)-2 <= g_block[i].pos_y))
				{
					g_ball.spd_y *= -1.0f;
					collsion_trigger++;
				}
			}
		}
	}
}

void RenderPlay(void)
{
	//ブロック
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		if (g_block[i].state > 0)
		{
			DrawRectGraph(g_block[i].pos_x, g_block[i].pos_y, g_block[i].grp_x, g_block[i].grp_y, g_block[i].grp_w, g_block[i].grp_h, g_block[i].handle, TRUE, FALSE);	//ブロック
		}
	}
	//パドル
	DrawRectGraph(g_paddle.pos_x, g_paddle.pos_y, g_paddle.grp_x, g_paddle.grp_y, g_paddle.grp_w, g_paddle.grp_h, g_paddle.handle, TRUE, FALSE);	//パドル
	DrawGraph(g_ball.pos_x, g_ball.pos_y, g_ball.handle, TRUE);					//ボール

	//スペースキーで発射のお知らせ
	if (start_flag == false)
	{
		DrawFormatString(200, 250, GetColor(255, 255, 255), "スペースキーでスタート！\n");
	}

	if (g_item_flag == true)
	{
		DrawFormatString(0, 0, GetColor(255, 255, 255), "Item:%d", g_item);
	}

	//デバッグ情報
	/*for (int i = 0; i < MAX_BLOCK; i++)
	{
		DrawFormatString(0, i * 20, GetColor(255, 255, 255), "g_block[%d].state=%d\n", i, g_block[i].state);
	}*/
//	DrawFormatString(0, 0, GetColor(255, 255, 255), "g_block[0].pos_x=%f\n",g_block[0].pos_x);
//	DrawFormatString(0, 20, GetColor(255, 255, 255), "g_ball_cnt=%d\n", g_block_cnt);
}

//----------------------------------------------------------------------
//! @brief ゲームクリア画面
//!
//! @param[in] なし(void)
//!
//! @return なし(void)
//----------------------------------------------------------------------
void GameClear(void)
{

	/*とりあえずボール止める*/
	g_ball.spd_x = 0.0f;
	g_ball.spd_y = 0.0f;

	//Enterキー押されたら
	if ((g_key_enter == 1) && (!(g_key_enter_old == 1)))
	{
		g_nextScene = TITLE;
	}
}

void RenderClear(void)
{
	//パドル
	DrawRectGraph(g_paddle.pos_x, g_paddle.pos_y, g_paddle.grp_x, g_paddle.grp_y, g_paddle.grp_w, g_paddle.grp_h, g_paddle.handle, TRUE, FALSE);	//パドル
	DrawGraph(g_ball.pos_x, g_ball.pos_y, g_ball.handle, TRUE);					//ボール

	/*クリア演出*/
	DrawFormatString(0, 200, GetColor(255, 255, 255), "くりあー！\nEnterキーでタイトルにもどるお\n");
}

//----------------------------------------------------------------------
//! @brief ゲームオーバー画面
//!
//! @param[in] なし(void)
//!
//! @return なし(void)
//----------------------------------------------------------------------
void GameOver(void)
{

	/*とりあえずボール止める*/
	g_ball.spd_x = 0.0f;
	g_ball.spd_y = 0.0f;

	//Enterキー押されたら
	if ((g_key_enter == 1) && (!(g_key_enter_old == 1)))
	{
		g_nextScene = TITLE;
	}
}

void RenderOver(void)
{
	//ブロック
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		if (g_block[i].state != 0)
		{
			DrawRectGraph(g_block[i].pos_x, g_block[i].pos_y, g_block[i].grp_x, g_block[i].grp_y, g_block[i].grp_w, g_block[i].grp_h, g_block[i].handle, TRUE, FALSE);	//ブロック
		}
	}
	//パドル
	DrawRectGraph(g_paddle.pos_x, g_paddle.pos_y, g_paddle.grp_x, g_paddle.grp_y, g_paddle.grp_w, g_paddle.grp_h, g_paddle.handle, TRUE, FALSE);	//パドル
	DrawGraph(g_ball.pos_x, g_ball.pos_y, g_ball.handle, TRUE);					//ボール

	/*クリア演出*/
	DrawFormatString(0, 200, GetColor(255, 255, 255), "げーむおーばー\nEnterキーでタイトルにもどるお\n");
}