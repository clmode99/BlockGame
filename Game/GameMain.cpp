//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/
//! @file   GameMain.cpp
//!
//! @brief  �Q�[���֘A�̃\�[�X�t�@�C��
//!
//! @date   2016.07.03
//!
//! @author Miyu Hara
//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/


/*�ꎞ�R�����g
�E��������
*/

// �w�b�_�t�@�C���̓ǂݍ��� ================================================
#include "GameMain.h"


/*�L���萔*/
#define BLOCK_WIDTH (20)							//�u���b�N�̉��̐�
#define BLOCK_HEIGHT (6)							//�u���b�N�̏c�̐�
#define MAX_BLOCK (BLOCK_WIDTH * BLOCK_HEIGHT)		//�u���b�N�̐�
#define CHIP_SIZE (32)								//�P�̃`�b�v�̃T�C�Y
#define BLOCK_START_X (0.0f)						//�u���b�N�̃X�^�[�g�ʒu
#define BLOCK_START_Y (2.0f * CHIP_SIZE)			//�u���b�N�̃X�^�[�g�ʒu
#define PLAYER_SPD_X (6.0f)							//�v���C���[�̃X�s�[�h

/*�\����*/
typedef struct tag_object
{
	HGRP handle;	//�O���t�B�b�N�n���h��
	int grp_x;		//�摜X
	int grp_y;		//�摜Y
	int grp_w;		//�摜�̕�
	int grp_h;		//�摜�̍���
	float pos_x;	//���WX
	float pos_y;	//���WY
	float spd_x;	//���xX
	float spd_y;	//���xY
	int state;		//��ԃt���O
}OBJECT;


/*�񋓑̐錾*/
//�V�[���Ǘ�
enum SCENE
{
	LOGO,	//���S���
	TITLE,	//�^�C�g�����
	PLAY,	//�v���C���
	CLEAR,	//�N���A���
	OVER	//�Q�[���I�[�o�[���
};

//�u���b�N���(�`��t���O�H)
enum BLOCK_STATE
{
	NONE = 10,	//�Ȃ�
	SAND,	//��
	DOOR,	//�h�A
	BOX,	//��
	STONE,	//��
};

// �O���[�o���ϐ��̒�` ====================================================
//�O���t�B�b�N�n���h��
int g_Logo_handle = 0;			//���S
int g_title_handle = 0;			//�^�C�g��
int g_bg_handle = 0;			//�w�i
int g_block_sand_handle = 0;	//��
int g_block_stone_handle = 0;	//��
int g_block_box_handle = 0;		//��
int g_block_door_handle = 0;	//�h�A
int g_info_handle = 0;			//�C���t�H���[�V����

//�T�E���h�n���h��
int g_decision_handle = 0;		//���艹
int g_sand_touch_handle = 0;	//���ɓ��������Ƃ�
int g_stone_touch_handle = 0;	//�΂ɓ��������Ƃ�
int g_box_get_handle = 0;		//�󔠉󂵂��Ƃ�
int g_door_touch_handle = 0;	//�������ĂȂ��Ƃ��Ƀh�A���������Ƃ�
int g_door_open_handle = 0;		//�������Ăăh�A�J�����Ƃ�
int g_clear_handle = 0;			//�N���A�����Ƃ�
int g_over_handle = 0;			//�Q�[���I�[�o�[�̂Ƃ�
int g_jump_handle = 0;			//�p�h���ɓ��������Ƃ�

//�L�[���
int g_key_code = 0;			//�L�[���
int g_key_old = 0;			//�P�t���[���O�̃L�[���
int g_key_enter = 0;		//Enter�L�[��������Ă邩(GetJoypad�`����Enter�L�[�����Ȃ�����)
int g_key_enter_old = 0;	//g_key_old��Enter�L�[�΁[�����

//�V�[�����
int g_scene = LOGO;			//�V�[���Ǘ�
int g_nextScene = g_scene;	//���̃V�[��

//�I�u�W�F�N�g���
OBJECT g_block[MAX_BLOCK];	//�u���b�N
OBJECT g_player;			//�v���C���[
OBJECT g_ball;				//�{�[��

//�t���O
bool g_init = false;		//�������t���O
bool g_start_flag = false;	//���˃t���O�Bfalse:���˂��ĂȂ��Atrue:����
bool g_item_flag = false;	//���������Ă��邩�Bfalse:�����Ă��Ȃ��Atrue�F�����Ă���
bool g_ball_spd_flag = false;	//�X�y�[�X�L�[�ő����Ȃ�
bool g_door_touch_flag = false;	//�h�A�ɐG�ꂽ��(����ɂ���ďo�����郁�b�Z�[�W���ς���Ă���)
bool g_door_break_flag = false;	//�h�A���󂵂���
bool g_door_flag = false;

//���̑�
int box_pos_x = 0;
int box_pos_y = 0;


/*�֐��̃v���g�^�C�v�錾*/
bool Collision(OBJECT o1, OBJECT o2);	//�����蔻��
void GameLogo(void);					//���S���
void RenderLogo(void);					//���S��ʂ̕`��
void GameTitle(void);					//�^�C�g�����
void RenderTitle(void);					//�^�C�g����ʂ̕`��
void GamePlay(void);					//�v���C���
void RenderPlay(void);					//�v���C��ʂ̕`��
void GameClear(void);					//�N���A���
void RenderClear(void);					//�N���A��ʂ̕`��
void GameOver(void);					//�Q�[���I�[�o�[���
void RenderOver(void);					//�Q�[���I�[�o�[��ʂ̕`��


// �֐��̒�` ==============================================================

//----------------------------------------------------------------------
//! @brief �Q�[���̏���������
//!
//! @param[in] �Ȃ�
//!
//! @return �Ȃ�
//----------------------------------------------------------------------
void InitializeGame(void)
{
	//�O���t�B�b�N�n���h���擾
	g_Logo_handle = LoadGraph("Resources\\images\\my_logo.png");	//���S
	g_title_handle = LoadGraph("Resources\\images\\title.png");		//�^�C�g��
	g_info_handle= LoadGraph("Resources\\images\\info.png");		//�C���t�H
	g_bg_handle = LoadGraph("Resources\\images\\bg.png");			//�w�i
	g_block_sand_handle= LoadGraph("Resources\\images\\block_sand.png");
	g_block_stone_handle = LoadGraph("Resources\\images\\block_stone.png");
	g_block_box_handle= LoadGraph("Resources\\images\\block_box.png");
	g_block_door_handle= LoadGraph("Resources\\images\\block_door.png");

	//�T�E���h�n���h���擾
	g_decision_handle = LoadSoundMem("Resources\\Sounds\\decision.mp3");
	g_sand_touch_handle = LoadSoundMem("Resources\\Sounds\\sand.mp3");
	g_stone_touch_handle = LoadSoundMem("Resources\\Sounds\\stone_touch.mp3");
	g_box_get_handle = LoadSoundMem("Resources\\Sounds\\box_get.mp3");
	g_door_touch_handle = LoadSoundMem("Resources\\Sounds\\door_touch.mp3");
	g_door_open_handle = LoadSoundMem("Resources\\Sounds\\door_open.mp3");
	g_clear_handle = LoadSoundMem("Resources\\Sounds\\clear.mp3");
	g_over_handle = LoadSoundMem("Resources\\Sounds\\over.mp3");
	g_jump_handle = LoadSoundMem("Resources\\Sounds\\jump.mp3");

	/*�󔠂̈ʒu�����߂�(y��2�`4�Ax�̓u���b�N��width�ȉ�)*/
	box_pos_x = GetRand(BLOCK_WIDTH) - 1;
	//x��-1�̂Ƃ���0�ɂ���
	if (box_pos_x < 0)
	{
		box_pos_x = 0;
	}
	box_pos_y = GetRand(2) + 2;

	//�u���b�N������
	for (int i = 0; i < BLOCK_HEIGHT; i++)			//����
	{
		for (int j = 0; j < BLOCK_WIDTH; j++)		//��
		{
			//�h�A
			if (i == 0)
			{
				//��
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
			//��
			if (i != 0)
			{
				g_block[(i * BLOCK_WIDTH) + j].handle = g_block_sand_handle;
				g_block[(i * BLOCK_WIDTH) + j].state = SAND;
			}

			//��
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

			//��
			if ((i == box_pos_y) && (j == box_pos_x))
			{
				g_block[(i * BLOCK_WIDTH) + j].handle = g_block_box_handle;
				g_block[(i * BLOCK_WIDTH) + j].state = BOX;
			}

			g_block[(i * BLOCK_WIDTH) + j].grp_x = 0;						//�摜X
			g_block[(i * BLOCK_WIDTH) + j].grp_y = 0;						//�摜Y
			g_block[(i * BLOCK_WIDTH) + j].grp_w = 32;						//��
			g_block[(i * BLOCK_WIDTH) + j].grp_h = 32;						//����
			g_block[(i * BLOCK_WIDTH) + j].pos_x = j*(g_block[i].grp_w) + BLOCK_START_X;		//���WX
			g_block[(i * BLOCK_WIDTH) + j].pos_y = i*(g_block[i].grp_h) + BLOCK_START_Y;		//���WY
			g_block[(i * BLOCK_WIDTH) + j].spd_x = 0.0f;						//���xX
			g_block[(i * BLOCK_WIDTH) + j].spd_y = 0.0f;						//���xY
		}
	}

	//�v���C���[
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

	//�{�[��������
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

	//�t���O������
	g_ball_spd_flag = false;
	g_item_flag = false;
	g_start_flag = false;
	g_door_touch_flag = false;
	g_door_break_flag = false;
}



//----------------------------------------------------------------------
//! @brief �Q�[���̍X�V����
//!
//! @param[in] �Ȃ�
//!
//! @return �Ȃ�
//----------------------------------------------------------------------
void UpdateGame(void)
{

	//�L�[���擾
	g_key_code = GetJoypadInputState(DX_INPUT_KEY_PAD1);
	g_key_enter = CheckHitKey(KEY_INPUT_RETURN);

	if (g_scene != g_nextScene)
	{
		g_scene = g_nextScene;
		g_init = false;
	}

	//�V�[���Ǘ�
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

	//�I���ɃL�[�������
	g_key_old = g_key_code;
	g_key_enter_old = g_key_enter;

}



//----------------------------------------------------------------------
//! @brief �Q�[���̕`�揈��
//!
//! @param[in] �Ȃ�
//!
//! @return �Ȃ�
//----------------------------------------------------------------------
void RenderGame(void)
{
	//�V�[���Ǘ�
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
//! @brief �Q�[���̏I������
//!
//! @param[in] �Ȃ�
//!
//! @return �Ȃ�
//----------------------------------------------------------------------
void FinalizeGame(void)
{

}

//----------------------------------------------------------------------
//! @brief �����蔻��
//!
//! @param[in] �����蔻�肵�����Q��OBJECT�^�̍\����
//!
//! @return true(�������Ă�)�Afalse(�������ĂȂ�)
//----------------------------------------------------------------------
bool Collision(OBJECT o1, OBJECT o2)
{
	/*�l�p�`�̓����蔻��*/
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
//! @brief ���S���
//!
//! @param[in] �Ȃ�(void)
//!
//! @return �Ȃ�(void)
//----------------------------------------------------------------------
void GameLogo(void)
{
	/*���ϐ����Ƃ������Ƃ��͋Z������̂ŁA�]�T�������琮����������*/

	/*���[�J���ϐ�*/
	static int color = 0, color_b = 0, time_temp = 0, time_temp2 = 0, time_fadeout = 0;
	static bool fade_flag = false;

	//������
	if (g_init == false)
	{
		g_init = true;
	}
	
	//�ǂꂩ�L�[�����ꂽ��t�F�[�h�A�E�g���ă^�C�g����
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
			/*���X�ɖ��邭(�P�b)*/
			if ((color >= 0) && (color < 60))
			{
				SetDrawBright((int)color*4.25, (int)color*4.25, (int)color*4.25);
			}
			/*�ێ�(�Q�b)*/
			if ((color >= 60) && (color < 180))
			{
				SetDrawBright(255, 255, 255);
			}
			/*���X�ɈÂ�(�P�b)*/
			if ((color >= 180) && (color <= 240))
			{
				SetDrawBright((int)255 - (color_b*4.25), (int)255 - (color_b*4.25), (int)255 - (color_b*4.25));
				color_b++;
			}
		}
	}

	//�L�[������Ă���t�F�[�h�A�E�g���鏈��
	if (fade_flag == true)
	{
		if ((color >= time_temp2) && (color <= time_fadeout))
		{
			SetDrawBright((int)time_temp - (color_b*(time_temp / 60)), (int)time_temp - (color_b*(time_temp / 60)), (int)time_temp - (color_b*(time_temp / 60)));
			color_b++;
		}
	}

	//�^�C�g����
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
//! @brief �Q�[���̃^�C�g�����
//!
//! @param[in] �Ȃ�(void)
//!
//! @return �Ȃ�(void)
//----------------------------------------------------------------------
void GameTitle(void)
{
	//������
	if (g_init == false)
	{
		SetDrawBright(255, 255, 255);	//���邳�߂�
		g_init = true;
	}

	//Enter�L�[�����ꂽ��
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
//! @brief �Q�[���̃v���C���
//!
//! @param[in] �Ȃ�(void)
//!
//! @return �Ȃ�(void)
//----------------------------------------------------------------------
void GamePlay(void)
{
	//������
	if (g_init == false)
	{
		InitializeGame();
		g_init = true;
	}

	/*�N���A����*/
	if ((g_ball.pos_y + g_ball.grp_h) <= 0.0f)
	{
		g_nextScene = CLEAR;
	}

	/*�Q�[���I�[�o�[����*/
	if (g_ball.pos_y > SCREEN_HEIGHT)
	{
		g_nextScene = OVER;
	}


	/*�p�h���̉^��*/
	g_player.pos_x += g_player.spd_x;

	/*�p�h���̃L�[����*/
	g_player.spd_x = 0.0f;
	if (g_key_code & PAD_INPUT_LEFT)	//���L�[�������ꂽ��
	{
		if (g_player.pos_x > 0.0f)
		{
			g_player.spd_x = -PLAYER_SPD_X;
		}
	}
	if (g_key_code & PAD_INPUT_RIGHT)	//�E�L�[�������ꂽ��
	{
		if ((g_player.pos_x + g_player.grp_w) < SCREEN_WIDTH)
		{
			g_player.spd_x = PLAYER_SPD_X;
		}
	}

	/*�{�[���̉^��*/
	if (g_start_flag==true)
	{
		//�X�y�[�X�L�[������Ă�Ԃ͑����Ȃ�
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

	/*�{�[���̃L�[����*/
	if (g_start_flag == false)
	{
		g_ball.spd_x = 0.0f;
		g_ball.spd_y = 0.0f;

		//�p�h���ɂ������悤�ɓ���
		if (g_key_code & PAD_INPUT_LEFT)	//���L�[�������ꂽ��
		{
			if (g_player.pos_x > 0.0f)
			{
				g_ball.spd_x = -PLAYER_SPD_X;
			}
		}
		if (g_key_code & PAD_INPUT_RIGHT)	//�E�L�[�������ꂽ��
		{
			if ((g_player.pos_x + g_player.grp_w) < SCREEN_WIDTH)
			{
				g_ball.spd_x = PLAYER_SPD_X;
			}
		}

		//�X�y�[�X�����ꂽ�甭��
		if ((g_key_enter == 1) && (!(g_key_enter_old == 1)))
		{
			g_ball.spd_x = 2.0f;
			g_ball.spd_y = -2.0f;
			g_start_flag = true;
			PlaySoundMem(g_jump_handle, DX_PLAYTYPE_BACK);
		}
	}

	/*�{�[���ƕǂ̔���*/
	if ((g_ball.pos_x <= 0.0f) || (g_ball.pos_x >= 640.0f - g_ball.grp_w))	//���E�̒[�ɂ�����
	{
		g_ball.spd_x *= -1.0f;
		PlaySoundMem(g_stone_touch_handle, DX_PLAYTYPE_BACK);
	}
	//if (g_ball.pos_y <= 0.0f)	//��̒[�ɂ�����
	//{
	//	g_ball.spd_y *= -1.0f;
	//}

	/*�{�[���ƃp�h���̓����蔻��*/
	if (g_start_flag == true)
	{
		if ((g_ball.pos_x <= (g_player.pos_x + g_player.grp_w)) &&
			((g_ball.pos_x + g_ball.grp_w) >= g_player.pos_x) &&
			(g_ball.pos_y <= (g_player.pos_y + 16.0f)) &&
			((g_ball.pos_y + g_ball.grp_h) >= g_player.pos_y))
		{
			/*�{�[�������ɓ����Ă�Ƃ��̂ݔ��]����*/
			if (g_ball.spd_y > 0.0f)
			{

				///*�{�[���̈ʒu�Ŋp�x��ς���*/
				/*���[�`�����œ���������*/
				if (((g_ball.pos_x + (g_ball.grp_w / 2)) >= g_player.pos_x) && (g_ball.pos_x <= ((g_player.pos_x + (g_player.grp_w / 2)) - (g_ball.grp_w / 2))))
				{
					g_ball.spd_x = -1.0f;
				}
				/*�����`�E�[�œ���������*/
				if ((g_ball.pos_x > ((g_player.pos_x + (g_player.grp_w / 2)) - (g_ball.grp_w / 2))) && (g_ball.pos_x <= (g_player.pos_x + g_player.grp_w)))
				{
					g_ball.spd_x = 1.0f;
				}

				//���]
				/*if (g_ball.spd_x < 0.0f)
				{
					g_ball.spd_x *= -1.0f;
				}*/
				g_ball.spd_y *= -1.0f;

			}

			/*�p�h���̑��ʂ̔���*/
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

	/*�{�[���ƃu���b�N�̓����蔻��*/
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		//����������u���b�N����
		if ((Collision(g_ball, g_block[i])) && (g_block[i].state != NONE))
		{
			//�󔠂͉󂵂��瑦���悤�Ȃ�[
			if (g_block[i].state == BOX)
			{
				g_item_flag = true;
				g_block[i].state = NONE;
				PlaySoundMem(g_box_get_handle, DX_PLAYTYPE_BACK);
			}
			//���͏���
			if (g_block[i].state == SAND)
			{
				g_block[i].state = NONE;
				PlaySoundMem(g_sand_touch_handle, DX_PLAYTYPE_BACK);
			}
			//���͌��������Ă���󂹂�
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

			//����(�����̃u���b�N�ɓ������Ă��A���˂͂P��j
			if (collsion_trigger == 0)
			{
				//���ɓ��������Ȃ�
				if ((g_ball.pos_x + 2 >= (g_block[i].pos_x + g_block[i].grp_w)) ||
					((g_ball.pos_x + g_ball.grp_w) - 2 <= g_block[i].pos_x))
				{
					g_ball.spd_x *= -1.0f;
					collsion_trigger++;
				}
				//�c�ɓ��������Ȃ�
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
	DrawGraph(0, 0, g_bg_handle, TRUE);	//�w�i

	//�u���b�N
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		if (g_block[i].state > NONE)
		{
			DrawRectGraph(g_block[i].pos_x, g_block[i].pos_y, g_block[i].grp_x, g_block[i].grp_y, g_block[i].grp_w, g_block[i].grp_h, g_block[i].handle, TRUE, FALSE);	//�u���b�N
		}
	}

	DrawGraph(g_player.pos_x, g_player.pos_y, g_player.handle, TRUE);	//�v���C���[
	DrawGraph(g_ball.pos_x, g_ball.pos_y, g_ball.handle, TRUE);			//�{�[��

	DrawGraph(4, (13 * CHIP_SIZE) + 2, g_info_handle, TRUE);			//�C���t�H

	//�X�y�[�X�L�[�Ŕ��˂̂��m�点
	if (g_start_flag == false)
	{
		DrawFormatString(30, (13 * CHIP_SIZE) + 18, GetColor(255, 255, 255), "�p�h���𓮂����ď��̎q��E�o�����悤�I\n���L�[�F���ړ��@���L�[�F�E�ړ��@�@�G���^�[�L�[�ŃX�^�[�g");
	}

	//�������ĂȂ��̂Ƀh�A�ɓ��������炨�m�点
	if ((g_door_touch_flag == true) && (g_item_flag == false))
	{
		DrawFormatString(30, (13 * CHIP_SIZE) + 18, GetColor(255, 255, 255), "���̎q�u�Ƃт�͂������������Ă�݂����E�E�E�v");
	}

	//�󔠊J�����炨�m�点
	if (g_item_flag == true)
	{
		if ((g_door_touch_flag == false) && (g_door_break_flag == false))
		{
			DrawFormatString(30, (13 * CHIP_SIZE) + 18, GetColor(255, 255, 255), "���̎q�u���A�󔠂̒��ɂ����������Ă��I\n�@�@�@�@����ǂ�����̂��Ȃ��H�v");
		}
		if ((g_door_touch_flag == true) && (g_door_break_flag == false))
		{
			DrawFormatString(30, (13 * CHIP_SIZE) + 18, GetColor(255, 255, 255), "���̎q�u���A�󔠂̒��ɂ����������Ă��I\n�@�@�@�@����ł������̂Ƃт�A�������邩���v");
		}
	}

	//�h�A�������炨�m�点
	if (g_door_break_flag == true)
	{
		DrawFormatString(30, (13 * CHIP_SIZE) + 18, GetColor(255, 255, 255), "���̎q�u�������g������A�Ƃт炪�������I�v");
	}
	
}

//----------------------------------------------------------------------
//! @brief �Q�[���N���A���
//!
//! @param[in] �Ȃ�(void)
//!
//! @return �Ȃ�(void)
//----------------------------------------------------------------------
void GameClear(void)
{
	static bool clear_flag = false;

	/*�Ƃ肠�����{�[���~�߂�*/
	g_ball.spd_x = 0.0f;
	g_ball.spd_y = 0.0f;

	if (clear_flag == false)
	{
		PlaySoundMem(g_clear_handle, DX_PLAYTYPE_BACK);
		clear_flag = true;
	}

	//Enter�L�[�����ꂽ��
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
	DrawGraph(0, 0, g_bg_handle, TRUE);	//�w�i
	//�u���b�N
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		if (g_block[i].state != NONE)
		{
			DrawRectGraph(g_block[i].pos_x, g_block[i].pos_y, g_block[i].grp_x, g_block[i].grp_y, g_block[i].grp_w, g_block[i].grp_h, g_block[i].handle, TRUE, FALSE);	//�u���b�N
		}
	}
	DrawRectGraph(g_player.pos_x, g_player.pos_y, g_player.grp_x, g_player.grp_y, g_player.grp_w, g_player.grp_h, g_player.handle, TRUE, FALSE);	//�v���C���[
	DrawGraph(g_ball.pos_x, g_ball.pos_y, g_ball.handle, TRUE);	//�{�[��

	DrawGraph(4, (13 * CHIP_SIZE) + 2, g_info_handle, TRUE);			//�C���t�H

	/*�N���A���o*/
	DrawFormatString(30, (13 * CHIP_SIZE) + 18, GetColor(255, 255, 255), "���̎q�͒E�o���邱�Ƃ��ł����I\nEnter�L�[�Ń^�C�g���ɂ��ǂ�܂�\n");
}

//----------------------------------------------------------------------
//! @brief �Q�[���I�[�o�[���
//!
//! @param[in] �Ȃ�(void)
//!
//! @return �Ȃ�(void)
//----------------------------------------------------------------------
void GameOver(void)
{
	static bool over_flag = false;
	
	/*�Ƃ肠�����{�[���~�߂�*/
	g_ball.spd_x = 0.0f;
	g_ball.spd_y = 0.0f;

	if (over_flag == false)
	{
		PlaySoundMem(g_over_handle, DX_PLAYTYPE_BACK);
		over_flag = true;
	}

	//Enter�L�[�����ꂽ��
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
	DrawGraph(0, 0, g_bg_handle, TRUE);	//�w�i
	//�u���b�N
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		if (g_block[i].state != NONE)
		{
			DrawRectGraph(g_block[i].pos_x, g_block[i].pos_y, g_block[i].grp_x, g_block[i].grp_y, g_block[i].grp_w, g_block[i].grp_h, g_block[i].handle, TRUE, FALSE);	//�u���b�N
		}
	}
	DrawRectGraph(g_player.pos_x, g_player.pos_y, g_player.grp_x, g_player.grp_y, g_player.grp_w, g_player.grp_h, g_player.handle, TRUE, FALSE);	//�v���C���[
//	DrawGraph(g_ball.pos_x, g_ball.pos_y, g_ball.handle, TRUE);	//�{�[��

	DrawGraph(4, (13 * CHIP_SIZE) + 2, g_info_handle, TRUE);			//�C���t�H

	/*�Q�[���I�[�o�[���o*/
	DrawFormatString(30, (13 * CHIP_SIZE) + 18, GetColor(255, 255, 255), "���̎q�͓ޗ��̂����֗����Ă������E�E�E\nEnter�L�[�Ń^�C�g���ɂ��ǂ�܂�");
}