//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/
//! @file   GameMain.cpp
//!
//! @brief  ゲーム関連のソースファイル
//!
//! @date   2016.07.03
//!
//! @author Miyu Hara
//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/


/*一時コメント
・音をつける
*/

// ヘッダファイルの読み込み ================================================
#include "GameMain.h"


/*記号定数*/
#define BLOCK_WIDTH (20)							//ブロックの横の数
#define BLOCK_HEIGHT (6)							//ブロックの縦の数
#define MAX_BLOCK (BLOCK_WIDTH * BLOCK_HEIGHT)		//ブロックの数
#define CHIP_SIZE (32)								//１つのチップのサイズ
#define BLOCK_START_X (0.0f)						//ブロックのスタート位置
#define BLOCK_START_Y (2.0f * CHIP_SIZE)			//ブロックのスタート位置
#define PLAYER_SPD_X (6.0f)							//プレイヤーのスピード

/*構造体*/
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
	int state;		//状態フラグ
}OBJECT;


/*列挙体宣言*/
//シーン管理
enum SCENE
{
	LOGO,	//ロゴ画面
	TITLE,	//タイトル画面
	PLAY,	//プレイ画面
	CLEAR,	//クリア画面
	OVER	//ゲームオーバー画面
};

//ブロック状態(描画フラグ？)
enum BLOCK_STATE
{
	NONE = 10,	//なし
	SAND,	//砂
	DOOR,	//ドア
	BOX,	//宝箱
	STONE,	//石
};

// グローバル変数の定義 ====================================================
//グラフィックハンドル
int g_Logo_handle = 0;			//ロゴ
int g_title_handle = 0;			//タイトル
int g_bg_handle = 0;			//背景
int g_block_sand_handle = 0;	//砂
int g_block_stone_handle = 0;	//石
int g_block_box_handle = 0;		//宝箱
int g_block_door_handle = 0;	//ドア
int g_info_handle = 0;			//インフォメーション

//サウンドハンドル
int g_decision_handle = 0;		//決定音
int g_sand_touch_handle = 0;	//砂に当たったとき
int g_stone_touch_handle = 0;	//石に当たったとき
int g_box_get_handle = 0;		//宝箱壊したとき
int g_door_touch_handle = 0;	//鍵もってないときにドア当たったとき
int g_door_open_handle = 0;		//鍵持っててドア開けたとき
int g_clear_handle = 0;			//クリアしたとき
int g_over_handle = 0;			//ゲームオーバーのとき
int g_jump_handle = 0;			//パドルに当たったとき

//キー情報
int g_key_code = 0;			//キー情報
int g_key_old = 0;			//１フレーム前のキー情報
int g_key_enter = 0;		//Enterキーを押されてるか(GetJoypad～だとEnterキー扱えないため)
int g_key_enter_old = 0;	//g_key_oldのEnterキーばーじょん

//シーン情報
int g_scene = LOGO;			//シーン管理
int g_nextScene = g_scene;	//次のシーン

//オブジェクト情報
OBJECT g_block[MAX_BLOCK];	//ブロック
OBJECT g_player;			//プレイヤー
OBJECT g_ball;				//ボール

//フラグ
bool g_init = false;		//初期化フラグ
bool g_start_flag = false;	//発射フラグ。false:発射してない、true:発射
bool g_item_flag = false;	//鍵を持っているか。false:持っていない、true：持っている
bool g_ball_spd_flag = false;	//スペースキーで速くなる
bool g_door_touch_flag = false;	//ドアに触れたか(これによって出現するメッセージが変わってくる)
bool g_door_break_flag = false;	//ドアを壊したか
bool g_door_flag = false;

//その他
int box_pos_x = 0;
int box_pos_y = 0;


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
	//グラフィックハンドル取得
	g_Logo_handle = LoadGraph("Resources\\images\\my_logo.png");	//ロゴ
	g_title_handle = LoadGraph("Resources\\images\\title.png");		//タイトル
	g_info_handle= LoadGraph("Resources\\images\\info.png");		//インフォ
	g_bg_handle = LoadGraph("Resources\\images\\bg.png");			//背景
	g_block_sand_handle= LoadGraph("Resources\\images\\block_sand.png");
	g_block_stone_handle = LoadGraph("Resources\\images\\block_stone.png");
	g_block_box_handle= LoadGraph("Resources\\images\\block_box.png");
	g_block_door_handle= LoadGraph("Resources\\images\\block_door.png");

	//サウンドハンドル取得
	g_decision_handle = LoadSoundMem("Resources\\Sounds\\decision.mp3");
	g_sand_touch_handle = LoadSoundMem("Resources\\Sounds\\sand.mp3");
	g_stone_touch_handle = LoadSoundMem("Resources\\Sounds\\stone_touch.mp3");
	g_box_get_handle = LoadSoundMem("Resources\\Sounds\\box_get.mp3");
	g_door_touch_handle = LoadSoundMem("Resources\\Sounds\\door_touch.mp3");
	g_door_open_handle = LoadSoundMem("Resources\\Sounds\\door_open.mp3");
	g_clear_handle = LoadSoundMem("Resources\\Sounds\\clear.mp3");
	g_over_handle = LoadSoundMem("Resources\\Sounds\\over.mp3");
	g_jump_handle = LoadSoundMem("Resources\\Sounds\\jump.mp3");

	/*宝箱の位置を決める(yは2～4、xはブロックのwidth以下)*/
	box_pos_x = GetRand(BLOCK_WIDTH) - 1;
	//xが-1のときは0にする
	if (box_pos_x < 0)
	{
		box_pos_x = 0;
	}
	box_pos_y = GetRand(2) + 2;

	//ブロック初期化
	for (int i = 0; i < BLOCK_HEIGHT; i++)			//高さ
	{
		for (int j = 0; j < BLOCK_WIDTH; j++)		//幅
		{
			//ドア
			if (i == 0)
			{
				//石
				if ((j < 6) || (j>14))
				{
					g_block[(i * BLOCK_WIDTH) + j].handle = g_block_stone_handle;
					g_block[(i * BLOCK_WIDTH) + j].state = STONE;
				}
				else
				{
					g_block[(i * BLOCK_WIDTH) + j].handle = g_block_door_handle;
					g_block[(i * BLOCK_WIDTH) + j].state = DOOR;
				}
			}
			//砂
			if (i != 0)
			{
				g_block[(i * BLOCK_WIDTH) + j].handle = g_block_sand_handle;
				g_block[(i * BLOCK_WIDTH) + j].state = SAND;
			}

			//石
			/*if ((i == 2) && (j == 1))
			{
				g_block[(i * BLOCK_WIDTH) + j].handle = g_block_stone_handle;
				g_block[(i * BLOCK_WIDTH) + j].state = STONE;
			}
			if ((i == 3) && (j == 10))
			{
				g_block[(i * BLOCK_WIDTH) + j].handle = g_block_stone_handle;
				g_block[(i * BLOCK_WIDTH) + j].state = STONE;
			}
			if ((i == 4) && (j == 16))
			{
				g_block[(i * BLOCK_WIDTH) + j].handle = g_block_stone_handle;
				g_block[(i * BLOCK_WIDTH) + j].state = STONE;
			}
			if ((i == 6) && (j == 9))
			{
				g_block[(i * BLOCK_WIDTH) + j].handle = g_block_stone_handle;
				g_block[(i * BLOCK_WIDTH) + j].state = STONE;
			}
			if ((i == 7) && (j == 3))
			{
				g_block[(i * BLOCK_WIDTH) + j].handle = g_block_stone_handle;
				g_block[(i * BLOCK_WIDTH) + j].state = STONE;
			}
			if ((i == 8) && (j == 17))
			{
				g_block[(i * BLOCK_WIDTH) + j].handle = g_block_stone_handle;
				g_block[(i * BLOCK_WIDTH) + j].state = STONE;
			}*/

			//宝箱
			if ((i == box_pos_y) && (j == box_pos_x))
			{
				g_block[(i * BLOCK_WIDTH) + j].handle = g_block_box_handle;
				g_block[(i * BLOCK_WIDTH) + j].state = BOX;
			}

			g_block[(i * BLOCK_WIDTH) + j].grp_x = 0;						//画像X
			g_block[(i * BLOCK_WIDTH) + j].grp_y = 0;						//画像Y
			g_block[(i * BLOCK_WIDTH) + j].grp_w = 32;						//幅
			g_block[(i * BLOCK_WIDTH) + j].grp_h = 32;						//高さ
			g_block[(i * BLOCK_WIDTH) + j].pos_x = j*(g_block[i].grp_w) + BLOCK_START_X;		//座標X
			g_block[(i * BLOCK_WIDTH) + j].pos_y = i*(g_block[i].grp_h) + BLOCK_START_Y;		//座標Y
			g_block[(i * BLOCK_WIDTH) + j].spd_x = 0.0f;						//速度X
			g_block[(i * BLOCK_WIDTH) + j].spd_y = 0.0f;						//速度Y
		}
	}

	//プレイヤー
	{
		g_player.handle = LoadGraph("Resources\\images\\paddle.png");
		g_player.grp_x = 0;
		g_player.grp_y = 0;
		g_player.grp_w = 64;
		g_player.grp_h = 16;
		g_player.pos_x = 9.0f * CHIP_SIZE;
		g_player.pos_y = (12.0f * CHIP_SIZE) + (CHIP_SIZE / 4.0f);
		g_player.spd_x = 0.0f;
		g_player.spd_y = 0.0f;
	}

	//ボール初期化
	{
		g_ball.handle = LoadGraph("Resources\\images\\player.png");
		g_ball.grp_x = 0;
		g_ball.grp_y = 0;
		g_ball.grp_w = 20;
		g_ball.grp_h = 25;
		g_ball.pos_x = g_player.pos_x + 20.0f;
		g_ball.pos_y = g_player.pos_y - g_ball.grp_h;
		g_ball.spd_x = 0.0f;
		g_ball.spd_y = 0.0f;
	}

	//フラグ初期化
	g_ball_spd_flag = false;
	g_item_flag = false;
	g_start_flag = false;
	g_door_touch_flag = false;
	g_door_break_flag = false;
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
	/*↓変数名とか処理とか力技すぎるので、余裕あったら整理したい↓*/

	/*ローカル変数*/
	static int color = 0, color_b = 0, time_temp = 0, time_temp2 = 0, time_fadeout = 0;
	static bool fade_flag = false;

	//初期化
	if (g_init == false)
	{
		g_init = true;
	}
	
	//どれかキー押されたらフェードアウトしてタイトルへ
	if (CheckHitKeyAll())
	{
		if (fade_flag == false)
		{
			fade_flag = true;
			time_temp2 = color;
			time_temp = (int)time_temp2*4.25;
			if (time_temp > 255)
			{
				time_temp = 255;
			}
			time_fadeout = color + 60;
			color_b = 0;
		}
	}
	else
	{
		if (fade_flag == false)
		{
			/*徐々に明るく(１秒)*/
			if ((color >= 0) && (color < 60))
			{
				SetDrawBright((int)color*4.25, (int)color*4.25, (int)color*4.25);
			}
			/*維持(２秒)*/
			if ((color >= 60) && (color < 180))
			{
				SetDrawBright(255, 255, 255);
			}
			/*徐々に暗く(１秒)*/
			if ((color >= 180) && (color <= 240))
			{
				SetDrawBright((int)255 - (color_b*4.25), (int)255 - (color_b*4.25), (int)255 - (color_b*4.25));
				color_b++;
			}
		}
	}

	//キー押されてたらフェードアウトする処理
	if (fade_flag == true)
	{
		if ((color >= time_temp2) && (color <= time_fadeout))
		{
			SetDrawBright((int)time_temp - (color_b*(time_temp / 60)), (int)time_temp - (color_b*(time_temp / 60)), (int)time_temp - (color_b*(time_temp / 60)));
			color_b++;
		}
	}

	//タイトルへ
	if ((color >= 300) || (fade_flag == true) && (color >= time_fadeout + 30))
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
		PlaySoundMem(g_decision_handle, DX_PLAYTYPE_BACK);
		g_nextScene = PLAY;
	}
}

void RenderTitle(void)
{
	DrawGraph(0, 0, g_title_handle, TRUE);
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
	if ((g_ball.pos_y + g_ball.grp_h) <= 0.0f)
	{
		g_nextScene = CLEAR;
	}

	/*ゲームオーバー判定*/
	if (g_ball.pos_y > SCREEN_HEIGHT)
	{
		g_nextScene = OVER;
	}


	/*パドルの運動*/
	g_player.pos_x += g_player.spd_x;

	/*パドルのキー処理*/
	g_player.spd_x = 0.0f;
	if (g_key_code & PAD_INPUT_LEFT)	//左キーを押されたら
	{
		if (g_player.pos_x > 0.0f)
		{
			g_player.spd_x = -PLAYER_SPD_X;
		}
	}
	if (g_key_code & PAD_INPUT_RIGHT)	//右キーを押されたら
	{
		if ((g_player.pos_x + g_player.grp_w) < SCREEN_WIDTH)
		{
			g_player.spd_x = PLAYER_SPD_X;
		}
	}

	/*ボールの運動*/
	if (g_start_flag==true)
	{
		//スペースキー押されてる間は速くなる
		if ((g_key_code & PAD_INPUT_10) && (g_ball_spd_flag == false))
		{
			g_ball.spd_x *= 2.0f;
			g_ball.spd_y *= 2.0f;
			g_ball_spd_flag = true;
		}
		else
		{
			if (g_ball.spd_x > 0.0f)
			{
				g_ball.spd_x = 2.0f;
			}
			else
			{
				g_ball.spd_x = -2.0f;
			}
			if (g_ball.spd_y > 0.0f)
			{
				g_ball.spd_y = 2.0f;
			}
			else
			{
				g_ball.spd_y = -2.0f;
			}
			g_ball_spd_flag = false;
		}
	}
	
	g_ball.pos_x += g_ball.spd_x;
	g_ball.pos_y += g_ball.spd_y;

	/*ボールのキー処理*/
	if (g_start_flag == false)
	{
		g_ball.spd_x = 0.0f;
		g_ball.spd_y = 0.0f;

		//パドルにくっつくように動く
		if (g_key_code & PAD_INPUT_LEFT)	//左キーを押されたら
		{
			if (g_player.pos_x > 0.0f)
			{
				g_ball.spd_x = -PLAYER_SPD_X;
			}
		}
		if (g_key_code & PAD_INPUT_RIGHT)	//右キーを押されたら
		{
			if ((g_player.pos_x + g_player.grp_w) < SCREEN_WIDTH)
			{
				g_ball.spd_x = PLAYER_SPD_X;
			}
		}

		//スペース押されたら発射
		if ((g_key_enter == 1) && (!(g_key_enter_old == 1)))
		{
			g_ball.spd_x = 2.0f;
			g_ball.spd_y = -2.0f;
			g_start_flag = true;
			PlaySoundMem(g_jump_handle, DX_PLAYTYPE_BACK);
		}
	}

	/*ボールと壁の反射*/
	if ((g_ball.pos_x <= 0.0f) || (g_ball.pos_x >= 640.0f - g_ball.grp_w))	//左右の端にきたら
	{
		g_ball.spd_x *= -1.0f;
		PlaySoundMem(g_stone_touch_handle, DX_PLAYTYPE_BACK);
	}
	//if (g_ball.pos_y <= 0.0f)	//上の端にきたら
	//{
	//	g_ball.spd_y *= -1.0f;
	//}

	/*ボールとパドルの当たり判定*/
	if (g_start_flag == true)
	{
		if ((g_ball.pos_x <= (g_player.pos_x + g_player.grp_w)) &&
			((g_ball.pos_x + g_ball.grp_w) >= g_player.pos_x) &&
			(g_ball.pos_y <= (g_player.pos_y + 16.0f)) &&
			((g_ball.pos_y + g_ball.grp_h) >= g_player.pos_y))
		{
			/*ボールが下に動いてるときのみ反転する*/
			if (g_ball.spd_y > 0.0f)
			{

				///*ボールの位置で角度を変える*/
				/*左端～中央で当たったら*/
				if (((g_ball.pos_x + (g_ball.grp_w / 2)) >= g_player.pos_x) && (g_ball.pos_x <= ((g_player.pos_x + (g_player.grp_w / 2)) - (g_ball.grp_w / 2))))
				{
					g_ball.spd_x = -1.0f;
				}
				/*中央～右端で当たったら*/
				if ((g_ball.pos_x > ((g_player.pos_x + (g_player.grp_w / 2)) - (g_ball.grp_w / 2))) && (g_ball.pos_x <= (g_player.pos_x + g_player.grp_w)))
				{
					g_ball.spd_x = 1.0f;
				}

				//反転
				/*if (g_ball.spd_x < 0.0f)
				{
					g_ball.spd_x *= -1.0f;
				}*/
				g_ball.spd_y *= -1.0f;

			}

			/*パドルの側面の判定*/
			if (g_player.pos_x >= (g_ball.pos_x + g_ball.grp_w))
			{
				if (g_ball.spd_x > 0.0f)
				{
					g_ball.spd_x *= -1.0f;
				}
			}
			if ((g_player.pos_x + g_player.grp_w) <= g_ball.pos_x)
			{
				if (g_ball.spd_x < 0.0f)
				{
					g_ball.spd_x *= -1.0f;
				}
			}

			PlaySoundMem(g_jump_handle, DX_PLAYTYPE_BACK);

		}
	}

	int collsion_trigger = 0;

	/*ボールとブロックの当たり判定*/
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		//当たったらブロック消す
		if ((Collision(g_ball, g_block[i])) && (g_block[i].state != NONE))
		{
			//宝箱は壊したら即さようならー
			if (g_block[i].state == BOX)
			{
				g_item_flag = true;
				g_block[i].state = NONE;
				PlaySoundMem(g_box_get_handle, DX_PLAYTYPE_BACK);
			}
			//砂は消す
			if (g_block[i].state == SAND)
			{
				g_block[i].state = NONE;
				PlaySoundMem(g_sand_touch_handle, DX_PLAYTYPE_BACK);
			}
			//扉は鍵を持ってたら壊せる
			if ((g_block[i].state == DOOR) && (g_item_flag == true))
			{
				g_block[i].state = NONE;
				g_door_break_flag = true;
				PlaySoundMem(g_door_open_handle, DX_PLAYTYPE_BACK);
			}
			if ((g_block[i].state == DOOR) && (g_item_flag== false))
			{
				g_door_touch_flag = true;
				PlaySoundMem(g_door_touch_handle, DX_PLAYTYPE_BACK);
			}
			if (g_block[i].state == STONE)
			{
				PlaySoundMem(g_stone_touch_handle, DX_PLAYTYPE_BACK);
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
					((g_ball.pos_y + g_ball.grp_h) - 2 <= g_block[i].pos_y))
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
	DrawGraph(0, 0, g_bg_handle, TRUE);	//背景

	//ブロック
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		if (g_block[i].state > NONE)
		{
			DrawRectGraph(g_block[i].pos_x, g_block[i].pos_y, g_block[i].grp_x, g_block[i].grp_y, g_block[i].grp_w, g_block[i].grp_h, g_block[i].handle, TRUE, FALSE);	//ブロック
		}
	}

	DrawGraph(g_player.pos_x, g_player.pos_y, g_player.handle, TRUE);	//プレイヤー
	DrawGraph(g_ball.pos_x, g_ball.pos_y, g_ball.handle, TRUE);			//ボール

	DrawGraph(4, (13 * CHIP_SIZE) + 2, g_info_handle, TRUE);			//インフォ

	//スペースキーで発射のお知らせ
	if (g_start_flag == false)
	{
		DrawFormatString(30, (13 * CHIP_SIZE) + 18, GetColor(255, 255, 255), "パドルを動かして女の子を脱出させよう！\n←キー：左移動　→キー：右移動　　エンターキーでスタート");
	}

	//鍵持ってないのにドアに当たったらお知らせ
	if ((g_door_touch_flag == true) && (g_item_flag == false))
	{
		DrawFormatString(30, (13 * CHIP_SIZE) + 18, GetColor(255, 255, 255), "女の子「とびらはかぎがかかってるみたい・・・」");
	}

	//宝箱開けたらお知らせ
	if (g_item_flag == true)
	{
		if ((g_door_touch_flag == false) && (g_door_break_flag == false))
		{
			DrawFormatString(30, (13 * CHIP_SIZE) + 18, GetColor(255, 255, 255), "女の子「あ、宝箱の中にかぎが入ってた！\n　　　　これどうするのかなぁ？」");
		}
		if ((g_door_touch_flag == true) && (g_door_break_flag == false))
		{
			DrawFormatString(30, (13 * CHIP_SIZE) + 18, GetColor(255, 255, 255), "女の子「あ、宝箱の中にかぎが入ってた！\n　　　　これでさっきのとびら、あけられるかも」");
		}
	}

	//ドアあけたらお知らせ
	if (g_door_break_flag == true)
	{
		DrawFormatString(30, (13 * CHIP_SIZE) + 18, GetColor(255, 255, 255), "女の子「かぎを使ったら、とびらがあいた！」");
	}
	
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
	static bool clear_flag = false;

	/*とりあえずボール止める*/
	g_ball.spd_x = 0.0f;
	g_ball.spd_y = 0.0f;

	if (clear_flag == false)
	{
		PlaySoundMem(g_clear_handle, DX_PLAYTYPE_BACK);
		clear_flag = true;
	}

	//Enterキー押されたら
	if ((g_key_enter == 1) && (!(g_key_enter_old == 1)))
	{
		clear_flag = false;
		StopSoundMem(g_clear_handle);
		PlaySoundMem(g_decision_handle, DX_PLAYTYPE_BACK);
		g_nextScene = TITLE;
	}
}

void RenderClear(void)
{
	DrawGraph(0, 0, g_bg_handle, TRUE);	//背景
	//ブロック
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		if (g_block[i].state != NONE)
		{
			DrawRectGraph(g_block[i].pos_x, g_block[i].pos_y, g_block[i].grp_x, g_block[i].grp_y, g_block[i].grp_w, g_block[i].grp_h, g_block[i].handle, TRUE, FALSE);	//ブロック
		}
	}
	DrawRectGraph(g_player.pos_x, g_player.pos_y, g_player.grp_x, g_player.grp_y, g_player.grp_w, g_player.grp_h, g_player.handle, TRUE, FALSE);	//プレイヤー
	DrawGraph(g_ball.pos_x, g_ball.pos_y, g_ball.handle, TRUE);	//ボール

	DrawGraph(4, (13 * CHIP_SIZE) + 2, g_info_handle, TRUE);			//インフォ

	/*クリア演出*/
	DrawFormatString(30, (13 * CHIP_SIZE) + 18, GetColor(255, 255, 255), "女の子は脱出することができた！\nEnterキーでタイトルにもどります\n");
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
	static bool over_flag = false;
	
	/*とりあえずボール止める*/
	g_ball.spd_x = 0.0f;
	g_ball.spd_y = 0.0f;

	if (over_flag == false)
	{
		PlaySoundMem(g_over_handle, DX_PLAYTYPE_BACK);
		over_flag = true;
	}

	//Enterキー押されたら
	if ((g_key_enter == 1) && (!(g_key_enter_old == 1)))
	{
		over_flag = false;
		StopSoundMem(g_over_handle);
		PlaySoundMem(g_decision_handle, DX_PLAYTYPE_BACK);
		g_nextScene = TITLE;
	}
}

void RenderOver(void)
{
	DrawGraph(0, 0, g_bg_handle, TRUE);	//背景
	//ブロック
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		if (g_block[i].state != NONE)
		{
			DrawRectGraph(g_block[i].pos_x, g_block[i].pos_y, g_block[i].grp_x, g_block[i].grp_y, g_block[i].grp_w, g_block[i].grp_h, g_block[i].handle, TRUE, FALSE);	//ブロック
		}
	}
	DrawRectGraph(g_player.pos_x, g_player.pos_y, g_player.grp_x, g_player.grp_y, g_player.grp_w, g_player.grp_h, g_player.handle, TRUE, FALSE);	//プレイヤー
//	DrawGraph(g_ball.pos_x, g_ball.pos_y, g_ball.handle, TRUE);	//ボール

	DrawGraph(4, (13 * CHIP_SIZE) + 2, g_info_handle, TRUE);			//インフォ

	/*ゲームオーバー演出*/
	DrawFormatString(30, (13 * CHIP_SIZE) + 18, GetColor(255, 255, 255), "女の子は奈落のそこへ落ちていった・・・\nEnterキーでタイトルにもどります");
}