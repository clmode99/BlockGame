//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/
//! @file   GameMain.cpp
//!
//! @brief  �Q�[���֘A�̃\�[�X�t�@�C��
//!
//! @date   2016.06.17
//!
//! @author Miyu Hara
//__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/__/

/*�ꎞ�I�R�����g
�E�����蔻��̐��x���悭������(�������ĂȂ��̂ɔ��˂������������)
�E�A�C�e���擾�ł���悤�ɂ���(����̃u���b�N�󂷂Ǝg����悤�ɂ���)
�E
�E�P��TRUE�ɂ���(�ǐ�)
*/

// �w�b�_�t�@�C���̓ǂݍ��� ================================================
#include "GameMain.h"

//�L���萔
#define BLOCK_WIDTH (8)		//�u���b�N�̐��̕�
#define BLOCK_HEIGHT (4)	//�u���b�N�̐��̍���
#define MAX_BLOCK (BLOCK_WIDTH*BLOCK_HEIGHT)		//�u���b�N�̐�
#define BLOCK_START_X (5)	//�u���b�N�̃X�^�[�g�ʒu
#define BLOCK_START_Y (40)	//�u���b�N�̃X�^�[�g�ʒu
#define PADDLE_SPD_X (8.0f)

//�\���̐錾
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
	int state;		//�`��t���O(0=�\�����Ȃ��A1=�ӂ��A2=�������A3=�A�C�e��)
}OBJECT;

//�񋓑̐錾
enum SCENE
{
	LOGO,	//���S���
	TITLE,	//�^�C�g�����
	PLAY,	//�v���C���
	CLEAR,	//�N���A���
	OVER	//�Q�[���I�[�o�[���
};

// �O���[�o���ϐ��̒�` ====================================================
OBJECT g_block[MAX_BLOCK];	//�u���b�N
OBJECT g_ball;				//�{�[��
OBJECT g_paddle;			//�p�h��
HGRP g_PongImage;			//�O���t�B�b�N�n���h��(PongImage.png)
HGRP g_Logo_handle;			//�O���t�B�b�N�n���h��(���S)
HGRP g_title_handle;		//�O���t�B�b�N�n���h��(�^�C�g��)
int g_key_code = 0;			//�L�[���
int g_key_old = 0;			//�P�t���[���O�̃L�[���
int g_key_enter = 0;		//Enter�L�[��������Ă邩(GetJoypad�`����Enter�L�[�����Ȃ�����)
int g_key_enter_old = 0;	//g_key_old��Enter�L�[�΁[�����
int g_block_cnt = 0;		//�u���b�N�J�E���^
int g_scene = PLAY;			//�V�[���Ǘ�
bool g_init = false;		//�������t���O
int g_nextScene = g_scene;	//���̃V�[��
int g_cnt = 0;				//�J�E���^
bool start_flag = false;	//�X�^�[�g�̔��˃t���O(false:�X�^�[�g�O�Atrue:���˂���)
int g_block_soft_handle = 0;	//�ʏ��Ԃ̃u���b�N
int g_block_hard_handle = 0;	//�����߂̃u���b�N
int g_block_item_handle = 0;	//�O���t�B�b�N�n���h��(�u���b�N�̃A�C�e��)
int g_block_type = 0;	//�u���b�N�^�C�v(0:�\�t�g�A1:�n�[�h�A2:�A�C�e��)
int g_item = 0;
bool g_item_flag = false;

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
	g_PongImage = LoadGraph("Resources\\images\\PongImage.png");	//PongImage.png�̃O���t�B�b�N�n���h��
	g_Logo_handle = LoadGraph("Resources\\images\\my_logo.png");	//���S�̃O���t�B�b�N�n���h��
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

	//�u���b�N������
	for (int i = 0; i < BLOCK_HEIGHT; i++)			//����
	{
		for (int j = 0; j < BLOCK_WIDTH; j++)		//��
		{
			//�ӂ[(16��)
			if ((i * BLOCK_WIDTH) + j < 16)
			{
				g_block[(i * BLOCK_WIDTH) + j].handle = g_block_soft_handle;	//�O���t�B�b�N�n���h��
				g_block[(i * BLOCK_WIDTH) + j].state = 1;
			}
			//��������[��(12��)
			else if ((i * BLOCK_WIDTH) + j < 28)
			{
				g_block[(i * BLOCK_WIDTH) + j].handle = g_block_hard_handle;	//�O���t�B�b�N�n���h��
				g_block[(i * BLOCK_WIDTH) + j].state = 2;
			}
			//�A�C�e���Ȃ�[��(4��)
			else if ((i * BLOCK_WIDTH) + j < 32)
			{
				g_block[(i * BLOCK_WIDTH) + j].handle = g_block_item_handle;	//�O���t�B�b�N�n���h��
				g_block[(i * BLOCK_WIDTH) + j].state = 3;
			}

			g_block[(i * BLOCK_WIDTH) + j].grp_x = 0;						//�摜X
			g_block[(i * BLOCK_WIDTH) + j].grp_y = 0;						//�摜Y
			g_block[(i * BLOCK_WIDTH) + j].grp_w = 64;						//��
			g_block[(i * BLOCK_WIDTH) + j].grp_h = 32;						//����
			g_block[(i * BLOCK_WIDTH) + j].pos_x = j*(g_block[i].grp_w) + BLOCK_START_X ;		//���WX
			g_block[(i * BLOCK_WIDTH) + j].pos_y = i*(g_block[i].grp_h) + BLOCK_START_Y;		//���WY
			g_block[(i * BLOCK_WIDTH) + j].spd_x = 1.0f;						//���xX
			g_block[(i * BLOCK_WIDTH) + j].spd_y = 0.0f;						//���xY
		}
	}

	static int temp_n1 = 0;
	static int temp_n2 = 0;
	static int temp_state = 0;
	static int temp_handle = 0;

	//�u���b�N�V���b�t��
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
	


	//�p�h��������
	{
		g_paddle.handle = LoadGraph("Resources\\images\\paddle.png");	//�O���t�B�b�N�n���h��
		g_paddle.grp_x = 0;				//�摜X
		g_paddle.grp_y = 0;				//�摜Y
		g_paddle.grp_w = 64;			//��
		g_paddle.grp_h = 16;			//����
		g_paddle.pos_x = 200.0f;		//���WX
		g_paddle.pos_y = 400.0f;		//���WY
		g_paddle.spd_x = 0.0f;			//���xX
		g_paddle.spd_y = 0.0f;			//���xY
	}

	//�{�[��������
	{
		g_ball.handle = LoadGraph("Resources\\images\\ball_4.png");			//�O���t�B�b�N�n���h��
		g_ball.grp_x = 0;				//�摜X
		g_ball.grp_y = 0;				//�摜Y
		g_ball.grp_w = 16;				//��
		g_ball.grp_h = 16;				//����
		g_ball.pos_x = g_paddle.pos_x + 20.0f;			//���WX
		g_ball.pos_y = g_paddle.pos_y-g_ball.grp_h;			//���WY
		g_ball.spd_x = 0.0f;			//���xX
		g_ball.spd_y = 0.0f;			//���xY
	}

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

	/*�~�̔���*/
	//float x1 = o1.pos_x + (o1.grp_w / 2.0f), y1 = o1.pos_y + (o1.grp_h / 2.0f);	//o1�̒��S���W
	//float x2 = o2.pos_x + (o2.grp_w / 2.0f), y2 = o2.pos_y + (o2.grp_h / 2.0f);	//o2�̒��S���W

	//float r1 = o1.grp_w / 2.0f;	//o1�̔��a
	//float r2 = o2.grp_w / 2.0f;	//o2�̔��a

	////����
	//if (((x1 - x2)*(x1 - x2)) + ((y1 - y2)*(y1 - y2)) <= ((r1 + r2)*(r1 + r2)))
	//{
	//	//�������Ă�
	//	return true;
	//}
	//else
	//{
	//	//�������ĂȂ�
	//	return false;
	//}

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
	/*���[�J���ϐ�*/
	static int color = 0, color_b = 0;

	//�t�F�[�h�C�����Ĉ�莞�Ԍo������t�F�[�h�A�E�g

	//������
	if (g_init == false)
	{
		g_cnt = 0;
		g_init = true;
	}
	g_cnt++;

	/*�Ȃ񂩃\�[�X����Ɩ��O�Ƃ������������ǁA�C�ɂ��ȁ[��*/
	/*���X�ɖ��邭(�Q�b)*/
	if ((color >= 0) && (color < 60))
	{
		SetDrawBright((int)color*4.25, (int)color*4.25, (int)color*4.25);
	}
	/*�ێ�(�Q�b)*/
	if ((color >= 60) && (color < 180))
	{
		SetDrawBright(255, 255, 255);
	}
	/*���X�ɈÂ�(�Q�b)*/
	if ((color >= 180) && (color <= 240))
	{
		SetDrawBright((int)255 - (color_b*4.25), (int)255 - (color_b*4.25), (int)255 - (color_b*4.25));
		color_b++;
	}

	//�Q�b��������^�C�g����
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
		g_nextScene = PLAY;
	}
}

void RenderTitle(void)
{
	DrawGraph(0, 0, g_title_handle, TRUE);
	//DrawFormatString(0, 0, GetColor(255, 255, 255), "�^�C�g�������[\nEnter�L�[�ł͂��܂邨\nEsc�L�[�ŏI��邨\n");
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
	if (g_block_cnt >= MAX_BLOCK)
	{
		g_nextScene = CLEAR;
	}

	/*�Q�[���I�[�o�[*/
	if ((g_ball.pos_y + g_ball.grp_h) >= SCREEN_HEIGHT)
	{
		g_nextScene = OVER;
	}

	/*�{�[���̉^��*/
	if (start_flag == false)
	{
		g_ball.spd_x = 0.0f;
		g_ball.spd_y = 0.0f;

		//�p�h���ɂ������悤�ɓ���
		if (g_key_code & PAD_INPUT_LEFT)	//���L�[�������ꂽ��
		{
			if (g_paddle.pos_x > 0.0f)
			{
				g_ball.spd_x = -PADDLE_SPD_X;
			}
		}
		if (g_key_code & PAD_INPUT_RIGHT)	//�E�L�[�������ꂽ��
		{
			if ((g_paddle.pos_x + g_paddle.grp_w) < SCREEN_WIDTH)
			{
				g_ball.spd_x = PADDLE_SPD_X;
			}
		}

		//�X�y�[�X�����ꂽ�甭��
		if (g_key_code&PAD_INPUT_10)
		{
			g_ball.spd_x = 3.0f;
			g_ball.spd_y = 3.0f;
			start_flag = true;
		}
	}
	g_ball.pos_x += g_ball.spd_x;
	g_ball.pos_y += g_ball.spd_y;


	/*�p�h���̉^��*/
	g_paddle.pos_x += g_paddle.spd_x;

	/*�p�h���̈ړ�*/
	g_paddle.spd_x = 0.0f;
	if (g_key_code & PAD_INPUT_LEFT)	//���L�[�������ꂽ��
	{
		if (g_paddle.pos_x > 0.0f)
		{
			g_paddle.spd_x = -PADDLE_SPD_X;
		}
	}
	if (g_key_code & PAD_INPUT_RIGHT)	//�E�L�[�������ꂽ��
	{
		if ((g_paddle.pos_x + g_paddle.grp_w) < SCREEN_WIDTH)
		{
			g_paddle.spd_x = PADDLE_SPD_X;
		}
	}
	
	/*�u���b�N�̉^��*/
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

	/*�{�[���ƕǂ̔���*/
	if ((g_ball.pos_x <= 0.0f) || (g_ball.pos_x >= 640.0f - g_ball.grp_w))	//���E�̒[�ɂ�����
	{
		g_ball.spd_x *= -1.0f;
	}
	if (g_ball.pos_y <= 0.0f)	//��̒[�ɂ�����
	{
		g_ball.spd_y *= -1.0f;
	}

	/*�{�[���ƃp�h���̓����蔻��*/
	if (start_flag == true)
	{
		if ((g_ball.pos_x <= (g_paddle.pos_x + g_paddle.grp_w)) &&
			((g_ball.pos_x + g_ball.grp_w) >= g_paddle.pos_x) &&
			(g_ball.pos_y <= (g_paddle.pos_y + g_paddle.grp_h)) &&
			((g_ball.pos_y + g_ball.grp_h) >= g_paddle.pos_y))
		{
			/*�{�[�������ɓ����Ă�Ƃ��̂ݔ��]����*/
			if (g_ball.spd_y > 0.0f)
			{

				///*�{�[���̈ʒu�Ŋp�x��ς���*/
				////�E�[�œ���������
				//if ((g_ball.pos_x >= (g_paddle.pos_x + (g_paddle.grp_w - 3.0f))) && (g_ball.pos_x <= (g_paddle.pos_x + g_paddle.grp_w)))
				//{
				//	g_ball.spd_x = 5.0f;
				//}
				////�E���œ���������
				//if ((g_ball.pos_x >= g_paddle.pos_x + 46.0f) && (g_ball.pos_x <= (g_paddle.pos_x + (g_paddle.grp_w - 3.0f))))
				//{
				//	g_ball.spd_x = 3.0f;
				//}
				////�����E���œ���������
				//if ((g_ball.pos_x >= g_paddle.pos_x + 32.0f) && (g_ball.pos_x <= (g_paddle.pos_x + 46.0f)))
				//{
				//	g_ball.spd_x = 2.0f;
				//}
				////���������œ���������
				//if ((g_ball.pos_x >= g_paddle.pos_x + 18.0f) && (g_ball.pos_x <= (g_paddle.pos_x + 32.0f)))
				//{
				//	g_ball.spd_x = -2.0f;
				//}
				////�����œ���������
				//if ((g_ball.pos_x >= g_paddle.pos_x + 3.0f) && (g_ball.pos_x <= (g_paddle.pos_x + 18.0f)))
				//{
				//	g_ball.spd_x = -3.0f;
				//}
				////���[�œ���������
				//if ((g_ball.pos_x >= g_paddle.pos_x) && (g_ball.pos_x <= (g_paddle.pos_x + 3.0f)))
				//{
				//	g_ball.spd_x = -5.0f;
				//}

				//���]
				//g_ball.spd_x *= -1.0f;
				g_ball.spd_y *= -1.0f;

			}

			/*�p�h���̑��ʂ̔���*/
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

	/*�{�[���ƃu���b�N�̓����蔻��*/
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		//����������u���b�N����
		if ((Collision(g_ball, g_block[i])) && (g_block[i].state !=0))
		{
			//�A�C�e���u���b�N�͉󂵂��瑦���悤�Ȃ�[
			if (g_block[i].state == 3)
			{
				if (g_item == 0)
				{
					g_item_flag = true;
				}
				g_item++;
				g_block[i].state = 0;
			}
			//��������͂��炩������
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
	//�u���b�N
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		if (g_block[i].state > 0)
		{
			DrawRectGraph(g_block[i].pos_x, g_block[i].pos_y, g_block[i].grp_x, g_block[i].grp_y, g_block[i].grp_w, g_block[i].grp_h, g_block[i].handle, TRUE, FALSE);	//�u���b�N
		}
	}
	//�p�h��
	DrawRectGraph(g_paddle.pos_x, g_paddle.pos_y, g_paddle.grp_x, g_paddle.grp_y, g_paddle.grp_w, g_paddle.grp_h, g_paddle.handle, TRUE, FALSE);	//�p�h��
	DrawGraph(g_ball.pos_x, g_ball.pos_y, g_ball.handle, TRUE);					//�{�[��

	//�X�y�[�X�L�[�Ŕ��˂̂��m�点
	if (start_flag == false)
	{
		DrawFormatString(200, 250, GetColor(255, 255, 255), "�X�y�[�X�L�[�ŃX�^�[�g�I\n");
	}

	if (g_item_flag == true)
	{
		DrawFormatString(0, 0, GetColor(255, 255, 255), "Item:%d", g_item);
	}

	//�f�o�b�O���
	/*for (int i = 0; i < MAX_BLOCK; i++)
	{
		DrawFormatString(0, i * 20, GetColor(255, 255, 255), "g_block[%d].state=%d\n", i, g_block[i].state);
	}*/
//	DrawFormatString(0, 0, GetColor(255, 255, 255), "g_block[0].pos_x=%f\n",g_block[0].pos_x);
//	DrawFormatString(0, 20, GetColor(255, 255, 255), "g_ball_cnt=%d\n", g_block_cnt);
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

	/*�Ƃ肠�����{�[���~�߂�*/
	g_ball.spd_x = 0.0f;
	g_ball.spd_y = 0.0f;

	//Enter�L�[�����ꂽ��
	if ((g_key_enter == 1) && (!(g_key_enter_old == 1)))
	{
		g_nextScene = TITLE;
	}
}

void RenderClear(void)
{
	//�p�h��
	DrawRectGraph(g_paddle.pos_x, g_paddle.pos_y, g_paddle.grp_x, g_paddle.grp_y, g_paddle.grp_w, g_paddle.grp_h, g_paddle.handle, TRUE, FALSE);	//�p�h��
	DrawGraph(g_ball.pos_x, g_ball.pos_y, g_ball.handle, TRUE);					//�{�[��

	/*�N���A���o*/
	DrawFormatString(0, 200, GetColor(255, 255, 255), "���肠�[�I\nEnter�L�[�Ń^�C�g���ɂ��ǂ邨\n");
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

	/*�Ƃ肠�����{�[���~�߂�*/
	g_ball.spd_x = 0.0f;
	g_ball.spd_y = 0.0f;

	//Enter�L�[�����ꂽ��
	if ((g_key_enter == 1) && (!(g_key_enter_old == 1)))
	{
		g_nextScene = TITLE;
	}
}

void RenderOver(void)
{
	//�u���b�N
	for (int i = 0; i < MAX_BLOCK; i++)
	{
		if (g_block[i].state != 0)
		{
			DrawRectGraph(g_block[i].pos_x, g_block[i].pos_y, g_block[i].grp_x, g_block[i].grp_y, g_block[i].grp_w, g_block[i].grp_h, g_block[i].handle, TRUE, FALSE);	//�u���b�N
		}
	}
	//�p�h��
	DrawRectGraph(g_paddle.pos_x, g_paddle.pos_y, g_paddle.grp_x, g_paddle.grp_y, g_paddle.grp_w, g_paddle.grp_h, g_paddle.handle, TRUE, FALSE);	//�p�h��
	DrawGraph(g_ball.pos_x, g_ball.pos_y, g_ball.handle, TRUE);					//�{�[��

	/*�N���A���o*/
	DrawFormatString(0, 200, GetColor(255, 255, 255), "���[�ނ��[�΁[\nEnter�L�[�Ń^�C�g���ɂ��ǂ邨\n");
}