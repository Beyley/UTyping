#include "DxLib.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <vector>
#include <deque>
#include <bitset>
#include <list>

using namespace std;

/* ============================================================ */

/* �G���Ȋ֐� */
#include "ututil.h"

/* ============================================================ */

#include "utuid.h"

/* ============================================================ */

#include "utchallenge.h"

/* ============================================================ */

#define COLOR_EXCELLENT GetColor(255, 255, 0)
#define COLOR_GOOD GetColor(0, 255, 0)
#define COLOR_FAIR GetColor(0, 128, 255)
#define COLOR_POOR GetColor(128, 128, 128)

#define COLOR_EXCELLENT2 GetColor(255, 255, 128)
#define COLOR_GOOD2 GetColor(128, 255, 128)
#define COLOR_FAIR2 GetColor(128, 192, 255)
#define COLOR_POOR2 GetColor(192, 192, 192)

#define COLOR_TIME GetColor(64, 192, 255)
#define COLOR_TIME2 GetColor(192, 192, 192)

#define COLOR_RED_ZONE GetColor(255, 32, 0)
#define COLOR_YELLOW_ZONE GetColor(224, 255, 0)
#define COLOR_BLUE_ZONE GetColor(0, 64, 255)
#define COLOR_CLEAR_ZONE GetColor(255, 255, 255)

#define SEC_EXCELLENT 0.02
#define SEC_GOOD 0.05
#define SEC_FAIR 0.10
#define SEC_POOR 0.20

#define SCORE_EXCELLENT 1500
#define SCORE_GOOD 1000
#define SCORE_FAIR 500
#define SCORE_POOR 0

#define SCORE_COMBO 10
#define SCORE_COMBO_MAX 1000

#define SCORE_TYPING 500
/* �^�C�s���O��1�����m�肳���邲�ƂɁi1�o�C�g�����A2�o�C�g��������Ȃ��j */

#define GAUGE_COUNT 100
#define GAUGE_RED_ZONE 15
#define GAUGE_YELLOW_ZONE 45
#define GAUGE_CLEAR_ZONE 75
/*
failed:0
red   :(0,RED]
yellow:(RED,YELLOW]
blue  :(YELLOW,CLEAR]
clear :(CLEAR,COUNT]
*/

#define NAME_LEN 16
#define TYPE_BUFFER_LEN NAME_LEN
/* �^�C�s���O�ɗp����o�b�t�@�̒��� */
/* ���O�̍ő咷�ɓ������A�ϊ��̂��߂ɍŒ�4�����͕K�v */

#define H_COMMENT 240
/* �y�R�����g�̕��i�����j��RANKING_LEN �ʕ���`�悷��Ƃ��Ɠ����z������ */

#define H_MUSIC_INFO_SUB (6+H_COMMENT+4+H_PLAY_DATA+6)

#define MUSIC_INFO_HEIGHT 60
/* �ȏ��̕��i�����j */

/* ============================================================ */

/* �`��ʒu�Ȃ� */

#define CIRCLE_SPEED 250.0
/* �����~�̕W���̑����i�h�b�g/�b�j */

#define X_SCORE (W_WINDOW - 25)
#define Y_SCORE 70

#define X_STAT_GAUGE (W_WINDOW - 10)
#define W_STAT_GAUGE 400
#define Y_STAT_GAUGE 10
#define H_STAT_GAUGE 40

#define X_GAUGE 90
//#define W_GAUGE 500
#define W_GAUGE_SEGMENT 5
#define W_GAUGE (W_GAUGE_SEGMENT * GAUGE_COUNT)
#define Y_GAUGE 455
#define H_GAUGE 10
#define W_GAUGE_PADDING 4
#define H_GAUGE_PADDING 2

#define X_TIME X_GAUGE
#define W_TIME W_GAUGE
#define Y_TIME 445
#define H_TIME 4

#define X_INFO 160
#define Y_INFO 10
//#define Y_INFO2 40

#define X_CIRCLE 100
#define Y_CIRCLE 180
#define R_CIRCLE 30

#define Y0_BAR (Y_CIRCLE - 60)
#define Y1_BAR (Y_CIRCLE + 50)
#define Y0_BEAT (Y_CIRCLE - 45)
#define Y1_BEAT (Y_CIRCLE + 40)

#define X_ACCURACY (X_CIRCLE - R_CIRCLE)
/* �~�̍��[ */
#define Y_ACCURACY 90

#define Y_LYRICS (Y_CIRCLE + R_CIRCLE + 30)

#define X_LYRICS_KANJI 100
/* ����̍���"Next: ", �E�ɉ̎������� */
#define Y_LYRICS_KANJI (Y_LYRICS + 35)
#define Y_LYRICS_KANJI_NEXT (Y_LYRICS + 70)

#define X_LYRICS_BIG (X_CIRCLE - R_CIRCLE)
#define Y_LYRICS_BIG 350

#define X_BUFFER (X_CIRCLE - R_CIRCLE)
#define Y_BUFFER 390

#define X_HIDDEN (X_CIRCLE + R_CIRCLE + 60)
/* Hidden�Ō����鍶�̌��E */
#define X_SUDDEN (X_HIDDEN + R_CIRCLE)
/* Sudden�Ō�����E�̌��E */
/* �����������Ƃ��ɁA���傤�ǔ��~���炢������ */

#define SCALE_FUNCTION 60.0
/* ����ʒu�̉~�����_�Ƃ�����W�Ǝv���Ƃ��́A����1�ɑ��������ʂ̒��� */

#define SEC_FADEOUT 1.2
/* �I����Ă���X�R�A�\����ʂɈڍs����O�Ƀt�F�[�h�A�E�g���鎞�� */

/* ============================================================ */

#define W_WINDOW 640
#define H_WINDOW 480

/* ============================================================ */

#define INFTY 1000000000

enum{
ID_EXCELLENT,
ID_GOOD,
ID_FAIR,
ID_POOR,
ID_PASS,
};

enum{
ID_FAILED_ZONE,
ID_RED_ZONE,
ID_YELLOW_ZONE,
ID_BLUE_ZONE,
ID_CLEAR_ZONE,
};

enum{
PHASE_READY,	/* �J�n�O */
PHASE_MAIN,	/* ���C�� */
PHASE_FADEOUT,	/* MAIN�I����t�F�[�h�A�E�g */
PHASE_RESULT,	/* �X�R�A�\�� */
PHASE_FINISHED,	/* ���O���͌�i�n�C�X�R�A���������j */
PHASE_EXIT,	/* �I������̂ŏ����Ă��炤 */
};

enum{
BEATLINE_BAR,
BEATLINE_BEAT,
};

/* ============================================================ */
/* �O���[�o���ϐ��Ƃ� */

int g_fontHandleDebug;

/* ============================================================ */

double myGetNowHiPerformanceCount(){
	static LARGE_INTEGER Frequency = {0, 0};
	static LARGE_INTEGER PerformanceCount0;	/* �ŏ��ɌĂяo�������̒l��ۑ� */
	LARGE_INTEGER PerformanceCount;
	if(Frequency.QuadPart == 0){
		if(!QueryPerformanceFrequency(&Frequency)){
			throw "������\�p�t�H�[�}���X�J�E���^���擾�ł��܂���B";
		}
		QueryPerformanceCounter(&PerformanceCount0);
	}
	QueryPerformanceCounter(&PerformanceCount);
	return (double)(PerformanceCount.QuadPart - PerformanceCount0.QuadPart) 
		/ Frequency.QuadPart;
}

/* ============================================================ */

#include "utcheck.h"

/* ============================================================ */

#include "utconfig.h"
/* Config */

/* ============================================================ */

static bool g_isUsingDxLib = false;

int myDxLib_Init(){
	int ret;
	if(!g_isUsingDxLib){
		ret = DxLib_Init();
		g_isUsingDxLib = true;
	}else{
		ret = 0;
	}
	
	/* �_�u���o�b�t�@��p���� */
	SetDrawScreen(DX_SCREEN_BACK);
	ClearDrawScreen();
	
	g_check.begin(GetColor(16, 16, 16));
	return ret;
}


int myDxLib_End(){
	int ret;
	if(g_isUsingDxLib){
		ret = DxLib_End();
		g_isUsingDxLib = false;
	}else{
		ret = 0;
	}
	
	g_check.end();
	return ret;
}

int myScreenFlip(){
	static int count = 0;
	static double fps = -1.0;
	static double timeCountBegin;
	if(g_config.f_showFPS){
		if(count == 0){
			timeCountBegin = myGetNowHiPerformanceCount();
		}else if(count == 30){
			double timeCountEnd = myGetNowHiPerformanceCount();
			fps = 30.0 / (timeCountEnd - timeCountBegin);
			/* x[us] �̂Ƃ� 30/(x/1000000) = 30000000/x */
			/* �Ă��Ax[s]�̂Ƃ�30/x */
			count = 0;
			timeCountBegin = timeCountEnd;
		}
		count++;
		if(fps >= 0.0){
			char buf[256];
			sprintf(buf, "[%4.1ffps]", fps);
			int width = GetDrawStringWidthToHandle(buf, strlen(buf), g_fontHandleDebug);
			DrawStringToHandle(W_WINDOW - width, 0, buf, GetColor(255, 255, 255), g_fontHandleDebug);
			/* �E���fps�\�� */
		}
	}
	if(g_config.f_debugMode){	/* �f�o�b�O���[�h�Ȃ獶��ɏ����Ă��� */
		DrawStringToHandle(0, 0, "Debug Mode", GetColor(255, 0, 0), g_fontHandleDebug);
	}
	int ret0, ret1;
	g_check.rap(GetColor(16, 16, 16));
	ret0 = ScreenFlip();
	g_check.end();
	g_check.begin(GetColor(32, 32, 32));
	ret1 = ClearDrawScreen();
	g_check.rap(GetColor(16, 16, 16));
	if(g_config.f_showProcessTime){
		g_check.draw();
	}
	return (ret0 == -1 || ret1 == -1) ? -1 : 0;
}

/* ============================================================ */

/* UTypingUserID�𒲂ׂ� */

bool checkUTypingUserID(){
	FILE *fp;
	fp = fopen("UTyping_ID.txt", "r");
	if(!fp){
		throw "UTyping_ID.txt ���J���܂���B";
	}
	int ID[4];
	getUTypingUserID(ID);
	for(int i=0; i<4; i++){
		int tmp;
		fscanf(fp, "%X", &tmp);
		if(tmp != ID[i]){
			return false;
		}
	}
	fclose(fp);
	return true;
}

/* ============================================================ */

#include "utkeyinput.h"
/*
char GetKeyboardInput(){
	return GetInputChar(TRUE);
}

char GetKeyboardInput(double &timeCount){
	return GetInputChar(TRUE);
}
//*/

/* config���Q�Ƃ��ĕ��� */
/*
char GetKeyboardInput(){
	if(g_config.f_useMultiThread){
		return MT_GetKeyboardInput();
	}else{
		return ST_GetKeyboardInput();
	}
}

char GetKeyboardInput(double &timeCount){
	if(g_config.f_useMultiThread){
		return MT_GetKeyboardInput(timeCount);
	}else{
		return ST_GetKeyboardInput(timeCount);
	}
}
*/

/* ============================================================ */

#include "utscore.h"
/* score,ranking */

/* ============================================================ */

#define CEFFECT1_SEC_CLEAR 0.6
#define CEFFECT1_FONT_SIZE 40

class CEffect1{
public:
	CEffect1();
	~CEffect1();
	void clear();
	void insert(int x, int y, char *str, int color, double time);
	void insert(int x, int y, char ch, int color, double time);
	void draw(double time);
private:
	int m_fontHandle;
	struct Data{
		int x, y;
		char str[20];
		int color;
		double time;
		int strWidth;
	};
	deque<Data> m_deq;
};

CEffect1::CEffect1(){
	m_fontHandle = CreateFontToHandle("�l�r ����", CEFFECT1_FONT_SIZE, 2, DX_FONTTYPE_ANTIALIASING);
}

CEffect1::~CEffect1(){
	DeleteFontToHandle(m_fontHandle);
}

void CEffect1::clear(){
	m_deq.clear();
}

void CEffect1::insert(int x, int y, char *str, int color, double time){
	CEffect1::Data data;
	data.x = x;
	data.y = y;
	strcpy(data.str, str);
	data.color = color;
	data.time = time;
	data.strWidth = GetDrawStringWidthToHandle(data.str, strlen(data.str), m_fontHandle);
	m_deq.push_back(data);
}

void CEffect1::insert(int x, int y, char ch, int color, double time){
	/* char1�����̏ꍇ�𕶎���̏ꍇ�ɋA�� */
	char buf[2];
	buf[0] = ch;
	buf[1] = '\0';
	insert(x, y, buf, color, time);
}

void CEffect1::draw(double time){
	while(!m_deq.empty() &&
			time - m_deq.front().time >= CEFFECT1_SEC_CLEAR){
		m_deq.pop_front();
	}
	for(deque<CEffect1::Data>::iterator itr = m_deq.begin(); itr != m_deq.end(); itr++){
		double timeDiff = time - (*itr).time;
		double ExRate;	/* �����̊g�嗦 */
		{
			double tmp = 1.0 - (timeDiff / CEFFECT1_SEC_CLEAR);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * (tmp * tmp * tmp));
			/* ���Ԃ����ɂꓧ���� */
		}
		{	/* ���Ԃ��o�ɂ�g�� */
			double tmp = timeDiff / CEFFECT1_SEC_CLEAR;
			ExRate = 1.0 + 4.0 * tmp * tmp;
		}
		DrawExtendStringToHandle(
			(*itr).x - ((*itr).strWidth * ExRate / 2), (*itr).y - (CEFFECT1_FONT_SIZE * ExRate / 2),
			ExRate, ExRate, (*itr).str, (*itr).color, m_fontHandle);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);	/* �u�����h���Ȃ��悤�ɖ߂� */
}

/* ============================================================ */

/* ���ɕ�����1�������ǉ�����A�O�������Ă����������Ώۂɂ���Effect */
/* �ꉞ�A�����񂪒����𑝂₳���ω�����\���ɂ��Ή� */

#define CEFFECTSTR_SEC 0.3
/* �폜�������܂ł̎��� */

#define CEFFECTSTR_SEC2 0.08
/* �ł����G�t�F�N�g�������鎞�� */

#define CEFFECTSTR_H 10
/* �ł��؂�����ɗ����鋗�� */

//#define CEFFECTSTR_V 15.0
/* 1�b�ō��������x��15.0������ */
/* �����init��posV�Őݒ�ł���悤�ɂȂ��� */

#define CEFFECTSTR_EX 2.4
/* �G�t�F�N�g�̕����̑傫���̍ŏ� */

class CEffectStr{
public:
	void init(int fontHandle, bool flagTypeEffect, double posV, double alphaCleared);
		/* �g�p����t�H���g�ƁA�^�C�s���O�����悤�ȃG�t�F�N�g�������邩���ŏ��ɐݒ� */
		/* �\���ł����������l�߂鑬�x�i����/�b�j�A�폜����̕s�����x���ݒ� */
	void push(char ch, double time);	/* �����Ɉꕶ���̃f�[�^��ǉ� */
	void push(char *str, double time);	/* �����ɐ������̃f�[�^��ǉ� */
	void update(const char *str, int len, double time);
		/* ���݂̃f�[�^��ݒ�B��ɑO�������������Ƃ������� */
		/* �����ŁA�f�[�^�̌����str����v���Ȃ������璼�� */
	void clear();
		/* ���݂̃f�[�^��P���Ɂi�����G�t�F�N�g�Ȃ��Łj�����B�����̃G�t�F�N�g�͎c���B */
	
	void draw(int X, int Y, double time);
		/* ���݂̕�������Q�Ƃ��\�� */
private:
	void drawChar(double X, double Y, const char *buf, double t, double alpha) const;
		/* 2�o�C�g�����̊֌W�㕶����œn�� */
	void updatePos(double time);
private:
	int m_fontHandle;
	bool m_flagTypeEffect;
	int m_fontWidth;
	
	double m_posV;
	double m_alphaCleared;
	
	struct Data{
		char ch;
		double time;	/* �ǉ����ꂽ���� */
		double timeCleared;	/* �폜���ꂽ���� */
		double x;	/* �폜�ς݂̕������ĈӖ��������A�\������ʒu */
		//bool isCleared;
	};
	deque<Data> m_deq;	/* ���݂̃f�[�^�̕������� */
	deque<Data> m_deqCleared;	/* �폜���ꂽ�������� */
	
	double m_pos;	/* �\�������ʒu���ǂꂾ�����ɂ���邩 */
	double m_lastTime;	/* �Ō�ɍX�V�������� */
};

void CEffectStr::init(int fontHandle, bool flagTypeEffect, double posV, double alphaCleared){
	m_fontHandle = fontHandle;
	{
		char str[2] = " ";
		m_fontWidth = GetDrawStringWidthToHandle(str, 1, m_fontHandle);
	}
	
	m_flagTypeEffect = flagTypeEffect;
	m_posV = posV * m_fontWidth;
	m_alphaCleared = alphaCleared;
	
	m_deq.clear();
	m_deqCleared.clear();
	
	m_pos = 0.0;
	//m_lastTime = 0.0;
}

void CEffectStr::push(char ch, double time){
	Data data;
	data.ch = ch;
#if 0
	if(m_deq.empty() || m_deq.back().isCleared){	/* ���݂��Ă���f�[�^���Ȃ� */
		data.x = 0.0;
	}else{	/* �Ō�̃f�[�^�̌�� */
		data.x = m_deq.back().x + m_fontWidth;	// + 5.0;
	}
#endif
	data.time = time;
	//data.isCleared = false;
	m_deq.push_back(data);
}

void CEffectStr::push(char *str, double time){
	while(*str){
		push(*str, time);
		str++;
	}
}


void CEffectStr::update(const char *str, int len, double time){
	updatePos(time);
	
	int diff = m_deq.size() - len;
		/* �f�[�^�̂ǂ��܂ł��������� */
	if(diff < 0){
		throw __LINE__;
	}
	/* �������f�[�^���ړ� */
	for(int i=0; i<diff; i++){
		m_deq[i].timeCleared = time;
		m_deq[i].x = m_pos;
		m_pos += m_fontWidth;
		m_deqCleared.push_back(m_deq[i]);
//printfDx("%10c, %10d, %10f, %10f\n", m_deq[i].ch, (int)m_deq[i].x, m_deq[i].time, m_deq[i].timeCleared);
	}
	for(int i=0; i<diff; i++){
		m_deq.pop_front();
	}
	
	if(len == 0){	/* �f�[�^����ɂȂ�����A���[��0�ɂ��� */
		m_pos = 0.0;
	}
	
	for(int i=0; i<len; i++){
		m_deq[i].ch = str[i];	/* �O�̂��ߐV�����f�[�^�ŏ㏑�� */
	}
	
}

void CEffectStr::clear(){
	m_deq.clear();
}

void CEffectStr::draw(int X, int Y, double time){
	updatePos(time);
	
	while(!m_deqCleared.empty() &&
			time - m_deqCleared.front().timeCleared >= CEFFECTSTR_SEC){	/* �����Ă���\���������Ԃ������� */
		m_deqCleared.pop_front();
	}
	
	char buf[3];
	for(deque<Data>::iterator itr = m_deqCleared.begin(); itr != m_deqCleared.end(); itr++){
		double x = itr->x;
		buf[0] = itr->ch;
		if(isJapanese1st(itr->ch)){
			itr++;
			if(itr == m_deqCleared.end()){
				break;
			}
			buf[1] = itr->ch;
			buf[2] = '\0';
		}else{
			buf[1] = '\0';
		}
		double xTime = (time - itr->timeCleared)/CEFFECTSTR_SEC;
		double t = 1.0 - xTime;
		if(t > 1.0) t = 1.0;
		t *= t*t;
		drawChar(X + x, Y + CEFFECTSTR_H * xTime * xTime,
			buf, time - itr->time, m_alphaCleared * t);
	}
	
	double pos = m_pos;
	for(deque<Data>::iterator itr = m_deq.begin(); itr != m_deq.end(); itr++){
		double x = pos;
		buf[0] = itr->ch;
		if(isJapanese1st(itr->ch)){
			itr++;
			if(itr == m_deq.end()){
				break;
			}
			buf[1] = itr->ch;
			buf[2] = '\0';
			pos += 2*m_fontWidth;
		}else{
			buf[1] = '\0';
			pos += m_fontWidth;
		}
		drawChar(X + x, Y, buf, time - itr->time, 1.0);
	}
}

void CEffectStr::drawChar(double X, double Y, const char *buf, double t, double alpha) const{
	if(m_flagTypeEffect && t < CEFFECTSTR_SEC2){
		t = 1.0 - t/CEFFECTSTR_SEC2;
		if(t > 1.0) t = 1.0;
		double ExRate = 1.0 + (CEFFECTSTR_EX - 1.0) * t * t;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64 * alpha / ExRate);
		DrawExtendStringToHandle(
			X - m_fontWidth * (ExRate - 1.0) / 2.0 , Y - m_fontWidth * (ExRate - 1.0),
			ExRate, ExRate, buf, GetColor(255, 255, 255), m_fontHandle);
		/* �t�H���g�̏c:��=1:2�Ƃ���������g���Ă��� */
		/* ����ɁA�S�p���������Ȃ����Ƃ��K�v�����A�^�C�s���O�ł̓��[�}����������Ȃ��̂�OK */
	}
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * alpha);
	DrawStringToHandle(X, Y, buf, GetColor(255, 255, 255), m_fontHandle);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
/*
if(alpha!=1.0){
DrawStringToHandle(X, Y, buf, GetColor(255, 0, 0), m_fontHandle);
}
*/
}

void CEffectStr::updatePos(double time){
	if(m_pos == 0.0){
		m_lastTime = time;
		return;
	}
	double timeDiff = time - m_lastTime;
	if(timeDiff < 0.0){
		return;
	}
	m_lastTime = time;
	m_pos -= timeDiff * m_posV;
	if(m_pos < 0.0){
		m_pos = 0.0;
	}
}

/* ============================================================ */

/* �ǉ���̃Q�[�W */
class CStatGauge{
public:
	void init();
	void draw();
	void inc(int ID_accuracy);
private:
	void drawBar(double val, int y, int h, int color) const;
	void drawLight(double Light, int y, int h, int color) const;
private:
	int m_val[4];	/* �Q�[�W�̌��ݕ`����钷�� */
	double m_drawX[4];
	double m_drawV[4];	/* �Q�[�W�̌��ݕ`����钷���̓������x */
	double m_rate;	/* ���̒���1�ɑ��������ʏ�̒��� */
	double m_light[4];	/* �Q�[�W�̂��ꂼ����Ƃ炷���̋��� */
	
	int m_color[4][2];
};

void CStatGauge::init(){
	for(int i=0; i<4; i++){	/* �ŏ��̒l */
		m_val[i] = 0;
		m_drawX[i] = 0.0;
		m_drawV[i] = 0.0;
		m_light[i] = 0.0;
	}
	m_rate = 20.0;	/* �ŏ��́A����1������̕\�����钷�� */
	
	m_color[ID_EXCELLENT][0] = COLOR_EXCELLENT;
	m_color[ID_EXCELLENT][1] = COLOR_EXCELLENT2;
	m_color[ID_GOOD][0] = COLOR_GOOD;
	m_color[ID_GOOD][1] = COLOR_GOOD2;
	m_color[ID_FAIR][0] = COLOR_FAIR;
	m_color[ID_FAIR][1] = COLOR_FAIR2;
	m_color[ID_POOR][0] = COLOR_POOR;
	m_color[ID_POOR][1] = COLOR_POOR2;
}

void CStatGauge::draw(){	/* �Q�[�W��\�� */
	int y = Y_STAT_GAUGE;
	int h = H_STAT_GAUGE / 4;
	
	for(int i=0; i<4; i++){	/* �Q�[�W�̎w���ׂ��l�ɋ߂Â��� */
		//m_statGauge[i] += (x[i] - m_statGauge[i]) * 0.2;
		m_drawV[i] += 0.12 * (m_val[i] - m_drawX[i]);
		m_drawX[i] += m_drawV[i];
		/* ���C */
		m_drawV[i] *= 0.7;
	}
		
	{
		double rate = 20.0;
		while(1){
			bool flag = true;
			for(int i=0; i<4; i++){
				if(m_val[i] * rate > W_STAT_GAUGE){
					flag = false;
				}
			}
			if(flag){
				break;
			}
			/* �����邽�т�0.75�{�����Ă����A�����Ȃ��Ȃ�܂ł�� */
			rate *= 0.75;
		}
		m_rate += (rate - m_rate) * 0.2;
	}
	
	
	{
		double x[4];	/* m_drawX[]����k����������x[]�ɓ���� */
		for(int i=0; i<4; i++){
			x[i] = m_drawX[i] * m_rate;
		}
		/* ���Ƃ��ƔZ���D�F�ŕ`�悵�Ȃ��̂� tan �Ȃǂւ̔z�� */
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);	/* �s������1/2 */
		DrawBox(X_STAT_GAUGE - W_STAT_GAUGE, y, X_STAT_GAUGE, y + 4 * h, GetColor(64, 64, 64), TRUE);
		/* ���ꂼ��̒�����\�� */
		for(int i=0; i<4; i++){
			drawBar(x[i], y+i*h, h, m_color[i][0]);
		}
	}
	
	/* �ŋߕύX���ꂽ���̂��Ƃ炷 */
	for(int i=0; i<4; i++){
		drawLight(m_light[i], y+i*h, h, m_color[i][1]);
		/*
		double Light = m_light[i];
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(192 * Light));
		Light *= 4.0;
		if(Light > 1.0){
			Light = 1.0;
		}
		int dh = (int)(h * 0.5 * (1.0 - Light)*(1.0 - Light));
		DrawBox(X_STAT_GAUGE - W_STAT_GAUGE, y+i*h + dh, X_STAT_GAUGE, y+(i+1)*h - dh, m_color[i][1], TRUE);
		*/
		m_light[i] *= 0.85;
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void CStatGauge::inc(int ID_accuracy){
	m_val[ID_accuracy]++;
	m_light[ID_accuracy] = 1.0;
}

void CStatGauge::drawBar(double val, int y, int h, int color) const{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 224);	/* �s������7/8 */
	//DrawBox((int)(X_STAT_GAUGE - val + 0.5), y, X_STAT_GAUGE, y+h, color, TRUE);
	int i_val = floor(val);
	double f_val = val - i_val;
	DrawBox(X_STAT_GAUGE - i_val, y, X_STAT_GAUGE, y+h, color, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(224 * f_val));	/* �s������(7/8)*�������� */
	DrawBox(X_STAT_GAUGE - i_val - 1, y, X_STAT_GAUGE - i_val, y+h, color, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void CStatGauge::drawLight(double Light, int y, int h, int color) const{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(192 * Light));
	Light *= 4.0;
	if(Light > 1.0){
		Light = 1.0;
	}
	int dh = (int)(h * 0.5 * (1.0 - Light)*(1.0 - Light));
	DrawBox(X_STAT_GAUGE - W_STAT_GAUGE, y + dh, X_STAT_GAUGE, y + h - dh, color, TRUE);
}

/* ============================================================ */

struct ConvertData{
public:
	ConvertData(const char *buf1, const char *buf3);
	bool match_front(const char *str);
	bool match(const char *str);
	bool get_unprocessed(char *buf);
public:
	char m_str[8];	/* ���{�� �� ���[�}��, �S�p �� ���p �̕ϊ���� 4 �����ȓ�������A5�ȏ�ɂ��Ă����΂悢 */
	int m_len;
	/* str �ɂ͏����Ɏg���镶���񂪓����Ă��邪�A���̂����m�肳��钷�� */
	/* ��: �u��v�ɑ΂��� {"nb", 1}, �u�����v�ɑ΂��� {"lltu", 4} �Ȃ� */
};

ConvertData::ConvertData(const char *buf1, const char *buf3){
	strcpy(m_str, buf1);
	m_len = strlen(buf1) - strlen(buf3);
	/* �����ł́A�f�[�^��M�p���āAbuf3��buf1�̌���ƈ�v���邱�Ƃ����肷��B */
}

bool ConvertData::match_front(const char *str){	/* �f�[�^�̑O����str����v���邩 */
	for(int i=0; str[i]!='\0'; i++){
		if(str[i] != m_str[i]){
			return false;
		}
	}
	return true;
}

bool ConvertData::match(const char *str){
	return (strcmp(m_str, str) == 0);
}

bool ConvertData::get_unprocessed(char *buf){
	/* ���m�胍�[�}���������擾 */
	strcpy(buf, m_str + m_len);
	return strlen(buf) != 0;
}

/* ============================================================ */

class CTrieNode{
public:
	CTrieNode();
	~CTrieNode();
	void insert(const char *str, const ConvertData &data);	/* str�ł��ǂ������data������ */
	CTrieNode *find(const char *str);
	CTrieNode *find(char ch);
private:
	CTrieNode *m_next[256];
public:
	/* ����private�ɂ���̖ʓ| */
	vector<ConvertData> m_data;
};

CTrieNode::CTrieNode(){
	for(int i=0; i<256; i++){
		m_next[i] = NULL;
	}
}

CTrieNode::~CTrieNode(){
	for(int i=0; i<256; i++){
		if(m_next[i] != NULL){
			delete m_next[i];
		}
	}
}

void CTrieNode::insert(const char *str, const ConvertData &data){
	if(*str == '\0'){	/* ���ǂ�I������̂Ńf�[�^������ */
		m_data.push_back(data);
		return;
	}
	int i = *str & 0xff;	/* 0(1)�`255 �ɂ��� */
	if(m_next[i] == NULL){	/* ���ǂ�Ȃ��̂ŐV������� */
		m_next[i] = new CTrieNode();
	}
	m_next[i]->insert(str+1, data);	/* �������ǂ� */
}

CTrieNode *CTrieNode::find(const char *str){
	if(*str == '\0'){	/* ������OK */
		return this;
	}
	int i = *str & 0xff;	/* 0(1)�`255 �ɂ��� */
	if(m_next[i] == NULL){	/* �Ȃ� */
		return NULL;
	}
	return m_next[i]->find(str+1);	/* �������ǂ� */
}

CTrieNode *CTrieNode::find(char ch){
	char buf[2];
	buf[0] = ch;
	buf[1] = '\0';
	return find(buf);
}

/* ============================================================ */

struct LyricsBlock{
	int lyricsNum;	/* Lyrics�̔z��̉��Ԗڂ��炩 */
	int lyricsLen;	/* �ꂩ���܂�̒����iByte�j */
	double timeJust,timeUntil;	/* ���傤�ǂ̎��ԁA�i�����܂ł݂̂��j�ł��Ƃ��ł���ł��x������ */
	bool isValid;	/* �L���Ȃ�true�A�����łȂ��i�X�R�A�Ώۂł͂Ȃ��Ǘ��p�̂��낢��j�Ȃ�false */
	int nTyped;	/* ���o�C�g�ł��ꂽ�� */
	bool isScoringTarget;	/* ���݃^�C�~���O���������Ώۂł��邩 */
	int scoringCount;	/* �������O�Ɂi���ꎩ�g���܂܂Ȃ��j�X�R�A�Ώۂ̂��̂��������� */
	double scoringTime;
		/* �^�C�~���O��������ꂽ�����A!isScoringTarget�̂Ƃ��ȊO�͈Ӗ��͂Ȃ� */
	//int r, g, b;	/* �ł��n�߂̃G�t�F�N�g�̐F�iscoringTime���L���̎��A�L���j */
	double clearedTime;	/* Block���ł��؂�ꂽ�����A���Ȃ�܂� */
		/* ��؂�ɂ��ẮA��؂肪�Ӗ��������Ȃ��Ȃ������� */
	double x, y, vx, vy, vz;
		/* �iclearedTime���񕉂̎��j�ł��؂�ꂽ���̒��˂����i�����̓����_���j��ێ� */
public:
	int lyricsPos();
};

int LyricsBlock::lyricsPos(){	/* �ł��ĂȂ��ŏ��̂��̂�Lyrics�z��ł̓Y������Ԃ� */
	return lyricsNum + nTyped;
}

struct Lyrics{
	char ch;	/* �ϊ����ꂽ�����i�������A�B�`abc123+-�Ȃǁj��1�o�C�g */
	int blockNum;	/* LyricsBlock�̔z��̉��ԖڂɊ܂܂�邩 */
	int blockPos;	/* ���̂Ȃ��ŉ��o�C�g�ڂ� */
};

/* ============================================================ */

struct LyricsKanji{	/* �\�������̎� */
public:
	char str[256];	/* �̎� */
	double timeBegin;	/* �\�����n�܂鎞�� */
	double timeEnd;	/* �\�����I��鎞�� */
public:
	void draw(int x, int y, int fontHandle) const;
private:
	int getDrawColor(bool flag) const;
};

void LyricsKanji::draw(int x, int y, int fontHandle) const{
	char buf[256], *ptr1;
	const char *ptr2;
	bool flag = true;	/* ���ʂɏ���(true)���A�D�F�ŏ���(false)�� */
	ptr1 = buf;
	ptr2 = this->str;
	while(*ptr2 != '\0'){
		if(*ptr2 == '\\'){
			ptr2++;
			switch(*ptr2){
			case '\0':	/* *ptr2��0�̂܂܂ɂ���while���炱�̂܂ܔ����� */
				break;
			case '|':	/* �����ŁA�F��ύX */
				{
					*ptr1 = '\0';
					int len = strlen(buf);
					int width = GetDrawStringWidthToHandle(buf, len, fontHandle);
					DrawStringToHandle(x, y, buf,
						getDrawColor(flag), fontHandle);
					x += width;
					ptr2++;
					ptr1 = buf;
					flag = !flag;
				}
				break;
			default:
				*ptr1++ = *ptr2++;
				break;
			}
		}else if(isJapanese1st(*ptr2)){
			*ptr1++ = *ptr2++;
			if(*ptr2 == '\0'){
				break;
			}
			*ptr1++ = *ptr2++;
		}else{
			*ptr1++ = *ptr2++;
		}
	}
	*ptr1 = '\0';
	DrawStringToHandle(x, y, buf,
		getDrawColor(flag), fontHandle);
	return;
}

int LyricsKanji::getDrawColor(bool flag) const{
	if(flag)return GetColor(255, 255, 255);
	else return GetColor(64, 64, 85);
}

/* ============================================================ */

struct BeatLine{
	int type;
	double time;
};

/* ============================================================ */

struct ReplayData{
	char ch;
	double time;
};

/* ============================================================ */

class CTyping{
public:
	CTyping();
	~CTyping();
private:
	void loadDictionary(const char *fileName);
	
	void addLyrics(const char *buf, double time);
	void addNullLyrics(double time, bool isLast);
public:
	void load(const char *fumenFileName, const char *rankingFileName);
	void unload();
	void setName(const char *name);
	void setChallenge(CChallenge &challenge);
private:
	void keyboard(char ch, double timeCount);
	void keyboard_1(char ch, double time);
	void keyboardEffect(char ch, double time);
	void updateLyricsBuffer(double time);
	void clearLyricsBuffer(vector<LyricsBlock>::iterator lyricsPos);
	
	bool idle(double timeCount);
private:
	void loadRanking(const char *fileName);
	void unloadRanking();
	
	void setText(const char *str, int color);
	
	void setTime();
	void synchronizeTime(int soundHandle, double frequencyRate);
	double getTime() const;
	double getTime(double timeCount) const;
	//void skipTime(double sec);
	
	void phase(int phaseNum);
	
	/* ���͏����BtypeBuffer�����łɂ��鎞�ɁAch�����邩�ǂ����A�������瓾�_�v�Z�����typeBuffer���X�V */
	/* ch = '\0'�Ƃ��邱�Ƃɂ��A���镶�������݂��邩�ǂ������e�X�g�ł��� */
	bool input(char ch, char *typeBuffer, int &typeBufferLen,
		vector<LyricsBlock>::iterator &lyricsPosition, bool f_successive, double time);
	bool input_1(char *typeBuffer, int &typeBufferLen, vector<Lyrics>::iterator &lyricsPos,
		bool f_successive, double time, bool isCheckOnly, bool isSaiki);	/* input�̎��ۂɌv�Z���镔�� */
	
	void setTyped(vector<Lyrics>::iterator lyBegin, vector<Lyrics>::iterator lyEnd);
	
	void scoreTyping(vector<Lyrics>::iterator lyBegin, vector<Lyrics>::iterator lyEnd);
	void scoreAccuracy(vector<Lyrics>::iterator lyricsPos, bool f_successive, double time);
	void scoreAccuracySub(vector<LyricsBlock>::iterator lyricsPosition, int ID_accuracy);
	
	void initGauge();
	int getGaugeZone();
	void updateGauge();
	int getDrawGaugeColor(int pos, int cnt);
	double getDrawGaugeAlpha(int pos, int cnt);
	void drawGauge(bool isResult);

	void drawElapsedTime(double time);
	
	void drawTypeBuffer(double time);
	
	int getBlockStr(vector<LyricsBlock>::iterator itr, char *buf, bool f_getUntyped) const;
	int getLyricsBuffer(char *buf) const;
	
	int getDrawPosX(double timeDiff) const;
	int getDrawPosY(int x) const;
	
	void draw();
	void drawResult();
	
	void drawFlagAbort();
public:
	void mainLoop();
	void replayLoop();
private:
	bool saveReplay(const char *fileName, bool &f_confirm);
	bool loadReplay(const char *fileName);
public:
	bool saveReplayLoop();
	bool loadReplayLoop();
private:
	CTrieNode m_trie;
	
	vector<Lyrics> m_lyrics;
	
	vector<LyricsBlock> m_lyricsBlock;
	vector<LyricsBlock>::iterator m_lyricsPosition;
	
	vector<LyricsBlock>::iterator m_lyricsPositionEnd;
	/* ���ݑł��Ă���̎����ǂ��܂ł��A�܂�A[m_lyricsPosition, m_lyricsPositionEnd) */
	/* ���͈̔͂̕����񂪗���Ă���Ƃ���ƕʂ̂Ƃ���ɕ\������� */
	CEffectStr m_lyricsBufferEffect;
	
	vector<LyricsBlock>::iterator m_lyricsBlockDrawLeft;
	vector<LyricsBlock>::iterator m_lyricsBlockDrawRight;
	/* draw�ŁA���O�ɕ`�悳�ꂽ�͈� */
	
	vector<LyricsKanji> m_lyricsKanji;
	vector<LyricsKanji>::iterator m_lyricsKanjiPosition;
	
	vector<BeatLine> m_beatLine;
	vector<BeatLine>::iterator m_beatLineDrawLeft;
	vector<BeatLine>::iterator m_beatLineDrawRight;
	
	char m_typeBuffer[TYPE_BUFFER_LEN + 1];
	int m_typeBufferLen;
	CEffectStr m_typeBufferEffect;
	//deque<double> m_typeBufferTime;
	
	int m_phase;
	
	double m_timeBegin;
	
	char m_name[NAME_LEN + 1];	/* �v���[���[�̖��O�i�ݒ肳��Ă�����j */
	
	bool m_flagLockName;
		/* ���O�����ɐݒ肳��Ă���Ƃ��́AEsc����x�������܂œ��͂���ی삷�� */
	
	int m_score;
	int m_scoreTyping;	/* �^�C�s���O�ɂ�链�_ */
	int m_scoreAccuracy;	/* ���x�ɂ�链�_ */
	int m_combo;
	
	int m_comboMax;
	
	int m_countExcellent;
	int m_countGood;
	int m_countFair;
	int m_countPoor;
	int m_countPass;	/* �X�R�A�\�����Ɏg�� */
	int m_countAll;
	
	int m_countClear;	/* �ł��؂����� */
	
	CRanking m_ranking;	/* �n�C�X�R�A�̃f�[�^ */
	int m_rank;	/* �n�C�X�R�A�̒��ł̏��ʁA�����N�C�����Ȃ���� -1 */
	
	bool m_isReplay;
	vector<ReplayData> m_replayData;
	
	int m_scoreDraw;	/* �\������X�R�A�i��1/10�j�A�{���̃X�R�A�Ɣ�גx�������� */
	
	CEffect1 m_effect1;	/* �L�[���̓G�t�F�N�g */
	
	int m_gauge;	/* �����鍰�Q�[�W�݂����Ȃ��� */
		/* ���ۂ̒B�l�ɉe�����󂯂��킯����Ȃ��񂾂���˂� */
	int m_gaugeMax;	/* 0�ȏケ��ȉ��̊Ԃœ����I�ɂ͌v�Z���� */
	int m_gaugeLastCount;	/* �O��̔��萔��ۑ� */
	int m_gaugeNewCount;	/* ���̂��ߐV�K���萔��ۑ� */
	int m_gaugeLastLost;	/* �O��̑ł���������ۑ� */
	
	CStatGauge m_statGauge;
	
	char m_text[256];	/* ����i����уR���{���j��\�� */
	int m_textColor;
	double m_textTime;
	
	double m_timeFinished;	/* �Q�[�����I�����A�t�F�[�h�A�E�g���n�܂����i�Ȃ��n�܂��Ă���́j������ۑ� */
	double m_timeLength;	/* ���ʂ̍Ō�A�i�s���\���Ɏg�p */
	
	CChallenge m_challenge;
	
	char m_musicFileName[256];
	
	int m_idleCounter;	/* idle������ڂ̌Ăяo������m�邽�߂̃J�E���^ */
	
	int m_fontHandleNormal;
	int m_fontHandleBig;
	
	int m_soundHandleMusic;
	
	bool m_flagFailed;	/* �Q�[�W���؂�ċ����I�� */
	
	bool m_flagAbort;	/* Esc�Œ��f���ꂩ�����Ƃ���true�ɂȂ� */
};

CTyping::CTyping(){
	/* ���[�}�������̓R���X�g���N�^�œǂ� */
	if(!g_config.f_debugMode){	/* �ʏ�̂Ƃ� */
		loadDictionary("convert.dat");
	}
	strcpy(m_name, "");
	
	/* �`�������W���������iDefault�͉����ςȂ��Ƃ��Ȃ��j */
	/* �`�������W�͋ȃ��[�h�̂Ƃ����������Ȃ��̂� */
	m_challenge.reset();
	
	m_idleCounter = 0;
	
	m_fontHandleNormal = CreateFontToHandle(NULL, 16, 2);
	m_fontHandleBig = CreateFontToHandle("�l�r ����", 36, 2, DX_FONTTYPE_ANTIALIASING);
	
	m_soundHandleMusic = -1;	/* �����ǂݍ��񂾂肵�Ă͂��Ȃ� */
}

CTyping::~CTyping(){
	unload();
	DeleteFontToHandle(m_fontHandleNormal);	/* �t�H���g���폜 */
	DeleteFontToHandle(m_fontHandleBig);
}

void CTyping::loadDictionary(const char *fileName){
	FILE *fp;
	fp = fopen(fileName,"r");
	if(fp == NULL){
		throw "���[�}���������J���܂���B";
	}
	char buf[256], buf1[256], buf2[256], buf3[256];
	while(fgetline(buf, fp)!=NULL){
		int n=sscanf(buf, "%s%s%s", buf1, buf2, buf3);
		/* ���[�}���A���{��A�c�郍�[�}�� */
		if(n<2){
			throw "���[�}�������̌`�����s���ł��B";
		}
		int len;
		if(n == 2){
			strcpy(buf3, "");
		}
		m_trie.insert(buf2, ConvertData(buf1, buf3));
		/* ���{����L�[�Ƀ��[�}���̃f�[�^������ */
	}
	fclose(fp);
}

void CTyping::addLyrics(const char *buf, double time){
	if(!m_lyricsBlock.empty()){	/* �̎������݂��Ă��� */
		LyricsBlock &lb = *(m_lyricsBlock.end() - 1);	/* ���̂Ȃ��ōŌ�̂��̂𑀍� */
		if(lb.timeUntil == INFTY){	/* �I���������ݒ肳��Ă��Ȃ���΁A���݂ɐݒ� */
			lb.timeUntil = time;
		}
	}
	
	LyricsBlock lb;
	lb.lyricsNum = m_lyrics.size();
	lb.lyricsLen = strlen(buf);
//printfDx("%d ", lb.lyricsLen);
	lb.timeJust = time;
	lb.timeUntil = INFTY;
	lb.isValid = true;
	lb.nTyped = 0;
	lb.isScoringTarget = true;
	lb.scoringCount = m_countAll;
	lb.clearedTime = -INFTY;
	Lyrics ly;
	ly.blockNum = m_lyricsBlock.size();
	for(int cnt=0; buf[cnt]!='\0'; cnt++){
		ly.ch = buf[cnt];
		ly.blockPos = cnt;
		m_lyrics.push_back(ly);
	}
	m_lyricsBlock.push_back(lb);
	
	m_countAll++;	/* ����̂���̎��̐��𐔂��� */
}

void CTyping::addNullLyrics(double time, bool isLast){
	if(!m_lyricsBlock.empty()){	/* �̎������݂��Ă��� */
		LyricsBlock &lb = *(m_lyricsBlock.end() - 1);	/* ���̂Ȃ��ōŌ�̂��̂𑀍� */
		if(lb.timeUntil == INFTY){	/* �I���������ݒ肳��Ă��Ȃ���΁A���݂ɐݒ� */
			lb.timeUntil = time;
		}
	}
	
	LyricsBlock lb;
	lb.lyricsNum = m_lyrics.size();
	lb.lyricsLen = 0;	/* �u" "�����邩��1�v�Ȃǂ��킴�킴���Ȃ��Ă��悢 */
		/* �ނ���A������擾�̎���0�łȂ��ƃS�~�������č��� */
	lb.isValid = isLast;	/* �Ō��isValid = true�̔ԕ���u���B����͖��Q�B */
	lb.nTyped = 0;
	lb.timeJust = time;
	lb.timeUntil = time;
	lb.isScoringTarget = false;
	lb.scoringCount = m_countAll;
	lb.clearedTime = -INFTY;
	Lyrics ly;
	ly.blockNum = m_lyricsBlock.size();
	ly.ch = ' ';	/* �ł��Ƃ��ł��Ȃ�' '��ԕ��ɂ��� */
	m_lyricsBlock.push_back(lb);
	m_lyrics.push_back(ly);
}

void CTyping::load(const char *fumenFileName, const char *rankingFileName){
	char directoryName[256];
	getDirFromPath(directoryName, fumenFileName);
	FILE *fp;
	fp = fopen(fumenFileName,"r");
	if(fp == NULL){
		throw "���ʃt�@�C�����J���܂���B";
	}
	
	/* �ǂݍ��ޑO�ɑS���� */
	m_lyrics.clear();
	m_lyricsBlock.clear();
	
	m_lyricsKanji.clear();
	m_beatLine.clear();
	strcpy(m_musicFileName, "");
	
	/* �̎��̂����܂�̐��i=m_count�����̘a�̍ő�l�j�𐔂��邽�߂�0�� */
	/* �܂��ALyricsBlock��scoringCount��ݒ肵�Ă������� */
	m_countAll = 0;
	
	double frequencyRate = m_challenge.frequencyRate();
	/* KeyShift�ɂ����g���̔{�� */
	
	m_timeLength = -1;

	/* �ǂݍ��݊J�n */
	
	char tmpBuf[256];
	char buf[256];
	double time;
	while(fgetline(tmpBuf, fp) != NULL){
		switch(tmpBuf[0]){
		case '+':	/* �ł̎� */
		{
			if(g_config.f_debugMode && g_config.f_debugBeat){
				/* �ł̎��͂��ׂĖ��� */
				continue;
			}
			int n = sscanf(tmpBuf + 1, "%lf%s", &time, buf);
			if(n < 2){
				throw "[����] �������s���ł��B(+)";
			}
			time /= frequencyRate;
			addLyrics(buf, time);
		}
			break;
		case '*':	/* �\������̎� */
		{
			char *ptr = strtok(tmpBuf + 1, " \t\n");	/* �X�y�[�X�ŋ�؂���܂ł̕��������o�� */
			if(ptr == NULL){
				throw "[����] �������s���ł��B(*)";
			}
			int n = sscanf(ptr, "%lf", &time);	/* ������ǂ� */
			if(n < 1){
				throw "[����] �������s���ł��B(*)";
			}
			time /= frequencyRate;
			ptr = strtok(NULL, "");	/* �c��̕��������o�� */
			LyricsKanji lk;
			strcpy(lk.str, ptr);
			lk.timeBegin = time;
			lk.timeEnd = INFTY;	/* �I��肪�ݒ肳��Ȃ���΁A�\�����ꑱ���� */
			m_lyricsKanji.push_back(lk);
		}
			break;
		case '=':	/* ���ߐ� */
		case '-':	/* ���� */
		{
			int n = sscanf(tmpBuf + 1, "%lf", &time);	/* ������ǂ� */
			if(n < 1){
				throw "[����] �������s���ł��B(=,-)";
			}
			time /= frequencyRate;
			BeatLine bl;
			if(tmpBuf[0] == '='){
				bl.type = BEATLINE_BAR;
			}else{	/* == '-' */
				bl.type = BEATLINE_BEAT;
			}
			bl.time = time;
			m_beatLine.push_back(bl);
			if(g_config.f_debugMode && g_config.f_debugBeat){
				/* ���q����"+"��ł̎��Ƃ��� */
				addLyrics("+", time);
			}
		}
			break;
		case '/':	/* ��؂� */
		{
			int n = sscanf(tmpBuf + 1, "%lf", &time);
			if(n < 1){
				throw "[����] �������s���ł��B(/)";
			}
			m_timeLength = time;	/* ���񏑂�����ŁA�Ō�̋�؂�̈ʒu���擾 */
			time /= frequencyRate;
			if(!m_lyricsKanji.empty()){	/* �\������̎������݂��Ă��� */
				LyricsKanji &lk = *(m_lyricsKanji.end() - 1);	/* ���̂Ȃ��ōŌ�̂��̂𑀍� */
				if(lk.timeEnd == INFTY){	/* �I���������ݒ肳��Ă��Ȃ���΁A���݂ɐݒ� */
					lk.timeEnd = time;
				}
			}
			if(g_config.f_debugMode && g_config.f_debugBeat){
				/* �ł̎��͂��ׂĖ����Ȃ̂ŁA��؂������ */
				continue;
			}
			
			addNullLyrics(time, false);
		}
			break;
		case '@':
			sprintf(m_musicFileName, "%s%s", directoryName, tmpBuf + 1);	/* �Đ����鉹�y�t�@�C������ݒ� */
			continue;
		default:
			continue;
		}
	}
	fclose(fp);

	if(m_countAll == 0){
		throw "[����] �ł̎�������܂���B";
	}
	
	/* �ł̎��̔ԕ��Ƃ��čŌ��' ',INFTY��ǉ����Ă��� */
	addNullLyrics(INFTY, false);
	/* !isValid�Ȃ�1�i�ނ��Ƃւ̑΍�Ƃ��āAisValid�Ȃ̂�ǉ����Ă��� */
	addNullLyrics(INFTY, true);
	
	/* �\������̎��̔ԕ��Ƃ��āA"",INFTY��ǉ� */
	{
		LyricsKanji lk;
		strcpy(lk.str, "");
		lk.timeBegin = INFTY;
		lk.timeEnd = INFTY;
		m_lyricsKanji.push_back(lk);
	}
	
	/* �������ԕ� */
	{
		BeatLine bl;
		bl.time = INFTY;
		m_beatLine.push_back(bl);
	}
	
	/* ���ݑł��Ă���̎��̈ʒu */
	m_lyricsPosition = m_lyricsBlock.begin();
	while(!(*m_lyricsPosition).isValid){
		m_lyricsPosition++;
	}
	
	m_lyricsKanjiPosition = m_lyricsKanji.begin();
	
	m_lyricsPositionEnd = m_lyricsPosition;	/* �ŏ��͔͈͂Ȃ��Ƃ������Ƃɂ��Ă����� */
	
	/* �^�C�v����ׂ��̎��ւ�����^�C�s���O���ʂȂ��G�t�F�N�g�������� */
	/* typeBuffer�Ɣ�ׂď����߂̃G�t�F�N�g */
	m_lyricsBufferEffect.init(m_fontHandleBig, false, 50.0, 0.15);
	
	m_lyricsBlockDrawLeft = m_lyricsBlock.begin();
	m_lyricsBlockDrawRight = m_lyricsBlockDrawLeft;
	/* �ŏ���draw�Őݒ肳��邩�牽�����Ă��悢�̂����A */
	/* �E�ɂ����i�܂Ȃ��̂ōŏ��ɂ��Ă����Ȃ���΂Ȃ�Ȃ� */
	
	/* ���R�͓��� */
	m_beatLineDrawLeft = m_beatLine.begin();
	m_beatLineDrawRight = m_beatLine.begin();
	
	/* �L�[���͂������� */
	m_typeBufferLen = 0;
	
	/* �^�C�v����ʒu�ւ�����^�C�s���O���ʂ���G�t�F�N�g�������� */
	/* lyricsBuffer�Ɣ�ׂđ傫�߂̃G�t�F�N�g */
	m_typeBufferEffect.init(m_fontHandleBig, true, 15.0, 0.3);
	//m_typeBufferTime.clear();
	
	/* �����L�[�������܂ł͎n�܂�Ȃ��̂ŁA�܂��͂��܂��Ă��Ȃ� */
	phase(PHASE_READY);
	
	/* �_�������� */
	m_score = 0;
	m_scoreTyping = 0;
	m_scoreAccuracy = 0;
	m_combo = 0;
	
	m_comboMax = 0;
	
	/* �D�ǉs����J�E���g������ */
	m_countExcellent = 0;
	m_countGood = 0;
	m_countFair = 0;
	m_countPoor = 0;
	
	/* �ł��؂�J�E���g������ */
	m_countClear = 0;
	
	strcpy(m_text, "");
	
	m_scoreDraw = 0;
	
	m_effect1.clear();	/* �L�[���̓G�t�F�N�g�������� */
	
	initGauge();	/* �Q�[�W������ */
	
	m_statGauge.init();	/* ���v�Q�[�W������ */
	
	loadRanking(rankingFileName);
	/* �����L���O�ǂݍ��� */
	
	m_soundHandleMusic = -1;
	
	m_flagFailed = false;
	
	m_flagAbort = false;
}

void CTyping::unload(){
	if(m_soundHandleMusic != -1){
		DeleteSoundMem(m_soundHandleMusic);
		m_soundHandleMusic = -1;
	}
	unloadRanking();
}

void CTyping::setName(const char *name){
	strcpy(m_name, name);
}

void CTyping::setChallenge(CChallenge &challenge){
	m_challenge = challenge;
}

void CTyping::keyboard(char ch, double timeCount){
	if(m_phase < PHASE_RESULT && ch == CTRL_CODE_ESC){
	/* ���ʉ�ʂ���ł�Esc�ɂ��I���������Ȃ� */
		if(m_flagAbort){	/* ���ꂪ2��� */
			phase(PHASE_EXIT);
		}else{
			m_flagAbort = true;
		}
		return;
	}
	m_flagAbort = false;	/* ���̃L�[�������ꂽ�璆�f�������� */
	
	if(m_phase == PHASE_READY){	/* �J�n�O�Ȃ� */
		if(m_soundHandleMusic != -1){	/* ���y�����łɓǂݍ��܂ꂽ */
			phase(PHASE_MAIN);
			
			if(g_config.f_debugMode && g_config.debugTime > 0){	/* �r������n�߂�f�o�b�O���[�h�̂Ƃ� */
				SetSoundCurrentTime(g_config.debugTime, m_soundHandleMusic);
				/* �y SetSoundCurrentTime ���g���Ă���̂Œ��ӂ��K�v��������Ȃ��z */
				PlaySoundMem(m_soundHandleMusic, DX_PLAYTYPE_BACK, FALSE);	/* ���y�𗬂��n�߂� */
			}else{
				/* �ʏ펞 */
				PlaySoundMem(m_soundHandleMusic, DX_PLAYTYPE_BACK);	/* ���y�𗬂��n�߂� */
			}
			
			setTime();	/* �n�܂����������o���� */
			
			synchronizeTime(m_soundHandleMusic, m_challenge.frequencyRate());
			/* �ꉞ�^�C�}�[���߁B�ǂ��炩�Ƃ����΁A�Đ��J�n�ʒu��ύX�����Ƃ��ɗL�p�B */
		}
		return;
	}
	if(m_phase == PHASE_RESULT){	/* �X�R�A�\�����Ȃ� */
		/* ���O�L���̂��߂Ƀo�b�t�@��ǂݏ��� */
		double time = getTime(timeCount);	/* �L�[�{�[�h�����������܂ł̎��Ԃ��擾 */
		
		if(ch == ' '){	/* �X�y�[�X�̓A���_�[�o�[�ɕϊ��i�X�y�[�X�݂̂̈Ⴂ�����Ȃ����߁j */
			ch = '_';
		}
		int ret;
		if(m_flagLockName){
			char buf[1];
			int len = 0;
			ret = editBuffer(ch, buf, len);	/* �_�~�[�𑀍� */
			/* Enter��Esc�������ꂽ���ǂ����݂̂��d�v */
		}else{
			ret = editBuffer(ch, m_typeBuffer, m_typeBufferLen);
		}
		switch(ret){
		case EDIT_BUFFER_OK:
			if(time < 4.0){	/* ���ʂ����ׂďo��܂Ŋm�肳���Ȃ� */
				//skipTime(1.0);	/* �҂̂����Ȑl�̂��߂Ɂc�c */
				return;
			}
			if(m_challenge.isEasy() || m_isReplay || g_config.f_debugMode){
				/* �ȒP�ɂȂ�I�v�V�������g�p�����ꍇ�A�����L���O�ɂ̂��Ȃ� */
				/* ���v���C����f�o�b�O���͋L�^���Ȃ� */
				phase(PHASE_FINISHED);
			}else{	/* ���v���C���E�f�o�b�O���[�h�� */
				if(m_typeBufferLen > 0){	/* ���O�����͂���Ă��邩�m�F */
					phase(PHASE_FINISHED);
				}
			}
			break;
		case EDIT_BUFFER_CANCEL:
			if(m_flagLockName){
				m_flagLockName = false;	/* ���O�̏��������h�~���Ȃ��� */
			}else{
				/* ���O���͂�S���� */
				strcpy(m_typeBuffer, "");
				m_typeBufferLen = 0;
			}
			break;
		}
		return;
	}
	if(m_phase == PHASE_FINISHED){	/* �����L���O�i���ʂɃ����N�C���������j�\�����Ȃ� */
		if(ch == CTRL_CODE_CR){	/* Enter�ŏI�� */
			phase(PHASE_EXIT);
		}
		return;
	}
	
	if(m_phase == PHASE_MAIN){	/* �Q�[�����C���̂Ƃ��̏��� */
		double time = getTime(timeCount);	/* �L�[�{�[�h�����������܂ł̎��Ԃ��擾 */
		keyboard_1(ch, time);
	}
}

void CTyping::keyboard_1(char ch, double time){
	/* �C�ӂ̂��̂�ł������Ƃɂł���'\0'�͓��͂ɗ��Ȃ��͂������A����Ɣ��ɍ��� */
#if 0
	if(ch == '\0' || ch == ' '){	/* ' '�͂ǂ�������Ȃ��̂����A���e����^����\���������̂� */
#endif
	if(ch < CTRL_CODE_CMP || ch == ' '){	/* ' '������悤�Ȃ��Ƃ͂��Ȃ� */
	/* ��������Ȃ��ꍇ�͖������Ă悢 */
		return;
	}
	
//printf("%c %f\n", ch, time);
#if 0
	/* input���Ń`�F�b�N���邱�ƂɂȂ��� */
	if(time < (*m_lyricsPosition).timeJust - SEC_POOR){
		/* poor���O�Ȃ̂őłĂȂ� */
		return;
	}
#endif
	
	if(input(ch, m_typeBuffer, m_typeBufferLen, m_lyricsPosition, true, time)){
//printf("ok\n");
	/* ���̓��͂����݂̈ʒu�œ����� */
		if(!m_isReplay){	/* ���v���C�ɋL�^ */
			ReplayData rd;
			rd.ch = ch;
			rd.time = time;
			m_replayData.push_back(rd);
		}
		
		while(!(*m_lyricsPosition).isValid){	/* �̎��̐؂�ڂ��w���Ă���Ȃ� */
			m_lyricsPosition->clearedTime = time;
			m_lyricsPosition++;	/* �i�߂� */
		}
		
		/* �L�[���̓G�t�F�N�g */
		keyboardEffect(ch, time);
		
		updateLyricsBuffer(time);
		
		updateGauge();
		return;
	}
	
	char tmpBuffer[TYPE_BUFFER_LEN + 1];
	int tmpLen;
	vector<LyricsBlock>::iterator tmpLyricsPosition;
	tmpLen = 0;
	tmpLyricsPosition = m_lyricsPosition;
	
	while(1){
//printf("loop\n");
		tmpLyricsPosition++;	/* �K��1�ȏ��΂��K�v������̂ōŏ���++ */
		if(time < (*tmpLyricsPosition).timeJust - SEC_POOR){
			/* poor�͈͂ɓ����Ă���Ƃ�����O�ɒT�����o�� */
			break;
		}
		if(!(*tmpLyricsPosition).isScoringTarget){
			/* ���łɃ^�C�~���O���̓_���ꂽ�Ƃ����A��؂� */
			continue;
		}
		vector<LyricsBlock>::iterator lyricsPos = tmpLyricsPosition;
			/* input��tmpLyricsPosition���ύX���ꂤ��̂ōT���Ă����B */
			/* �y�A���S���Y���ς������z */
		
		if(input(ch, tmpBuffer, tmpLen, tmpLyricsPosition, false, time)){
//printf("ok\n");
		/* �V�����ʒu�ł��̓��͂������� */
			if(!m_isReplay){	/* ���v���C�ɋL�^ */
				ReplayData rd;
				rd.ch = ch;
				rd.time = time;
				m_replayData.push_back(rd);
			}
			
			/* �V�����f�[�^���������� */
			memcpy(m_typeBuffer, tmpBuffer, tmpLen);
			m_typeBufferLen = tmpLen;
			m_lyricsPosition = tmpLyricsPosition;
			while(!(*m_lyricsPosition).isValid){	/* �̎��̐؂�ڂ��w���Ă���Ȃ� */
				m_lyricsPosition->clearedTime = time;
				m_lyricsPosition++;	/* �i�߂� */
			}
			
			/* �L�[���̓G�t�F�N�g */
			keyboardEffect(ch, time);
			
			clearLyricsBuffer(lyricsPos);	/* �Â����e����x�������Ă��� */
			updateLyricsBuffer(time);	/* �V�����ݒ� */
			
			updateGauge();
			return;
		}
	}
//printf("ng\n");
	return;
}

void CTyping::keyboardEffect(char ch, double time){
	m_effect1.insert(X_CIRCLE, Y_CIRCLE, ch, GetColor(170, 170, 255), time);
	//m_typeBufferTime.push_back(time);
	m_typeBufferEffect.push(ch, time);
	m_typeBufferEffect.update(m_typeBuffer, m_typeBufferLen, time);
	return;
}

void CTyping::updateLyricsBuffer(double time){
	/* m_lyricsPositionEnd�̍X�V�Ƃ���ɔ����G�t�F�N�g�̏��̒ǉ� */
	{
		vector<LyricsBlock>::iterator lastPos = m_lyricsPositionEnd;
		
		if(m_lyricsPositionEnd < m_lyricsPosition){
			m_lyricsPositionEnd = m_lyricsPosition;
		}
		
		if(m_lyricsPositionEnd->timeJust != INFTY){
#if 0
			while(m_lyricsPositionEnd->timeJust != INFTY &&
						/* �[�𒴂��Ȃ��悤�Ƀ`�F�b�N */
					( time >= m_lyricsPositionEnd->timeJust
						/* �܂��A���傤�ǂ̈ʒu�ɒB���Ă��Ȃ��ŏ��̉̎��܂Ői�� */
					|| (m_lyricsPositionEnd->isValid && !m_lyricsPositionEnd->isScoringTarget) )){
						/* �̓_�ς݁A�܂�A�ł������̂��̂��܂ނ悤�ɔ͈͂𑝂₷ */
				m_lyricsPositionEnd++;
			}
#endif
			if(!(m_challenge.test(CHALLENGE_HIDDEN) || m_challenge.test(CHALLENGE_STEALTH))){
			/* Hidden, Stealth�ł́A���Ԍo�߂ɂ��łׂ��ɂȂ������Ƃ������Ȃ� */
				while(time >= m_lyricsPositionEnd->timeJust){
					/* �܂��A���傤�ǂ̈ʒu�ɒB���Ă��Ȃ��ŏ��̉̎��܂Ői�� */
					m_lyricsPositionEnd++;
				}
			}
			
			while(m_lyricsPositionEnd->isValid && !m_lyricsPositionEnd->isScoringTarget){
				/* �̓_�ς݁A�܂�A�ł������̂��̂��܂ނ悤�ɔ͈͂𑝂₷ */
				m_lyricsPositionEnd++;
			}
		}
		
		while(lastPos != m_lyricsPositionEnd){
			char buf[256];
			getBlockStr(lastPos, buf, false);
			if(lastPos->timeJust < time){
				m_lyricsBufferEffect.push(buf, lastPos->timeJust);
			}else{
				m_lyricsBufferEffect.push(buf, time);
			}
			lastPos++;
		}
	}
	
	{	/* �ǂ̕��������ۂɎc���Ă��邩�������� */
		char buf[256];
		int len;
		len = getLyricsBuffer(buf);
		
		m_lyricsBufferEffect.update(buf, len, time);
	}
}

/* �ǂ��܂ł̓��e������������`���� */
void CTyping::clearLyricsBuffer(vector<LyricsBlock>::iterator lyricsPos){
	/* LyricsBuffer�̓��e�������B����ɂ���Ďc���𔭐������Ȃ��B */
	/* ���Ƃ��Ƃ������c���͎c���B */
	m_lyricsPositionEnd = lyricsPos;
	m_lyricsBufferEffect.clear();
}

/* ------------------------------------------------------------ */

bool CTyping::idle(double timeCount){	/* ������Ȃ�i�I������Ƃ��ȊO�j true ��Ԃ� */
	if(m_phase == PHASE_EXIT){	/* �I�����邱�ƂɂȂ��Ă��� */
		return false;
	}
	if(m_phase == PHASE_READY){	/* �J�n���Ă��Ȃ���Γ��ɂ��邱�Ƃ͖��� */
		if(m_soundHandleMusic == -1){	/* �܂����y��ǂݍ���łȂ�������ǂݍ��� */
			SetCreateSoundDataType(g_config.loadSoundType);	/* config���݂ēǂݍ��ݕ������� */
			m_soundHandleMusic = LoadSoundMem(m_musicFileName);
			if(m_soundHandleMusic == -1){
				throw "[����] �ȃt�@�C�����Đ��ł��܂���B";
			}
			if(g_config.volume >= 0){
				/* ���ʂ��w�肳��Ă���΁A���ʐݒ� */
				ChangeVolumeSoundMem(g_config.volume, m_soundHandleMusic);
			}
			
			if(m_challenge.key() != 0){	/* �L�[�V�t�g���s���Ă��� */
				int oldFrequency = GetFrequencySoundMem(m_soundHandleMusic);	/* ���̍Đ����g����ǂݍ��� */
				int newFrequency = (int)(oldFrequency *m_challenge.frequencyRate());
					/* �ύX����{���������ĐV�������g�����o�� */
				SetFrequencySoundMem(newFrequency, m_soundHandleMusic);	/* �ύX */
			}
		}
		return true;
	}
	if(m_phase == PHASE_FADEOUT){
		double time = getTime(timeCount);
		if(time >= m_timeFinished + SEC_FADEOUT){	/* �Ō�̉������ʉ߂�����t�F�[�h�A�E�g���I������ */
			phase(PHASE_RESULT);	/* ���ʉ�ʂɂ��� */
			if(m_phase == PHASE_EXIT){	/* �i���ʉ�ʂ��Ȃ��Łj�I�����邱�ƂɂȂ��� */
				return false;
			}
			return true;
		}
	}
	
	if(m_phase != PHASE_MAIN){	/* ���������̓Q�[�����C���̂Ƃ��̏��� */
		return true;
	}
	
	if(m_flagFailed){	/* �Q�[�W�؂�ŋ����I�� */
		phase(PHASE_FADEOUT);
		return true;
	}
	
//printf("idle[");
	double time = getTime(timeCount);
	
#if 0
	if((*m_lyricsPosition).timeJust == INFTY){
		/* �Ō��isValid = true�Ȃ̂ɔԕ��Ȃ�̂Ƃ���ɂ��� */
		phase(PHASE_FADEOUT);
		return true;
	}
	
	if(flag){	/* ����`�F�b�N����Əd����������Ȃ��̂ŁA�K�v�ȂƂ��Ɍ��肷�� */
		/* �u�����̃L�[�������Ό��݂̈ʒu�œ��͉\�v�ł͂Ȃ� */
		if(!input('\0', m_typeBuffer, m_typeBufferLen, m_lyricsPosition, true, time)){
			/* ch��'\0'�Ƃ��āA�łĂ���͂����݂��邩�`�F�b�N */
			/* f_successive��true�Ȃ̂ɓ��ɈӖ��͂Ȃ� */
			/* ���͂��؂ꂽ���͔̂�΂��im_lyricsPosition���i��ŁA�R���{���؂��j */
			while(time >= (*m_lyricsPosition).timeUntil){
				m_lyricsPosition++;
				
				/* �{����m_lyricsPosition���i�񂾂�A�����ɗ��� */
				m_combo = 0;
				m_typeBufferLen = 0;
				/* ���͒��̂͂����g���邱�Ƃ͂Ȃ��i����A�����ŏ����Ă��܂��Ă悢�j */
			}
			
			updateGauge();
		}
	}
#endif
#if 0
	if(!input('\0', m_typeBuffer, m_typeBufferLen, m_lyricsPosition, true, time)){
		/* ch��'\0'�Ƃ��āA�łĂ���͂����݂��邩�`�F�b�N */
		/* f_successive��true�Ȃ̂ɓ��ɈӖ��͂Ȃ� */
		/* ���͂��؂ꂽ���͔̂�΂��im_lyricsPosition���i��ŁA�R���{���؂��j */
//printfDx(".");
		m_combo = 0;
		m_typeBufferLen = 0;
		/* ���͒��̂͂����g���邱�Ƃ͂Ȃ��i����A�����ŏ����Ă��܂��Ă悢�j */
		do{
		/* �y�ʂ�while�ł����������A���ʂ̃~�X�Łu��ΑłĂȂ������v���������ꍇ��΂���z */
//printfDx("+\n");
			m_lyricsPosition++;
		}while(time >= (*m_lyricsPosition).timeUntil);
		updateGauge();
	}
#endif
	while(1){
		if(m_lyricsPosition->timeJust == INFTY){
			/* �Ō��isValid = true�Ȃ̂ɔԕ��Ȃ�̂Ƃ���ɂ��� */
			//m_lyricsPositionEnd = m_lyricsPosition;
			phase(PHASE_FADEOUT);
			return true;
		}
		/* ch��'\0'�Ƃ��āA�łĂ���͂����݂��邩�`�F�b�N */
		/* f_successive��true�Ȃ̂ɓ��ɈӖ��͂Ȃ� */
		if(input('\0', m_typeBuffer, m_typeBufferLen, m_lyricsPosition, true, time)){
			/* ���͉\�Ȃ烋�[�v�𔲂��� */
			break;
		}
		/* ���͂��؂ꂽ���͔̂�΂��im_lyricsPosition���i��ŁA�R���{���؂��j */
		m_combo = 0;
		m_typeBufferLen = 0;
		/* ���͒��̂͂����g���邱�Ƃ͂Ȃ��i����A�����ŏ����Ă��܂��Ă悢�j */
		m_lyricsPosition++;
		while(!(*m_lyricsPosition).isValid){
			m_lyricsPosition++;
		}
		clearLyricsBuffer(m_lyricsPosition);
		
		updateGauge();
	}
	
	// lyricsPositionEnd�̍X�V��updateLyricsBuffer�Ɉړ��B��������draw�ł�邩�炱���ł��Ȃ��B
	
//printf("]\n");
	if(m_idleCounter++ % 20 == 0){	/* 20���1����s */
		/* ���������y�Ɠ��� */
		synchronizeTime(m_soundHandleMusic, m_challenge.frequencyRate());
	}
	
	return true;
}

/* ------------------------------------------------------------ */

void CTyping::loadRanking(const char *fileName){
	m_ranking.open(fileName);
	m_ranking.read();
}

void CTyping::unloadRanking(){
	m_ranking.close();
}

/* ------------------------------------------------------------ */

void CTyping::setText(const char *str, int color){	/* �\���������ݒ� */
	m_textTime = getTime();
	strcpy(m_text, str);
	m_textColor = color;
}

/* ------------------------------------------------------------ */

void CTyping::setTime(){	/* �J�n������ݒ� */
	m_timeBegin = myGetNowHiPerformanceCount();
}

/* ���y�̃X�^�[�g�ƊJ�n�������������Ȃ�悤�ɁA�J�n�����𒲐� */
void CTyping::synchronizeTime(int soundHandle, double frequencyRate){
	if(soundHandle == -1 || CheckSoundMem(soundHandle) != 1){
	/* ���y���ǂݍ��܂�Ă��Ȃ����A�Đ�����Ă��Ȃ����̂Ƃ� */
		return;
	}
	
	double musicTime;
	musicTime = (GetSoundCurrentTime(soundHandle) / 1000.0) / frequencyRate;
	/* �y GetSoundCurrentTime ���g���Ă���̂Œ��ӂ��K�v��������Ȃ��z */
	/* �Ȃ̎��g����ς��Ă鎞�́A���ۂɗ��ꂽ���Ԃ��Ƃ邽�߁A���g���̔䂾������ */
	double realTime = myGetNowHiPerformanceCount() - m_timeBegin;
	double diff = realTime - musicTime;
//printfDx("%.4f %.4f\n", musicTime, realTime);
	
	/* �킸���Ȃ���iGetSoundCurrentTime�łƂ�Ȃ��悤�ȁj�͖��� */
	if(diff >= 0.0005){
		diff -= 0.0005;
	}else if(diff <= -0.0005){
		diff += 0.0005;
	}else{
		return;
	}
	
	m_timeBegin += diff;
}

double CTyping::getTime() const{	/* �J�n��������̌o�ߕb���擾 */
	return myGetNowHiPerformanceCount() - m_timeBegin;
}

double CTyping::getTime(double timeCount) const{	/* timeCount�̊J�n��������̌o�ߕb���擾 */
	return timeCount - m_timeBegin;
}

#if 0
void CTyping::skipTime(double sec){	/* �o�ߎ��Ԃ�sec�������₷ */
	m_timeBegin -= sec;	/* �J�n�����������������Ƃɂ���Ή��� */
}
#endif

/* ------------------------------------------------------------ */

void CTyping::phase(int phaseNum){
	if(m_phase == PHASE_MAIN){	/* �v���C���Ԃ����Z */
		if(!(m_isReplay || g_config.f_debugMode)){	/* �ʏ펞�̂� */
		/* m_challenge.isEasy()�ł������� */
			double playTime = getTime();
			m_ranking.addPlayTime(playTime);
		}
	}
	
	if(phaseNum == PHASE_FADEOUT){	/* �I������̂Ńt�F�[�h�A�E�g���� */
		m_timeFinished = getTime();	/* �I�����������ۑ� */
	}else if(phaseNum == PHASE_RESULT){	/* �I�����āA�X�R�A�\���� */
		if(m_flagFailed){
			phase(PHASE_EXIT);	/* �����I�����́A�X�R�A�\���Ȃ� */
			return;
		}
		setTime();	/* �\���J�n����Ă���̎��Ԃ𗘗p���邽�߂ɃZ�b�g */
		
		strcpy(m_typeBuffer, m_name);	/* �o�b�t�@�ɖ��O������B */
		m_typeBufferLen = strlen(m_name);
		if(m_typeBufferLen > 0){	/* ���O�����ɓ��͂���Ă���Ȃ�A�ی삷�� */
			m_flagLockName = true;
		}else{
			m_flagLockName = false;
		}
		
		m_flagAbort = false;	/* Esc�ɂ�钆�f�������� */
		
		/* �ʉ߂������𐔂��Ă��� */
		m_countPass = m_countAll - m_countExcellent - m_countGood - m_countFair - m_countPoor;
		
		/* �R���{��������ʒu���ŏI�������i�Ō�̕����������Ƒł��؂����j */
		if(m_combo == m_countAll){
			m_comboMax = -1;	/* �ő�R���{�����u�t���R���{�v�Ƃ��Ă��� */
		}
	}else if(phaseNum == PHASE_FINISHED){
		if(m_challenge.isEasy() || m_isReplay || g_config.f_debugMode){
			/* �ȒP�ɂȂ�I�v�V�������g�p�����ꍇ�A�����L���O�ɂ̂��Ȃ� */
			/* ���v���C����f�o�b�O���͋L�^���Ȃ� */
			phase(PHASE_EXIT);
			return;
		}else{
			CScore score(m_typeBuffer, m_score, m_scoreAccuracy, m_scoreTyping,
				m_countExcellent, m_countGood, m_countFair, m_countPoor, m_countPass,
				m_countAll, m_comboMax, m_challenge);
			int achievement = score.getLevel();
			if(achievement == SCORE_NO_DATA){	/* �Q�[�W�Ɉˑ�����B���x */
				switch(getGaugeZone()){
				case ID_FAILED_ZONE:
					achievement = SCORE_FAILED; break;
				case ID_RED_ZONE:
					achievement = SCORE_RED_ZONE; break;
				case ID_YELLOW_ZONE:
					achievement = SCORE_YELLOW_ZONE; break;
				case ID_BLUE_ZONE:
					achievement = SCORE_BLUE_ZONE; break;
				case ID_CLEAR_ZONE:
					achievement = SCORE_CLEAR; break;
				}
			}

			m_rank = m_ranking.update(score, achievement, 
					g_config.f_rankingCheckDate, g_config.f_rankingCheckChallenge);
			m_ranking.write();
			if(m_rank < 0){
				phase(PHASE_EXIT);	/* �����N�C�����Ȃ���ΏI�� */
				return;
			}
		}
	}
	m_phase = phaseNum;
}

/* ------------------------------------------------------------ */

bool CTyping::input(char ch, char *typeBuffer, int &typeBufferLen,
		vector<LyricsBlock>::iterator &lyricsPosition, bool f_successive, double time){
	if(typeBufferLen == 0){
		/* �V���ɑł��n�߂�Ƃ� */
		if(time >= lyricsPosition->timeUntil){
			/* �ł��n�߂�ʒu�����Ԑ؂�Ȃ�łĂȂ� */
			return false;
		}
		if(ch != '\0'){
			/* �`�F�b�N���́A�����x���ł\�����l������ */
			if(lyricsPosition->isScoringTarget && time < lyricsPosition->timeJust - SEC_POOR){
				/* �ł��n�߂�ʒu�����̓_�ŁA���ꂪ���傤�ǂ��SEC_POOR�ȏ�O�̂Ƃ� */
				/* �@�i�̕s�̈�̊O�ō̓_���ꂤ�鎞�j�A */
				return false;
			}
		}
	}
	
	vector<Lyrics>::iterator lyricsPos = &m_lyrics[lyricsPosition->lyricsPos()];
	
	typeBuffer[typeBufferLen++] = ch;	/* �ł���������ǉ� */
	/* CheckOnly�i�łĂ���͂����݂��邩���ׂ�j�̂Ƃ��́A\0��ǉ�����B */
	/* �@���̂Ƃ��A���ۂɑł���������1�������邱�Ƃɂ���āA*/
	/* �@�C�ӂ�1��������ɑł����Ƃ��Ď��Ԑ����𔻒�ł��� */
	
	typeBuffer[typeBufferLen] = '\0';
	/* typeBufferLen���u���ۂɑł��������v�Ƃ��ėp���邽�� */
	/* ����������Ă����Ȃ��ƃo�b�t�@�I�[�o�[�������N���� */
	
	bool ret;
	
	if(g_config.f_debugMode){
		ret = (time < lyricsPosition->timeUntil);
		/* �y���̊֐��̑O�̕��Œe����Ȃ������̂Ŏ���ret�͏��true�̂悤�ȁz */
		if(ch == '\0' || !ret){	/* �`�F�b�N�̂Ƃ������Ȃ��Ƃ��͌��ɖ߂��ĕԂ� */
			typeBufferLen--;
		}else{
			/* ����ł��Ă��A�����1���ׂđłĂ����Ƃɂ��� */
			typeBufferLen = 0;
			
			scoreAccuracy(lyricsPos, f_successive, time);
			
			lyricsPosition++;
			vector<Lyrics>::iterator tmpLyricsPos = &m_lyrics[lyricsPosition->lyricsNum];
			scoreTyping(lyricsPos, tmpLyricsPos);
			setTyped(lyricsPos, tmpLyricsPos);
		}
	}else{
		ret = input_1(typeBuffer, typeBufferLen, lyricsPos, f_successive, time, (ch=='\0'), false);
			/* �Ōォ��2�Ԗڂ̓`�F�b�N�݂̂� */
			/* �Ō��false�͍ċA�łȂ����Ƃ������B */
		if(ch != '\0' && ret){	/* ���ۂɑł��A���ꂪ�������ꍇ */
			lyricsPosition = &m_lyricsBlock[lyricsPos->blockNum];
				/* �V�����̎��̈ʒu�� */
			/* typeBuffer���X�V����Ă邩��A�߂��Ȃ� */
		}else{
			typeBufferLen--;	/* typeBufferLen�����ɖ߂� */
		}
	}
	
	return ret;
#if 0
	if(ch == '\0'){
		ret = input_1(typeBuffer, typeBufferLen, lyricsPos, f_successive, time, true, false);
		/* �Ōォ��2�Ԗڂ�true��CheckOnly�ł��邱�Ƃ����� */
		/* �Ō��false�͍ċA�łȂ����Ƃ������B */
		typeBufferLen--;	/* typeBufferLen�����ɖ߂� */
	}else{
		ret = input_1(typeBuffer, typeBufferLen, lyricsPos, f_successive, time, false, false);
		/* �Ōォ��2�Ԗڂ�false�͎��ۂɑł������Ƃ����� */
		/* �Ō��false�͍ċA�łȂ����Ƃ������B */
		if(ret){
			lyricsPosition = &m_lyricsBlock[lyricsPos->blockNum];
				/* �V�����̎��̈ʒu�� */
			/* typeBuffer���X�V����Ă邩��A�߂��Ȃ� */
		}else{
			typeBufferLen--;
		}
	}
	return ret;
#endif
}

bool CTyping::input_1(char *typeBuffer, int &typeBufferLen, vector<Lyrics>::iterator &lyricsPos,
		bool f_successive, double time, bool isCheckOnly, bool isSaiki){
/* isSaiki�͊O����̌Ăяo���ł�false,������̌Ăяo���ł�true */
#if 0
	if(isSaiki && strlen(typeBuffer) == 0){
	/* ���m�胍�[�}�����Ȃ��̂ɍċA���� */
		return true;
	}
#endif
	vector<Lyrics>::iterator tmpLyricsPos = lyricsPos;
	CTrieNode *trie = &m_trie;
	while(1){
		trie = trie->find(tmpLyricsPos->ch);	/* ���{��1�o�C�g�i�� */
		tmpLyricsPos++;
		if(trie == NULL){	/* trie�����ǂ�Ȃ��A�܂�A�܂Ƃ߂đłĂ�͈͂𒴂����B */
			return false;
		}
		for(vector<ConvertData>::iterator convData = trie->m_data.begin();
				convData != trie->m_data.end(); convData++){	/* ���[�}������T�� */
			if(convData->match_front(typeBuffer)){
				int tmpLen = typeBufferLen - convData->m_len;
				/* ����ϊ�����镔���̎��̕�������ɑ΂��Ď��ۂɑł���Ă��镔���̒����B */
				/* ����͕��Ƃ��ɂȂ肤��B */
				if(tmpLen <= 0){	/* ����ɕK�v�Ȃ�L�[�𑫂��Ĉ�x�ɑłĂ�͈͂͂����܂� */
				/* ����́A{��΁c�c,nb�c�c}��n�����ł����󋵂Ȃǂ��܂܂��B */
					if(time >= m_lyricsBlock[(*(tmpLyricsPos-1)).blockNum].timeUntil){
						/* ���̍Ō�̕��������Ԑ؂ꂵ�Ă��Ă͂����Ȃ� */
						continue;
					}
				}
				char tmpTypeBuffer[TYPE_BUFFER_LEN + 1];
				bool flag;
				flag = convData->get_unprocessed(tmpTypeBuffer);
					/* ���m�胍�[�}���ɂȂ�\��̕������擾 */
				//strcpy(tmpTypeBuffer, (*convData).m_str + (*convData).m_len);
				if((!flag)
					|| input_1(tmpTypeBuffer, tmpLen, tmpLyricsPos, f_successive, time, isCheckOnly, true)){
				/* ���m��ɂȂ郍�[�}�����Ȃ��Ƃ���A */
				/* ���݂��Ă��A�ċA�̌��ʂ��ꂪ�łĂ邱�Ƃ����������Ƃ� */
					if(isCheckOnly){	/* ���ׂ邽�߂̂Ƃ��́A�������Ȃ��B */
						return true;
					}
					/* ���ׂ邽�߂ł͂Ȃ��A���ۂɑł��Ă���Ƃ� */
					if(typeBufferLen >= 1){
					/* �V�������߂̑ł��n�� */
						scoreAccuracy(lyricsPos, f_successive, time);
						/* ���傤�ǂ̃^�C�~���O�Ƃ̂��ꂩ��v�Z�������_��^����B */
					}
					
					/* �u���v�̏����A�S�Ă�3�o�C�g�ȏ�ւ̕ϊ��Ɍ��ʂ�����̂Œ��� */
					/* �y �Ă��A����́Aconvert.dat���g�����A�����ŗ�O�������Ȃ��Ă��ςނ悤�ɂ����� �z */
					if(typeBufferLen >= 2 && tmpLyricsPos - lyricsPos > 3){
					/* 2�����ł��āA2�o�C�g��(���{���1������)�������x�ɑłĂ�͈͂ŁA����ɍ̓_�Ώۂł��� */
					/* �܂�A�u�����v�ɂ�����"kk"�̎��_�Łu���v���̓_�����B */
						scoreAccuracy(lyricsPos + 2, f_successive, time);
						/* ���傤�ǂ̃^�C�~���O�Ƃ̂��ꂩ��v�Z�������_��^����B */
					}
					if(convData->match(typeBuffer)){	/* ���S��v */
						/* �ϊ����ꂽ�̎��̕��������_��^���� */
						if(!isSaiki){
						/* �ċA��2�i�ڈȍ~�ł��^�C�s���O�_���v�Z�����2�d�ɂȂ�̂ŁA���ڂ̌Ăяo���̂ݏ��� */
						/* ��F{��,nt} */
							scoreTyping(lyricsPos, tmpLyricsPos);
							setTyped(lyricsPos, tmpLyricsPos);
						}
						lyricsPos = tmpLyricsPos;	/* �ǂ��܂ōs��������߂� */
						strcpy(typeBuffer, tmpTypeBuffer);
						typeBufferLen = tmpLen;
					}
					return true;
				}else{
					/* �ċA�̌��ʂ��߂����� */
					continue;
				}
			}
		}
	}
}

/* ------------------------------------------------------------ */

void CTyping::setTyped(vector<Lyrics>::iterator lyBegin, vector<Lyrics>::iterator lyEnd){
	vector<LyricsBlock>::iterator lyBlockBegin = &m_lyricsBlock[lyBegin->blockNum];
	vector<LyricsBlock>::iterator lyBlockEnd = &m_lyricsBlock[lyEnd->blockNum];
	/* �ł��n�߂���ł��I�����O��Block�͂��ׂđł���� */
	for(vector<LyricsBlock>::iterator itr = lyBlockBegin; itr != lyBlockEnd; itr++){
		itr->nTyped = itr->lyricsLen;
		m_countClear++;
	}
	/* �Ō��Block�́A�ł��I���̉̎�����������ʒu�܂� */
	lyBlockEnd->nTyped = lyEnd->blockPos;
	if(lyBlockEnd->nTyped == lyBlockEnd->lyricsLen){
		m_countClear++;
	}
}

/* ------------------------------------------------------------ */

void CTyping::scoreTyping(vector<Lyrics>::iterator lyBegin, vector<Lyrics>::iterator lyEnd){
/* [lyBegin, lyEnd)��ł����Ƃ��̓��_ */
	bool isJapanese2 = false;
	for(vector<Lyrics>::iterator i = lyBegin; i != lyEnd; i++){
		if(isJapanese2){
			isJapanese2 = false;
		}else{
			m_scoreTyping += SCORE_TYPING;
			if(isJapanese1st((*i).ch)){	/* ���{���1�o�C�g�ڂȂ� */
				isJapanese2 = true;
			}
		}
	}
	m_score = m_scoreTyping + m_scoreAccuracy;
}

void CTyping::scoreAccuracy(vector<Lyrics>::iterator lyricsPos, bool f_successive, double time){
	vector<LyricsBlock>::iterator lyricsPosition;
	lyricsPosition = &m_lyricsBlock[(*lyricsPos).blockNum];
	if(!(*lyricsPosition).isScoringTarget){
	/* �^�C�~���O�̍̓_�Ώۂł͂Ȃ��A�܂��́A�����̓_���ꂽ */
		return;
	}
	
	if(!f_successive){
		m_combo = 0;
	}
	
	double timeDiff = time - (*lyricsPosition).timeJust;
//#ifdef DEBUG_MODE
	double tmp = timeDiff;
//#endif
	int scoreCombo = SCORE_COMBO * m_combo;	/* �R���{���𑝂₷�O�ɃR���{���{�[�i�X���v�Z */
	if(scoreCombo > SCORE_COMBO_MAX){
		scoreCombo = SCORE_COMBO_MAX;
	}
	
	m_combo++;	/* �Ƃ肠�����R���{���� 1 ���₷ */
	if(timeDiff < 0.0){
		timeDiff = -timeDiff;
	}
	int score;
	char buf[256];
	char *strAccuracy;	/* ����̕����i�D�ǉs�j */
	int color;	/* �\���F */
	if(timeDiff < SEC_EXCELLENT){
		score = SCORE_EXCELLENT + scoreCombo;
		m_countExcellent++;
		strAccuracy = "�D";
		color = COLOR_EXCELLENT;
		
		scoreAccuracySub(lyricsPosition, ID_EXCELLENT);
		/*
		(*lyricsPosition).r = 192;
		(*lyricsPosition).g = 192;
		(*lyricsPosition).b = 64;
		*/
	}else if(timeDiff < SEC_GOOD){
		score = SCORE_GOOD + scoreCombo;
		m_countGood++;
		strAccuracy = "��";
		color = COLOR_GOOD;
		
		scoreAccuracySub(lyricsPosition, ID_GOOD);
		/*
		(*lyricsPosition).r = 170;
		(*lyricsPosition).g = 170;
		(*lyricsPosition).b = 85;
		*/
	}else if(timeDiff < SEC_FAIR){
		score = SCORE_FAIR + scoreCombo;
		m_countFair++;
		strAccuracy = "��";
		color = COLOR_FAIR;
		
		scoreAccuracySub(lyricsPosition, ID_FAIR);
		/*
		(*lyricsPosition).r = 128;
		(*lyricsPosition).g = 128;
		(*lyricsPosition).b = 128;
		*/
	}else{
		score = SCORE_POOR;
		m_countPoor++;
		m_combo = 0;	/* �R���{���r�؂�Ă��� */
		strAccuracy = "�s��";
		color = COLOR_POOR;
		
		scoreAccuracySub(lyricsPosition, ID_POOR);
		/*
		(*lyricsPosition).r = 85;
		(*lyricsPosition).g = 85;
		(*lyricsPosition).b = 85;
		*/
	}
	if(m_combo > m_comboMax){	/* �R���{���̍ő�l���X�V */
		m_comboMax = m_combo;
	}
	if(!g_config.f_debugMode){
		if(m_combo >= 10){	/* �R���{��10�𒴂�����R���{����\�� */
			sprintf(buf, "%s %d", strAccuracy, m_combo);
		}else{
			sprintf(buf, "%s", strAccuracy);
		}
	}else{	/* �f�o�b�O�� */
		sprintf(buf, "%+lf", tmp);	/* ���Ԃ̂����\�� */
		if(-SEC_FAIR < tmp && tmp < 0.0){
			//color = GetColor((int)255 * (-tmp/SEC_FAIR), (int)255 * (1 - (-tmp/SEC_FAIR)), 0);
			color = GetColor(255, 0, 0);
		}else if(0.0 <= tmp && tmp < SEC_FAIR){
			//color = GetColor(0, (int)255 * (1 - (tmp/SEC_FAIR)), (int)255 * (tmp/SEC_FAIR));
			color = GetColor(0, 0, 255);
		}
	}
	setText(buf, color);
	
	m_scoreAccuracy += score;	/* ���_���Z */
	m_score = m_scoreTyping + m_scoreAccuracy;
	(*lyricsPosition).isScoringTarget = false;	/* 2��ȏ㓾�_��^�����邱�Ƃ͂Ȃ� */
	(*lyricsPosition).scoringTime = time;	/* ���_������ꂽ�������L�^ */
}

/* lyricsPosition�͂��̂����g����������Ȃ��c�c */
void CTyping::scoreAccuracySub(vector<LyricsBlock>::iterator lyricsPosition, int ID_accuracy){
	if(ID_accuracy <= ID_POOR){
		m_statGauge.inc(ID_accuracy);
	}
	
	int ID_gaugeZone = getGaugeZone();
	switch(ID_accuracy){
	case ID_EXCELLENT:
		switch(ID_gaugeZone){
		case ID_RED_ZONE:
			m_gauge += 200; break;
		case ID_YELLOW_ZONE:
			m_gauge += 200; break;
		case ID_BLUE_ZONE:
			m_gauge += 150; break;
		case ID_CLEAR_ZONE:
			m_gauge += 100; break;
		}
		break;
	case ID_GOOD:
		switch(ID_gaugeZone){
		case ID_RED_ZONE:
			m_gauge += 100; break;
		case ID_YELLOW_ZONE:
			m_gauge += 100; break;
		case ID_BLUE_ZONE:
			m_gauge += 75; break;
		case ID_CLEAR_ZONE:
			m_gauge += 50; break;
		}
		break;
	case ID_FAIR:
		switch(ID_gaugeZone){
		case ID_RED_ZONE:
			m_gauge += 0; break;
		//case ID_YELLOW_ZONE:
		//case ID_BLUE_ZONE:
		//case ID_CLEAR_ZONE:
		//	m_gauge += 0; break;
		}
		break;
	case ID_POOR:
		switch(ID_gaugeZone){
		case ID_RED_ZONE:
			m_gauge -= 50; break;
		case ID_YELLOW_ZONE:
			m_gauge -= 100; break;
		case ID_BLUE_ZONE:
			m_gauge -= 150; break;
		case ID_CLEAR_ZONE:
			m_gauge -= 150 + m_countAll; break;
		}
		break;
	}
	m_gaugeNewCount++;

	if(m_gauge > m_gaugeMax)m_gauge = m_gaugeMax;
	if(m_gauge <= 0){
		m_gauge = 0;
		if(g_config.f_failByGauge){
			m_flagFailed = true;
		}
	}
}

/* ------------------------------------------------------------ */

/*
		red	yellow	blue		clear
Excellent	+200	+200	+150		+100
Good		+100	+100	+ 75		+ 50
Fair		+  0	+  0	+  0		+  0
Poor		- 50	-100	-150		-150-n
Passed		-100	-150	-200		-200-n
��ł��؂�	- 50	-100	-100-n/2	-100-n

25n����X�^�[�g
�l�͈̔͂� 0 - 100n
�\����0-100�A�؂�グ
*/

void CTyping::initGauge(){
	m_gauge = m_countAll * 25;
	m_gaugeMax = m_countAll * GAUGE_COUNT;
	m_gaugeLastCount = 0;
	m_gaugeNewCount = 0;
	m_gaugeLastLost = 0;
}

int CTyping::getGaugeZone(){
	if(m_gauge == 0){
		return ID_FAILED_ZONE;
	}else if(m_gauge <= m_countAll * GAUGE_RED_ZONE){
		return ID_RED_ZONE;
	}else if(m_gauge <= m_countAll * GAUGE_YELLOW_ZONE){
		return ID_YELLOW_ZONE;
	}else if(m_gauge <= m_countAll * GAUGE_CLEAR_ZONE){
		return ID_BLUE_ZONE;
	}else{
		return ID_CLEAR_ZONE;
	}
}

/* �y ���̊֐��͓��_�𓾂�Ƃ��ƁAm_lyricsPosition���ς�����Ƃ����ƂɌĂ΂��K�v������ �z */
/* �Ƃ����̂́A0%�ȉ���100%�ȏ�ɂȂ��ďC�������Ƃ� */
void CTyping::updateGauge(){
	{
		int count = m_lyricsPosition->scoringCount;	/* ����Ă����� */
		
		if(count > m_gaugeLastCount){
			/* count ���i�񂾂�ł������ƒʉ߂𔻒� */
			{	/* ��ł��؂�����_ */
				int lost = count - m_countClear;	/* �ł�����Ȃ������� */
				int t = lost - m_gaugeLastLost;	/* ���񏈗����镪 */
				m_gaugeLastLost = lost;
				while(t-- > 0){
					switch(getGaugeZone()){
					case ID_RED_ZONE:
						m_gauge -= 50; break;
					case ID_YELLOW_ZONE:
						m_gauge -= 100; break;
					case ID_BLUE_ZONE:
						m_gauge -= 100 + (m_countAll/2); break;
					case ID_CLEAR_ZONE:
						m_gauge -= 100 + m_countAll; break;
					}

				}
				/* �ł�����Ȃ��ƌ��_�i�ȉ���Pass�̌��_�Əd������j */
			}
			
			{	/* �ʉ߂����_ */
				int t = count - m_gaugeLastCount;
				m_gaugeLastCount = count;
				
				t -= m_gaugeNewCount;
				if(t < 0){	/* count���i�ޑO�ɑłĂ邱�Ƃ����邪�A����͌�񂵂��� */
					m_gaugeNewCount = -t;
				}else{
					while(t-- > 0){
						switch(getGaugeZone()){
						case ID_RED_ZONE:
							m_gauge -= 100; break;
						case ID_YELLOW_ZONE:
							m_gauge -= 150; break;
						case ID_BLUE_ZONE:
							m_gauge -= 200; break;
						case ID_CLEAR_ZONE:
							m_gauge -= 200 + m_countAll; break;
						}
					}
					m_gaugeNewCount = 0;
				}
			}
		}
	}
	
	if(m_gauge <= 0){
		m_gauge = 0;
		if(g_config.f_failByGauge){
			m_flagFailed = true;
		}
	}
}


int CTyping::getDrawGaugeColor(int pos, int cnt){
	if(pos < GAUGE_RED_ZONE){
		return GetColor(255, 32, 0);
	}else if(pos < GAUGE_YELLOW_ZONE){
		return GetColor(224, 255, 0);
	}else if(pos < GAUGE_CLEAR_ZONE){
		return GetColor(0, 64, 255);
	}else{
		return GetColor(255, 255, 255);
	}
}
/*
int CTyping::getDrawGaugeColor(int pos, int cnt){
	if(pos < 12){
		if(pos < cnt) return GetColor(255, 32, 0);
		else return GetColor(64, 8, 0);
	}else if(pos < 36){
		if(pos < cnt) return GetColor(224, 255, 0);
		else return GetColor(56, 64, 0);
	}else if(pos < 72){
		if(pos < cnt) return GetColor(0, 64, 255);
		else return GetColor(0, 16, 64);
	}else{
		if(pos < cnt) return GetColor(255, 255, 255);
		else return GetColor(64, 64, 64);
	}
}

int CTyping::getDrawGaugeColor(int pos, int cnt){
	cnt--;
	if(pos < 12){
		if(pos == cnt) return GetColor(255, 32, 0);
		else if(pos < cnt) return GetColor(192, 24, 0);
		else return GetColor(64, 8, 0);
	}else if(pos < 36){
		if(pos == cnt) return GetColor(224, 255, 0);
		else if(pos < cnt) return GetColor(168, 192, 0);
		else return GetColor(56, 64, 0);
	}else if(pos < 72){
		if(pos == cnt) return GetColor(0, 64, 255);
		else if(pos < cnt) return GetColor(0, 48, 192);
		else return GetColor(0, 16, 64);
	}else{
		if(pos == cnt) return GetColor(255, 255, 255);
		else if(pos < cnt) return GetColor(192, 192, 192);
		else return GetColor(64, 64, 64);
	}
}
*/
double CTyping::getDrawGaugeAlpha(int pos, int cnt){
	if(pos >= cnt) return 0.25;
	else return 1.0;
#if 0
	cnt--;
	cnt-=pos;	/* �_�����Ă���E�[���牽�Ԗڂ��i0����j */
	if(cnt > 5) return 0.6;
	double tmp = 0.4;
	while(cnt--){
		tmp *= 0.7;
	}
	return 0.6+tmp;
#endif
}

void CTyping::drawGauge(bool isResult){
	//if(m_flagFailed){	/* �y�Q�[�W0�Ŏ��񂾌�ɉ񕜂����肷�邱�Ƃ��Ȃ��悤�ɂ���z */
		//m_gauge = 0;
	//}
	
	int y0, y1;
	
	if(isResult){
		y0 = 355;
	}else{
		y0 = Y_GAUGE;
	}
	y1 = y0 + H_GAUGE;
	
	//int cnt = (GAUGE_COUNT * m_gauge + m_gaugeMax-1)/ m_gaugeMax;
		/* m_gauge / m_gaugeMax �� 0 - GAUGE_COUNT�ɂ���i�؂�グ�j */
		/* GAUGE_COUNT * 100(==GAUGE_COUNT) * m_countAll �́A�����̐���10���ȉ��Ȃ�int�ɓ���@*/
	int cnt = (m_gauge + m_countAll - 1)/ m_countAll;
	
	/* ���Ƃ��ƔZ���D�F�ŕ`�悵�Ȃ��̂� tan �Ȃǂւ̔z�� */
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);	/* �s������1/2 */
	DrawBox(X_GAUGE - W_GAUGE_PADDING, y0 - H_GAUGE_PADDING,
		X_GAUGE + W_GAUGE + W_GAUGE_PADDING, y1 + H_GAUGE_PADDING,
		GetColor(16, 16, 16), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	
	for(int i=0; i<GAUGE_COUNT; i++){
		int x0 = X_GAUGE + W_GAUGE_SEGMENT * i;
		int x1 = x0 + W_GAUGE_SEGMENT;
		int color = getDrawGaugeColor(i, cnt);
		double alpha = getDrawGaugeAlpha(i, cnt);
		if(i<cnt){
			/* ���ӂ�����\�� */

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(48*alpha));	/* �s������3/16�{ */
			/*
			if(i==cnt-1){
				DrawBox(x0-2, y0, x1+2, y1, 
					color, TRUE);
			}
			*/
			DrawBox(x0 +1, y0 -1, x1 -1, y1 +1, 
				color, TRUE);
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255*alpha));
		DrawBox(x0 +1, y0, x1 -1, y1, 
			color, TRUE);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void CTyping::drawElapsedTime(double time){
	time /= m_timeLength;	/* �����ɒ��� */
	if(time > 1.0) time = 1.0;	/* �����Ă�����ł��؂� */
	int pos = (int)(W_TIME*time);
	DrawBox(X_TIME, Y_TIME, X_TIME+pos, Y_TIME+H_TIME,
		COLOR_TIME, TRUE);
	DrawBox(X_TIME+pos -3, Y_TIME, X_TIME+pos+ 3, Y_TIME+H_TIME,
		COLOR_TIME2, TRUE);
}

/* ------------------------------------------------------------ */

void CTyping::drawTypeBuffer(double time){
/*
	m_typeBuffer[m_typeBufferLen] = '\0';
	DrawStringToHandle(X_BUFFER, Y_BUFFER, m_typeBuffer,
		GetColor(255, 255, 255), m_fontHandleBig);
*/
	m_typeBufferEffect.update(m_typeBuffer, m_typeBufferLen, time);	/* �O�̂��߁y�{���͕s�v�z */
	m_typeBufferEffect.draw(X_BUFFER, Y_BUFFER, time);
#if 0
	int x = 0;
	char str[2];
	str[1] = '\0';
	
	/* ���낢��ʓ|�Ȃ̂ŁA�����t�H���g��M�p���邱�Ƃɂ��� */
	str[0] = ' ';
	int w = GetDrawStringWidthToHandle(str, 1, m_fontHandleBig);
	
	{
		int s = m_typeBufferTime.size();
		while(s > m_typeBufferLen){
			s--;
			m_typeBufferTime.pop_front();
		}
	}
	for(int i=0; i<m_typeBufferLen; i++){
		/* i�����ڂ�`�� */
		str[0] = m_typeBuffer[i];
		
		int y = 0;
		double timeDiff = time - m_typeBufferTime[i];
		if(timeDiff < 0.03){	/* 0.03�b��ɂ͒�ʒu */
			//y -= ((0.03 - timeDiff) / 0.03) * 3;	/* 3�����ォ��o�Ă��� */
			x += ((0.03 - timeDiff) / 0.03) * 5;	/* 5�����E����o�Ă��� */
		}
		DrawStringToHandle(X_BUFFER + x, Y_BUFFER + y, str, GetColor(255, 255, 255), m_fontHandleBig);
		
		/* ���̕����̈ʒu�� */
		x += w;
		//x += GetDrawStringWidthToHandle(str, 1, m_fontHandleBig);
	}
#endif
}

/* ------------------------------------------------------------ */

/* true:���^�C�v�̂��̂��擾, false:���ׂĎ擾 */
int CTyping::getBlockStr(vector<LyricsBlock>::iterator itr, char *buf, bool f_getUntyped) const{
	int len = 0;
	for(int j = (f_getUntyped ? itr->nTyped : 0);
			j<(*itr).lyricsLen; j++){	/* �u���b�N���̖��^�C�v�̕�����buf�Ɋi�[ */
		buf[len++] = m_lyrics[(*itr).lyricsNum + j].ch;
	}
	buf[len] = '\0';
	return len;
}

int CTyping::getLyricsBuffer(char *buf) const{
	int len = 0;
	for(vector<LyricsBlock>::iterator i = m_lyricsPosition; i != m_lyricsPositionEnd; i++){
		len += getBlockStr(i, &buf[len], true);
	}
	buf[len] = '\0';
	return len;
}

/* ------------------------------------------------------------ */

/* timeDiff: ���傤�ǂ̈ʒu�̉��b�ォ */
int CTyping::getDrawPosX(double timeDiff) const{
	/* CIRCLE_SPEED * m_challenge.speed() �������A�i�������A�������j */
	return X_CIRCLE + (int)(-timeDiff * (CIRCLE_SPEED * m_challenge.speed()));
}

int CTyping::getDrawPosY(int x) const{
	x -= X_CIRCLE;	/* ��ʂ�0�����W��0�Ƃ��킹�� */
	double y = 0;
	if(m_challenge.test(CHALLENGE_SIN)){
		y += sin(x / SCALE_FUNCTION) * SCALE_FUNCTION;
	}
	if(m_challenge.test(CHALLENGE_COS)){
		y += cos(x / SCALE_FUNCTION) * SCALE_FUNCTION;
	}
	if(m_challenge.test(CHALLENGE_TAN)){
		y += tan(x / SCALE_FUNCTION) * SCALE_FUNCTION;
	}
	return (int)-y;	/* �X�N���[�����W�͏㉺���t*/
}

/* ------------------------------------------------------------ */

void CTyping::draw(){
	if(m_phase >= PHASE_RESULT){	/* �X�R�A�\���ȍ~ */
		drawResult();
		//drawFlagAbort();
		return;
	}
	double time;
	if(m_phase == PHASE_READY){
		if(m_soundHandleMusic == -1){
			DrawStringToHandle(50, 370, "Now loading...", GetColor(255, 255, 255), m_fontHandleBig);
		}else{
			DrawStringToHandle(50, 370, "Press any key to start.", GetColor(255, 255, 255), m_fontHandleBig);
		}
		time = 0.0;	/* �n�܂�O��0�b�Ŏ~�܂��Ă��� */
		if(g_config.f_debugMode && g_config.debugTime > 0){	/* �r������n�߂�f�o�b�O���[�h�̂Ƃ� */
			time = g_config.debugTime / 1000.0;
		}
	}else{
		time = getTime();	/* �J�n��������̌o�ߕb���擾 */
	}
	
	/*{
		char buf[256];
		sprintf(buf, "%4d �R���{", m_combo);
		int width = GetDrawStringWidthToHandle(buf, strlen(buf), m_fontHandleNormal);
		DrawStringToHandle(X_INFO - width, Y_INFO2, buf,
			GetColor(255, 255, 255), m_fontHandleNormal);
	}*/
	/*
	DrawFormatStringToHandle(10, Y_INFO, GetColor(255, 255, 255), m_fontHandleNormal,
		"%10d �_", 10*m_scoreDraw);
	DrawFormatStringToHandle(10, Y_INFO2, GetColor(255, 255, 255), m_fontHandleNormal,
		"%10d �R���{", m_combo);
	*/
	
	/* ���ߐ��A������\�� */
	while((*m_beatLineDrawLeft).time != INFTY){
		double timeDiff = time - (*m_beatLineDrawLeft).time;
		int posX = getDrawPosX(timeDiff);
		if(posX >= 0){
			/* ��ʂ���o�Ă��Ȃ� */
			break;
		}
		/* ���łɉ�ʂ���o�Ă��܂����̂ŁA�`��Ώۂ���폜 */
		m_beatLineDrawLeft++;
	}
	while((*m_beatLineDrawRight).time != INFTY){
		double timeDiff = time - (*m_beatLineDrawRight).time;
		int posX = getDrawPosX(timeDiff);
		if(posX >= W_WINDOW){
			/* ��ʂɓ����Ă��Ă��Ȃ� */
			break;
		}
		/* �V������ʂɓ����Ă����̂ŁA�`��Ώۂɒǉ� */
		m_beatLineDrawRight++;
	}
	for(vector<BeatLine>::iterator i = m_beatLineDrawLeft; i != m_beatLineDrawRight; i++){
		double timeDiff = time - (*i).time;	/* �ʂ�߂���ʒu�ɂȂ��Ă��牽�b�ォ */
		int posX = getDrawPosX(timeDiff);
		int posY = getDrawPosY(posX);
		if((*i).type == BEATLINE_BAR){
			DrawLine(posX, Y0_BAR  + posY, posX, Y1_BAR  + posY, GetColor(255, 255, 255));
		}else{	/* == BEATLINE_BEAT */
			DrawLine(posX, Y0_BEAT + posY, posX, Y1_BEAT + posY, GetColor(128, 128, 128));
		}
	}
	
	if(!m_challenge.test(CHALLENGE_STEALTH)){	/* Stealth�Ȃ�\�����Ȃ� */
		/* �~�ƁA�~�̉��̉̎���\�� */
		
		int xMin = 0, xMax = W_WINDOW;	/* �iHidden, Sudden �̂��߂́j�`�����[�ƉE�[ */
		if(m_challenge.test(CHALLENGE_HIDDEN)){
			xMin = X_HIDDEN;
		}
		if(m_challenge.test(CHALLENGE_SUDDEN)){
			xMax = X_SUDDEN;
		}
		SetDrawArea(xMin, 0, xMax, H_WINDOW);	/* �`��͈͂𐧌� */
		
		while((*m_lyricsBlockDrawLeft).timeJust != INFTY){
			double timeDiff = time - (*m_lyricsBlockDrawLeft).timeJust;
			int posX = getDrawPosX(timeDiff);
			if(posX >= -(R_CIRCLE + 300)){
				/* �~���͂ݏo��̎���A�ł��؂��Ē��˂邱�Ƃ��l�����čL�߂Ɏ�� */
				/* ��ʂ���o�Ă��Ȃ� */
				break;
			}
			/* ���łɉ�ʂ���o�Ă��܂����̂ŁA�`��Ώۂ���폜 */
			m_lyricsBlockDrawLeft++;
		}
		
		while((*m_lyricsBlockDrawRight).timeJust != INFTY){
			double timeDiff = time - (*m_lyricsBlockDrawRight).timeJust;
			int posX = getDrawPosX(timeDiff);
			if(posX >= W_WINDOW + R_CIRCLE){
				/* ��ʂɓ����Ă��Ă��Ȃ� */
				break;
			}
			/* �V������ʂɓ����Ă����̂ŁA�`��Ώۂɒǉ� */
			m_lyricsBlockDrawRight++;
		}
		
		for(vector<LyricsBlock>::iterator i = m_lyricsBlockDrawRight; i != m_lyricsBlockDrawLeft;){
			i--;	/* [Left, Right)�Ȃ̂ŁA�����Ńf�N�������g */
			double timeDiff = time - (*i).timeJust;	/* ���傤�Ǒłʒu�ɂȂ��Ă��牽�b�ォ */
			int posX = getDrawPosX(timeDiff);
			int posY = getDrawPosY(posX);
			
			if(!(*i).isValid){	/* �̎��̐؂�� */
				if(timeDiff < 0.0){	/* �؂�ڂ͔���ʒu�ɗ�����O�̂Ƃ������\�� */
					double alpha = 1.0;
					if((*i).clearedTime >= 0.0){	/* �؂�ڂ������Ă���Ƃ� */
						double t = ((*i).timeJust - time) / ((*i).timeJust - (*i).clearedTime);
						alpha = t*t*t;
					}else if(i < m_lyricsPosition){	/* �؂�ڂ������ꂸ�Ɏ��ɐi�񂾏ꍇ */
					/* �܂�A�ł��؂炸�ɁA�V���������𑁂߂ɑł��n�߂��ꍇ�i�Ɍ��邩�ȁH�j */
						continue;	/* �\�����Ȃ� */
					}
					/* �D�F�̉~�𗬂� */
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 51 * alpha);	/* alpha * 1/5�̕s�����x�ŕ`�� */
					DrawCircle(posX, Y_CIRCLE + posY, R_CIRCLE - 1, GetColor(85, 85, 85), TRUE);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * alpha);
					DrawCircle(posX, Y_CIRCLE + posY, R_CIRCLE, GetColor(85, 85, 85), FALSE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
				continue;
			}
			
			char buf[256];
			int len = getBlockStr(i, buf, true);	/* �u���b�N���̖��^�C�v�̕�����buf�Ɋi�[ */
			if(len == 0){	/* ���ׂă^�C�v����Ă��� */
				if((*i).clearedTime < 0){
					/* ������ۑ� */
					(*i).clearedTime = time;
					/* �ʒu��ۑ� */
					(*i).x = posX;
					(*i).y = posY;
					double arg = (GetRand(32) - 16) / 20.0;
					/* -0.8�ȏ�0.8�ȉ�0.05���݂Ń����_�� */
					double arg2 = GetRand(354) / 113.0;
					/* 0�`�΂��炢�܂ł̃����_���i355/113���΁j */
					(*i).vx = 100.0 * sin(arg) * sin(arg2);
					(*i).vy = 100.0 * cos(arg);
					(*i).vz = 0.7 * sin(arg) * cos(arg2);
				}
				// continue;
			}
			
			int Color;
			if((*i).isScoringTarget){	/* �܂��^�C�~���O�_��������ĂȂ� */
				Color = GetColor(255, 0, 0);
			}else{
				double sTime = (time - (*i).scoringTime) / 0.20;
				if(sTime < 1.0){
					if(sTime < 0.0){
						sTime = 0.0;	/* �������������ꂤ��̂ŔO�̂��� */
					}
					
					Color = GetColor((int)(255*0.4*(1.0+sTime)*(1.0-sTime)),
						(int)(255*0.4*(1.0-sTime)*(1.0-sTime)),
						(int)(255*0.4*(1.0+1.5*sTime)));
					
				}else{
					Color = GetColor(0, 0, 255);
				}
			}
			{
				int rCircle = R_CIRCLE;
				if((*i).clearedTime >= 0.0){
					double fTime = (time - (*i).clearedTime)/0.25;
					if(fTime >= 1.0){	/* �ł��؂��Ă���\�����Ԃ������� */
						continue;
					}
					if(fTime < 0.0){
						fTime = 0.0;	/* �������������ꂤ��̂ŔO�̂��� */
					}
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * 0.3 * (1.0 - fTime*fTime));
					/* �ŏ����炠����x�������A����ɏ��X�ɔ������� */
					posX = (int)((*i).vx * fTime) + (*i).x;
					posY = (int)(((125.0 * fTime) - (*i).vy) * fTime) + (*i).y;
					/* �����x(vx,vy)�A�����x����2*125.0�ňʒu�v�Z */
					rCircle = R_CIRCLE / (1.0 + (*i).vz * fTime);
				}
				DrawCircle(posX, Y_CIRCLE + posY, rCircle - 1, Color, TRUE);	/* �����~ */
				DrawCircle(posX, Y_CIRCLE + posY, rCircle, GetColor(0, 0, 0), FALSE);	/* �����~�̗֊s */
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			
			/* �~�̉��̉̎� */
			if(!m_challenge.test(CHALLENGE_LYRICS_STEALTH)){	/* LyricsStealth�Ȃ�\�����Ȃ� */
				int strWidth = GetDrawStringWidthToHandle(buf, len, m_fontHandleNormal);
				int posXLeft = posX - strWidth / 2;	/* ���S���~�̒��S�� */
				if(posXLeft < posX - R_CIRCLE){	/* �~�̍��[���o�Ă�Ȃ� */
					posXLeft = posX - R_CIRCLE;	/* �~�̍��[�ɂ��킹�� */
				}
				DrawStringToHandle(posXLeft, Y_LYRICS + posY, buf,
					GetColor(255, 255, 255), m_fontHandleNormal);	/* �����~�ɏ�����镶�� */
			}
		}
		SetDrawArea(0, 0, W_WINDOW, H_WINDOW);	/* �`��͈͂����ɖ߂� */
	}
	
	DrawCircle(X_CIRCLE, Y_CIRCLE, R_CIRCLE, GetColor(255, 255, 255), FALSE);	/* ����ʒu�̉~ */
	
	{	/* ���b�Z�[�W�\�� */
		double timeDiff = time - m_textTime;
		if(timeDiff <= (0.5+0.1)){	/* ���b�Z�[�W�i�D�ǉs�j��0.5�b�ԕ\������� */
			/* 0.1�b�c�����c�� */
			//int strLen = strlen(m_text);
			//int strWidth = GetDrawStringWidthToHandle(m_text, strLen, m_fontHandleBig);
			int strX;
			//strX = 100 - strWidth / 2;
			strX = X_ACCURACY;
			int strY;	/* �\�������Y���W */
			strY = Y_ACCURACY;
			if(timeDiff < 0.05){	/* 0.05�b��ɂ͒�ʒu */
				strY += ((0.05 - timeDiff) / 0.05) * 10;	/* 10����������o�Ă��� */
			}
			if(timeDiff > 0.5){
				double t = (timeDiff - 0.5) / 0.1;
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255 * (1.0 - t)));
			}
			DrawStringToHandle(strX, strY, m_text,
				m_textColor, m_fontHandleBig);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}
	
	if(!m_challenge.test(CHALLENGE_LYRICS_STEALTH)){	/* LyricsStealth ���ƕ\������Ȃ� */
		/* �\������̎����o�� */
		while(time >= (*m_lyricsKanjiPosition).timeEnd){	/* �\���I�����Ă���Ȃ玟�ɐi�� */
			m_lyricsKanjiPosition++;
		}
		static int strWidth = -1;
		static char *strNext = "Next: ";
		if(strWidth < 0){
			strWidth = GetDrawStringWidthToHandle(strNext, strlen(strNext), m_fontHandleNormal);
		}
		DrawStringToHandle(X_LYRICS_KANJI - strWidth, Y_LYRICS_KANJI_NEXT, strNext,
			GetColor(255, 255, 255), m_fontHandleNormal);
		if(time >= (*m_lyricsKanjiPosition).timeBegin){	/* �\���J�n���Ă���Ȃ� */
			(*m_lyricsKanjiPosition).draw(
				X_LYRICS_KANJI, Y_LYRICS_KANJI, m_fontHandleNormal);	/* �o�� */
			(*(m_lyricsKanjiPosition + 1)).draw(
				X_LYRICS_KANJI, Y_LYRICS_KANJI_NEXT, m_fontHandleNormal);	/* Next���o�� */
		}else{
			(*m_lyricsKanjiPosition).draw(
				X_LYRICS_KANJI, Y_LYRICS_KANJI_NEXT, m_fontHandleNormal);	/* Next���o�� */
		}
	}
	
	if(!m_challenge.test(CHALLENGE_LYRICS_STEALTH)){	/* LyricsStealth ���ƕ\������Ȃ� */
		updateLyricsBuffer(time);
		
		/* ���ܑΏۂɂȂ��Ă��郍�[�}���̎�(m_lyricsPosition����m_lyricsPositionEnd�܂�)��\�� */
		/* Hidden �� Stealth �̂Ƃ��A�q���g�ɂȂ�̂�����邽�߁A */
		/* ������ł܂ł͗�O�I�ɕ\������Ȃ� */
		/* �yupdateLyricsBuffer��PositionEnd�������Ǘ����邱�ƂɂȂ����z */
#if 0
		if(!( (m_challenge.test(CHALLENGE_HIDDEN) || m_challenge.test(CHALLENGE_STEALTH))
				&& m_typeBufferLen == 0 )){
#endif
			m_lyricsBufferEffect.draw(X_LYRICS_BIG, Y_LYRICS_BIG, time);
			/*
			char buf[256];
			getLyricsBuffer(buf);
			DrawStringToHandle(X_LYRICS_BIG, Y_LYRICS_BIG, buf,
				GetColor(255, 255, 255), m_fontHandleBig);
			//*/
#if 0
		}
#endif
	}
	
	/* �^�C�v����������\�� */
	drawTypeBuffer(time);
	
	drawGauge(false);

	drawElapsedTime(time);
	
	m_statGauge.draw();
	
	m_scoreDraw = m_score/10 - (int)(0.7 * (m_score/10 - m_scoreDraw));
	{
		char buf[256];
		sprintf(buf, "%8d", 10*m_scoreDraw);	// "%8d �_"
		int width = GetDrawStringWidthToHandle(buf, strlen(buf), m_fontHandleBig);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64);	/* 1/4�̕s�������ō��ŕ`�� */
		DrawBox(X_SCORE - width - 4, Y_SCORE - 4, X_SCORE + 4, Y_SCORE + 36 + 4,
			GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawStringToHandle(X_SCORE - width, Y_SCORE, buf,
			GetColor(255, 255, 255), m_fontHandleBig);
	}
	
	{
		char buf[256];
		sprintf(buf, "�ő� %4d �R���{", m_comboMax);
		int width = GetDrawStringWidthToHandle(buf, strlen(buf), m_fontHandleNormal);
		DrawStringToHandle(X_INFO - width, Y_INFO, buf,
			GetColor(255, 255, 255), m_fontHandleNormal);
	}
	
	m_effect1.draw(time);	/* �L�[���̓G�t�F�N�g */
	
	if(m_phase == PHASE_FADEOUT){	/* �t�F�[�h�A�E�g */
		double xTime = (time - m_timeFinished) / SEC_FADEOUT;
		if(xTime >= 1.0){
			xTime = 1.0;
		}
		if(m_flagFailed){
			/* �㉺���炨�낷 */
			double t = xTime * 1.5;	/* ��������1.5�{�̑����Ŋ��������� */
			t *= 1.5;
			if(t >= 1.0){
				t = 1.0;
			}
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 224);
			double tmp = t * (1 + 3.0 * t * (t - 1.0));
			int h = H_WINDOW * 0.5 * tmp;
			DrawBox(0, 0, W_WINDOW, h, GetColor(64, 64, 64), TRUE);
			DrawBox(0, H_WINDOW - h, W_WINDOW, H_WINDOW, GetColor(64, 64, 64), TRUE);
		}
		
		{
			double tmp = ((-2.0*xTime)+3.0)*xTime*xTime;
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * tmp);
			DrawBox(0, 0, W_WINDOW, H_WINDOW, GetColor(0, 0, 0), TRUE);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	
	drawFlagAbort();
}

void CTyping::drawResult(){
	double time = getTime();
	DrawFormatStringToHandle(30, 10, GetColor(255, 255, 255), m_fontHandleBig,
		"���� :");
	if(time >= 0.6){
		DrawFormatStringToHandle(320, 15, COLOR_EXCELLENT, m_fontHandleNormal,
			"�@�D : %4d / %d", m_countExcellent, m_countAll);
	}
	if(time >= 0.7){
		DrawFormatStringToHandle(320, 40, COLOR_GOOD, m_fontHandleNormal,
			"�@�� : %4d / %d", m_countGood, m_countAll);
	}
	if(time >= 0.8){
		DrawFormatStringToHandle(320, 65, COLOR_FAIR, m_fontHandleNormal,
			"�@�� : %4d / %d", m_countFair, m_countAll);
	}
	if(time >= 0.9){
		DrawFormatStringToHandle(320, 90, COLOR_POOR, m_fontHandleNormal,
			"�s�� : %4d / %d", m_countPoor, m_countAll);
	}
	if(time >= 1.0){
		DrawFormatStringToHandle(320, 115, GetColor(255, 255, 255), m_fontHandleNormal,
			"�ʉ� : %4d / %d", m_countPass, m_countAll);
	}
	if(time >= 1.8){
		if(m_comboMax >= 0){
			DrawFormatStringToHandle(30, 160, GetColor(255, 255, 255), m_fontHandleBig,
				"�ő� %d �R���{", m_comboMax);
		}else{
			DrawFormatStringToHandle(30, 160, GetColor(255, 255, 0), m_fontHandleBig,
				"�t���R���{");
		}
	}
	if(time >= 2.6){
		DrawFormatStringToHandle(30, 225, GetColor(255, 255, 255), m_fontHandleBig,
			"���_ :");
	}
	if(time >= 2.9){
		DrawFormatStringToHandle(320, 225, GetColor(255, 255, 255), m_fontHandleNormal,
			"�� : %7d �_", m_scoreAccuracy);
	}
	if(time >= 3.2){
		DrawFormatStringToHandle(320, 250, GetColor(255, 255, 255), m_fontHandleNormal,
			"�� : %7d �_", m_scoreTyping);
	}
	if(time >= 4.0){
		DrawFormatStringToHandle(30, 295, GetColor(255, 255, 255), m_fontHandleBig,
			"�����_ : %8d �_", m_score);
		
		drawGauge(true);
	}
	if(!(m_challenge.isEasy() || m_isReplay || g_config.f_debugMode)){	/* �ʏ펞�̂� */
		/* �ȒP�ɂȂ�I�v�V�������g�p�����ꍇ�⃊�v���C����f�o�b�O���@�łȂ���΁A */
		/* �����L���O�ɂ���邽�߁A���O����́B */
		
		DrawStringToHandle(60, 410, m_typeBuffer, GetColor(255, 255, 255), m_fontHandleBig);
		/* ���͂��ꂽ���O��\�� */
		if(time >= 4.0){
			/* ���O���ݒ肳��ĂȂ��ꍇ�A��莞�Ԃ��o��������͊J�n */
			if(m_phase < PHASE_FINISHED){	/* ���O���m�肵���̂��͕\�����Ȃ� */
				DrawFormatStringToHandle(30, 395, GetColor(255, 255, 255), m_fontHandleNormal,
					"���O����͂��Ă������� :");
				if(m_flagLockName){
					DrawFormatStringToHandle(280, 395, GetColor(64, 64, 85), m_fontHandleNormal,
						"�i Esc: ���O�ύX �j");
				}
			}
		}
	}
	if(m_phase == PHASE_FINISHED){
		if(m_rank >= 0){
			int strWidth;
			strWidth = GetDrawFormatStringWidthToHandle( m_fontHandleBig,
				"%d �ʂɃ����N�C�����܂���", m_rank + 1);
			int boxWidth = strWidth + 80;
			if(boxWidth > W_WINDOW){
				boxWidth = W_WINDOW;
			}
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 170);	/* �i1/3�́j�������ŕ`�� */
			DrawBox(320 - boxWidth / 2, 200, 320 + boxWidth/2, 280, GetColor(170, 170, 170), TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 192);	/* �i1/4�́j�������ŕ`�� */
			DrawFormatStringToHandle(320 - strWidth / 2, 240 - 18,
				GetColor(0, 0, 0), m_fontHandleBig,
				"%d �ʂɃ����N�C�����܂���", m_rank + 1);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);	/* �u�����h���Ȃ��悤�ɖ߂� */
		}
	}
	return;
}

void CTyping::drawFlagAbort(){
	if(m_flagAbort){
		const char *str = "������xEsc�������ƒ��f���܂�";
		int strWidth;
		strWidth = GetDrawStringWidthToHandle(str, strlen(str), m_fontHandleBig);
		int boxWidth = strWidth + 80;
		if(boxWidth > W_WINDOW){
			boxWidth = W_WINDOW;
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 170);	/* �i1/3�́j�������ŕ`�� */
		DrawBox(320 - boxWidth / 2, 200, 320 + boxWidth/2, 280, GetColor(170, 170, 170), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 192);	/* �i1/4�́j�������ŕ`�� */
		DrawStringToHandle(320 - strWidth / 2, 240 - 18,
			str, GetColor(0, 0, 0), m_fontHandleBig);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);	/* �u�����h���Ȃ��悤�ɖ߂� */
	}
}

/* ------------------------------------------------------------ */

void CTyping::mainLoop(){
	m_isReplay = false;
	m_replayData.clear();
	
	//ClearDrawScreen();
	while(1){
g_check.rap(GetColor(16, 0, 0));
		draw();
		myScreenFlip();
		double timeCountTmp = myGetNowHiPerformanceCount();	/* ������ۑ����Ă��� */
		
g_check.rap(GetColor(16, 16, 0));
	//ProcessStreamSoundMemAll();
		//ClearDrawScreen();
		if(ProcessMessage() == -1){
			break;
		}
g_check.rap(GetColor(0, 0, 16));
		while(1){
			double timeCount;
			char ch = GetKeyboardInput(timeCount);
			if(ch == 0){	/* �o�b�t�@����ɂȂ�΃L�[���͏����I�� */
				break;
			}
			//if(ch == CTRL_CODE_ESC){	/* Esc�ŃQ�[���𒆒f */
			//	goto L1;
			//}
			keyboard(ch, timeCount);
		}
		
g_check.rap(GetColor(0, 16, 0));
		if(!idle(timeCountTmp)){
			break;
		}
	}
//L1:
	return;
}

void CTyping::replayLoop(){
	m_isReplay = true;
	
	vector<ReplayData>::iterator itrReplay = m_replayData.begin();
	
	//ClearDrawScreen();
	while(1){
g_check.rap(GetColor(16, 0, 0));
		draw();
		myScreenFlip();
		
		double timeCountTmp = myGetNowHiPerformanceCount();	/* ������ۑ����Ă��� */
		
g_check.rap(GetColor(16, 16, 0));
		if(ProcessMessage() == -1){
			break;
		}
		//ClearDrawScreen();
		
g_check.rap(GetColor(0, 0, 16));
		while(1){
			double timeCount;
			char ch = GetKeyboardInput(timeCount);
			if(ch == 0){	/* �o�b�t�@����ɂȂ�΃L�[���͏����I�� */
				break;
			}
			if(ch == CTRL_CODE_ESC){	/* Esc�ŃQ�[���𒆒f */
				goto L1;
			}
			/* ���v���C�̎��́A�Q�[�����C�����A�L�[���͂�Esc�ȊO���� */
			if(m_phase != PHASE_MAIN){
				keyboard(ch, timeCount);
			}
		}
		
		if(m_phase == PHASE_MAIN){
			double time = getTime(timeCountTmp);
			while(itrReplay != m_replayData.end() && time > (*itrReplay).time){
				keyboard_1((*itrReplay).ch, (*itrReplay).time);
				itrReplay++;
			}
		}
		
g_check.rap(GetColor(0, 16, 0));
		if(!idle(timeCountTmp)){
			break;
		}
	}
L1:
	return;
}

/* ------------------------------------------------------------ */

bool CTyping::saveReplay(const char *fileName, bool &f_confirm){
/* f_confirm = false: �㏑���m�F���܂�, true: �㏑���m�F�����ς� */
	FILE *fp;
	if(g_config.overwriteReplay != CONFIG_ALWAYS && !f_confirm){
		/* ��ɏ㏑����A�㏑���m�F���m�F�ς݂̏ꍇ�͂��̑���͕s�v */
		/* �t�@�C�������݂��邩�`�F�b�N���� */
		fp = fopen(fileName, "rb");
		if(fp != NULL){	/* ���݂��� */
			fclose(fp);
			if(g_config.overwriteReplay == CONFIG_QUERY){	/* �m�F����ݒ�Ȃ� */
				/* �m�F���邱�Ƃɂ��� */
				f_confirm = true;
			}
			/* �ǂ���ɂ���i�m�F����Ƃ��Ă��ЂƂ܂��j���s */
			return false;
		}
	}
	f_confirm = false;	/* �߂��Ă��� */
	fp = fopen(fileName, "wb");
	if(fp == NULL){
		return false;
	}
	for(vector<ReplayData>::iterator itr = m_replayData.begin(); itr != m_replayData.end(); itr++){
		fwrite(&(*itr).ch, sizeof(char), 1, fp);
		fwrite(&(*itr).time, sizeof(double), 1, fp);
	}
	fclose(fp);
	return true;
}

bool CTyping::loadReplay(const char *fileName){
	FILE *fp;
	fp = fopen(fileName, "rb");
	if(fp == NULL){
		return false;
	}
	m_replayData.clear();	/* �ǂݍ��ޑO�Ɍ��ݓ����Ă���̂����� */
	while(1){
		ReplayData rd;
		if(fread(&rd.ch, sizeof(char), 1, fp) < 1){
			break;
		}
		if(fread(&rd.time, sizeof(double), 1, fp) < 1){
			break;
		}
		m_replayData.push_back(rd);
	}
	fclose(fp);
	return true;
}

/* ------------------------------------------------------------ */

bool CTyping::saveReplayLoop(){
	char message[256];
	strcpy(message, "");
	bool isSaved = false;
	
	bool f_confirm = false;	/* ���ݏ㏑���m�F�𕷂��Ă���Ƃ���ł��邩 */
	
	char buf[256];
	strcpy(buf, "");
	int len = 0;
	while(1){
		if(ProcessMessage() == -1){
			return false;
		}
		//ClearDrawScreen();
		
		DrawStringToHandle(10, 10, "save replay as", GetColor(255, 255, 255), m_fontHandleBig);
		if(len != 0){
			DrawStringToHandle(10, 50, buf, GetColor(255, 255, 255), m_fontHandleBig);
		}else{
			int color = GetColor(128, 128, 128);
			if(isSaved){	/* �Z�[�u��Ȃ瓯������ */
				color = GetColor(255, 255, 255);
			}
			DrawStringToHandle(10, 50, g_config.defaultReplayFile, color, m_fontHandleBig);
		}
		DrawStringToHandle(10, 90, message, GetColor(255, 255, 255), m_fontHandleBig);
		
		myScreenFlip();
		while(1){
			char ch = GetKeyboardInput();
			if(ch == 0){	/* �L�[���͏����I�� */
				break;
			}
			if(isSaved){	/* �Z�[�u��A������������߂� */
				return true;
			}
			strcpy(message, "");	/* �����ł����烁�b�Z�[�W�͏��� */
			if(f_confirm){
				switch(ch){
				case CTRL_CODE_ESC:	/* �����������f */
					return false;
				case 'y':
				case 'Y':
					/* yes�Ȃ�f_confirm=true�̌��ŃZ�[�u */
					isSaved = saveReplay((len != 0) ? buf : g_config.defaultReplayFile, f_confirm);
					if(isSaved){
						strcpy(message, "saved.");
					}else{
						strcpy(message, "error.");
					}
					break;
				case 'n':
				case 'N':
					/* no�Ȃ�f_confirm���������A���b�Z�[�W���Ȃ��ɖ߂� */
					f_confirm = false;
					strcpy(message, "");
					break;
				}
			}else{
				switch(editBuffer(ch, buf, len)){	/* �o�b�t�@�𑀍� */
				case EDIT_BUFFER_OK:
					isSaved = saveReplay((len != 0) ? buf : g_config.defaultReplayFile, f_confirm);
					if(isSaved){
						strcpy(message, "saved.");
					}else if(f_confirm){
						strcpy(message, "already exists. overwrite? [y/n]");
					}else{
						strcpy(message, "error.");
					}
					break;
				case EDIT_BUFFER_CANCEL:
					return false;
				}
			}
		}
	}
}

bool CTyping::loadReplayLoop(){
	char message[256];
	strcpy(message, "");
	
	bool isLoaded = false;
	
	char buf[256];
	strcpy(buf, "");
	int len = 0;
	while(1){
		if(ProcessMessage() == -1){
			return false;
		}
		//ClearDrawScreen();
		
		DrawStringToHandle(10, 10, "find replay file", GetColor(255, 255, 255), m_fontHandleBig);
		if(len != 0){
			DrawStringToHandle(10, 50, buf, GetColor(255, 255, 255), m_fontHandleBig);
		}else{
			int color = GetColor(128, 128, 128);
			if(isLoaded){	/* �Z�[�u��Ȃ瓯������ */
				color = GetColor(255, 255, 255);
			}
			DrawStringToHandle(10, 50, g_config.defaultReplayFile, color, m_fontHandleBig);
		}
		DrawStringToHandle(10, 90, message, GetColor(255, 255, 255), m_fontHandleBig);
		
		myScreenFlip();
		while(1){
			char ch = GetKeyboardInput();
			if(ch == 0){	/* �L�[���͏����I�� */
				break;
			}
			if(isLoaded){	/* ���[�h��A������������߂� */
				return true;
			}
			strcpy(message, "");	/* �����ł����烁�b�Z�[�W�͏��� */
			
			switch(editBuffer(ch, buf, len)){	/* �o�b�t�@�𑀍� */
			case EDIT_BUFFER_OK:
				isLoaded = loadReplay((len != 0) ? buf : g_config.defaultReplayFile);
				if(isLoaded){
					strcpy(message, "loaded.");
				}else{
					strcpy(message, "error.");
				}
				break;
			case EDIT_BUFFER_CANCEL:
				return false;
			}
		}
	}
}

/* ============================================================ */

#define COMMENT_N_LINES 12

struct MusicInfo{
	char m_title[256];
	char m_artist[256];
	char m_fumenAuthor[256];
	int m_level;
	char m_fumenFileName[256];
	char m_rankingFileName[256];
	char m_comment[COMMENT_N_LINES][256];
	
	int m_num;
	int m_numAll;
public:
	MusicInfo();
	MusicInfo(const MusicInfo &info);
	~MusicInfo();
	
	void load(CTyping &typing);
	
	void readRanking();
	
	void draw(int y, int brightness);
	void drawComment(int x, int y);
	void drawPlayData(int x, int y);
	void drawRanking(int x, int y, int rankBegin, int rankLen);
	
	bool titleCmp(const char *buf);
	//void renewFont();
private:
	void createFont();
	void deleteFont();
private:
	CRanking m_ranking;
	static int m_count;	/* ����Ă���MusicInfo�̌� */
	static int m_fontHandleNormal;
	static int m_fontHandleTitle;
	static int m_fontHandleRanking;
	static int m_fontHandleAchievement;
};

int MusicInfo::m_count = 0;
int MusicInfo::m_fontHandleNormal;
int MusicInfo::m_fontHandleTitle;
int MusicInfo::m_fontHandleRanking;
int MusicInfo::m_fontHandleAchievement;

MusicInfo::MusicInfo(){
//printfDx("%d++ ", m_count);
	if(m_count++ == 0){
		createFont();
	}
}

MusicInfo::MusicInfo(const MusicInfo &info){
	*this = info;
//printfDx("%d++ ", m_count);
	if(m_count++ == 0){
		createFont();
	}
}

MusicInfo::~MusicInfo(){
	if(--m_count == 0){
		deleteFont();
	}
//printfDx("--%d ", m_count);
}

void MusicInfo::load(CTyping &typing){
	typing.load(m_fumenFileName, m_rankingFileName);
}

/* �����L���O��ǂݍ��� */
void MusicInfo::readRanking(){
	m_ranking.open(m_rankingFileName);
	m_ranking.read();
	m_ranking.close();
}

#define X_TITLE 40
#define Y_TITLE (30 - 30/2)
#define X_NUM (W_WINDOW - 25)
#define Y_NUM (60-44)
#define Y_ARTIST Y_NUM
#define X_LEVEL (W_WINDOW - 270)
#define Y_LEVEL (60-22)
#define X_F_AUTHOR (W_WINDOW - 30)
#define Y_F_AUTHOR Y_LEVEL

#define X_ACHIEVEMENT (X_TITLE - 25)
#define Y_ACHIEVEMENT (30 - 24/2)

void MusicInfo::draw(int y, int brightness){	/* �ȏ���y���獂��60�ŕ`�� */
	int color = GetColor(brightness, brightness, brightness);
	/* �^�C�g���̒����𑪂��Ă��� */
	int widthTitle = GetDrawStringWidthToHandle(m_title, strlen(m_title), m_fontHandleTitle);
	
	/* �ʂ��ԍ� */
	int widthNum = GetDrawFormatStringWidthToHandle(m_fontHandleNormal, "%d/%d", m_num, m_numAll);
	DrawFormatStringToHandle(X_NUM - widthNum, y + Y_NUM, color, m_fontHandleNormal,
		"%d/%d", m_num, m_numAll);
	
	int width;
	
	/* �A�[�e�B�X�g */
	{
		width = GetDrawFormatStringWidthToHandle(m_fontHandleNormal, "%s", m_artist);
		int x;
		x = (W_WINDOW - 150) - width * 2 / 3;
		/* �O����2/3�̈ʒu��W_WINDOW - 150�ɂ��킹�� */
		
		/* ���E�̒�������� */
		int widthL = x - (X_TITLE + widthTitle);
		int widthR = (X_NUM - widthNum) - (x + width);
		
		if(widthL <= 20 || widthR <=20){	/* �^�C�g����ʂ��ԍ��ɋ߂��ꍇ�́A */
			x += (widthR - widthL) / 4;
			/* ���� "/ 2" �Ƃ���ƃ^�C�g���̒[�ƒʂ��ԍ�����ߋ����� */
			//x = (xLeft + xRight - width) / 2;	/* �^�C�g���̒[�ƒʂ��ԍ�����ߋ����� */
		}
		
		/* ���E�̒��������i�������j */
		widthL = x - (X_TITLE + widthTitle);
		widthR = (X_NUM - widthNum) - (x + width);
		
		if(widthL < 0 || widthL + widthR <= 0){
		/* �^�C�g���ɂ͔��Ȃ��悤�ɂ��� */
		/* ���������͈͂Ɏ��܂�Ȃ��Ƃ� */
			x = X_TITLE + widthTitle;	/* �^�C�g���̒��� */
		}else if(widthR < 0){
		/* �^�C�g���ɂ͗]�T�����邪�A�ʂ��ԍ��ɔ��Ƃ� */
			x += widthR;	/* �ʂ��ԍ��̒��O */
		}
		DrawFormatStringToHandle(x, y + Y_ARTIST, color, m_fontHandleNormal,
			"%s", m_artist);
	}
	
	/* ���ʍ�� */
	{
		int tmp = brightness * 2 / 3;
		int tmpColor = GetColor(tmp, tmp, tmp);
		width = GetDrawFormatStringWidthToHandle(m_fontHandleNormal, "(���ʍ쐬�@%s)", m_fumenAuthor);
		DrawFormatStringToHandle(X_F_AUTHOR - width, y + Y_F_AUTHOR, tmpColor, m_fontHandleNormal,
			"(���ʍ쐬�@%s)", m_fumenAuthor);
	}
	
	/* ��Փx */
	for(int i=0; i<5; i++){
		if(m_level <= i){
			int tmp = brightness / 2;
			int tmpColor = GetColor(tmp, tmp, tmp);
			DrawStringToHandle(X_LEVEL + 16 * i, y + Y_LEVEL, "��",	/* �� */
				tmpColor, m_fontHandleNormal);
		}else{
			DrawStringToHandle(X_LEVEL + 16 * i, y + Y_LEVEL, "��",
				GetColor(brightness, brightness, 0), m_fontHandleNormal);
		}
	}

	/* �B���x */
	{
		int achievement = m_ranking.getScoreLevel();
		switch(achievement){
		case SCORE_NO_DATA:
			break;
		case SCORE_FAILED:
			DrawStringToHandle(X_ACHIEVEMENT, y + Y_ACHIEVEMENT, "��",
				GetColor(brightness/3, brightness/3, brightness/3), m_fontHandleAchievement,
				GetColor(brightness/4, brightness/4, brightness/4));
			break;
		case SCORE_RED_ZONE:
			DrawStringToHandle(X_ACHIEVEMENT, y + Y_ACHIEVEMENT, "��",
				GetColor(brightness*2/3, brightness/12, 0), m_fontHandleAchievement,
				GetColor(brightness/2, brightness/16, 0));
			break;
		case SCORE_YELLOW_ZONE:
			DrawStringToHandle(X_ACHIEVEMENT, y + Y_ACHIEVEMENT, "��",
				GetColor(brightness*7/12, brightness*2/3, 0), m_fontHandleAchievement,
				GetColor(brightness*7/16, brightness/2, 0));
			break;
		case SCORE_BLUE_ZONE:
			DrawStringToHandle(X_ACHIEVEMENT, y + Y_ACHIEVEMENT, "��",
				GetColor(0, brightness/6, brightness*2/3), m_fontHandleAchievement,
				GetColor(0, brightness/8, brightness/2));
			break;
		case SCORE_CLEAR:
			DrawStringToHandle(X_ACHIEVEMENT, y + Y_ACHIEVEMENT, "��",
				GetColor(brightness*2/3, brightness*2/3, brightness*2/3), m_fontHandleAchievement,
				GetColor(brightness/2, brightness/2, brightness/2));
			break;
		case SCORE_FULL_COMBO:
			DrawStringToHandle(X_ACHIEVEMENT, y + Y_ACHIEVEMENT, "��",
				GetColor(brightness/4, brightness*5/8, brightness), m_fontHandleAchievement,
				GetColor(0, brightness/2, brightness));
			break;
		case SCORE_FULL_GOOD:
			DrawStringToHandle(X_ACHIEVEMENT, y + Y_ACHIEVEMENT, "��",
				GetColor(brightness/4, brightness, brightness/4), m_fontHandleAchievement,
				GetColor(0, brightness, 0));
			break;
		case SCORE_PERFECT:
			DrawStringToHandle(X_ACHIEVEMENT, y + Y_ACHIEVEMENT, "��",
				GetColor(brightness, brightness, brightness/4), m_fontHandleAchievement,
				GetColor(brightness, brightness, 0));
			break;
		}
	}
	
	/* �^�C�g���i�Ō�ɏ����j */
	DrawStringToHandle(X_TITLE, y + Y_TITLE, m_title, color, m_fontHandleTitle);
}

void MusicInfo::drawComment(int x, int y){
	for(int i = 0; i < COMMENT_N_LINES; i++){
		DrawStringToHandle(30+x, y+2 +20* i, m_comment[i], GetColor(255, 255, 255), m_fontHandleNormal);
	}
}

void MusicInfo::drawPlayData(int x, int y){
	m_ranking.drawPlayData(x, y, m_fontHandleNormal);
}


void MusicInfo::drawRanking(int x, int y, int rankBegin, int rankLen){
	m_ranking.draw(x, y, rankBegin, rankLen, m_fontHandleRanking);
}
/*
void MusicInfo::renewFont(){
	deleteFont();
	createFont();
}
*/

bool MusicInfo::titleCmp(const char *buf){
/* �y �֐����������B�v�C�� �z */
/* �^�C�g���̈ꕔ�ƈ�v���邩��Ԃ� */
	return isSubstr(buf, m_title);
}

void MusicInfo::createFont(){
	m_fontHandleNormal = CreateFontToHandle("�l�r �S�V�b�N", 16, 3, DX_FONTTYPE_ANTIALIASING);
	m_fontHandleTitle = CreateFontToHandle("�l�r ����", 30, 3, DX_FONTTYPE_ANTIALIASING);
	m_fontHandleRanking = CreateFontToHandle(NULL, 16, 2);
	m_fontHandleAchievement = CreateFontToHandle("�l�r �S�V�b�N", 24, 2, DX_FONTTYPE_ANTIALIASING_EDGE);
}

void MusicInfo::deleteFont(){	/* �t�H���g���폜 */
	DeleteFontToHandle(m_fontHandleNormal);
	DeleteFontToHandle(m_fontHandleTitle);
	DeleteFontToHandle(m_fontHandleRanking);
	DeleteFontToHandle(m_fontHandleAchievement);
}

/* ============================================================ */

vector<MusicInfo> g_infoArray;

vector<MusicInfo>::iterator prevInfo(vector<MusicInfo>::iterator itr){
/* 1�߂�B�ŏ�����1�߂�ƍŌ�ɁB */
	if(itr == g_infoArray.begin()){
		itr = g_infoArray.end();
	}
	itr--;
	return itr;
}

vector<MusicInfo>::iterator nextInfo(vector<MusicInfo>::iterator itr){
/* 1�i�ށB1�i��ōŌ�𒴂���ƍŏ��ɁB */
	itr++;
	if(itr == g_infoArray.end()){
		itr = g_infoArray.begin();
	}
	return itr;
}

vector<MusicInfo>::iterator randomInfo(){
	return &g_infoArray[GetRand(g_infoArray.size() - 1)];
}

/* ============================================================ */

struct DrawMainInfo{
public:
	vector<MusicInfo>::iterator musicInfoItr;	/* MusicInfo�̔z��̂ǂ����w���Ă��邩 */
	int rankingPos;
	int rankingFlag;	/* true:�ڍ׃����L���O�Afalse:1�ʂ݂̂܂��́A�R�����g�\�� */
	
	int drawPosX;
	double drawPosY;
	int *addHeight;
private:
	int _addHeight[21];
	int m_addHeightWait;	/* 0����0�ȊO�ɂȂ鎞�ɑ҂B����𐔂��� */
	
	char m_findStr[256];	/* �����ɗp���镶����B�Č����p */
public:
	DrawMainInfo();
	void resetPos();
	
	void step();
	
	void next();
	void prev();
	
	void left();
	void right();
	
	void find(const char *buf);
	void findNext();
	void randomSelect();
};

DrawMainInfo::DrawMainInfo(){
	musicInfoItr = g_infoArray.begin();
	
	addHeight = _addHeight + 10;	/* �y����R�s�[�������ɂ܂����\���z */
	strcpy(m_findStr, "");
	
	resetPos();
}

void DrawMainInfo::resetPos(){
	rankingPos = 0;
	rankingFlag = false;
	drawPosX = 0;
	drawPosY = 0.0;
	for(int i=-10; i<=10; i++){
		addHeight[i] = 0;
	}
	m_addHeightWait = 25;
}

void DrawMainInfo::step(){
	for(int i=-10; i<=10; i++){
		int h = 0;	/* �{���̕� */
		if(i==0){
			if(rankingFlag){
				h = H_RANKING;
			}else if(rankingPos == 0){
				h = H_RANKING1;
			}else{
				h = H_MUSIC_INFO_SUB;
			}
			if(addHeight[0] == 0 && m_addHeightWait > 0){
			/* ��0����J�����Ƃ��Ă��āA����ɕK�v�Ȏ��Ԍo�߂��Ă��Ȃ� */
				m_addHeightWait--;
				continue;
			}
		}
		addHeight[i] = h + (int)((addHeight[i] - h) * 0.80);
	}
	{
		drawPosX *= 0.70;
	}
	{
		int sign = drawPosY==0 ? 0 : drawPosY>0 ? 1 : -1;
		double tmp = -0.05 * drawPosY;	/* drawPosY��␳����� */
		tmp -= sign * 1.0;
		tmp -= sign * 0.0010 * drawPosY * drawPosY;
		drawPosY += tmp;
		if(drawPosY * tmp > 0.0){	/* 0��ʂ�߂��� */
			drawPosY = 0.0;
		}
	}
}

void DrawMainInfo::next(){
	musicInfoItr = nextInfo(musicInfoItr);	/* ���̋� */
	
	drawPosY += MUSIC_INFO_HEIGHT + (addHeight[0] + addHeight[1]) / 2.0;
	for(int i=-10; i<10; i++){
		addHeight[i] = addHeight[i+1];
	}
	addHeight[10] = 0;
	
	m_addHeightWait = 25;
	
	rankingPos = 0;
	rankingFlag = false;
}

void DrawMainInfo::prev(){
	musicInfoItr = prevInfo(musicInfoItr);	/* �O�̋� */
	
	drawPosY -= MUSIC_INFO_HEIGHT + (addHeight[0] + addHeight[-1]) / 2.0;
	for(int i=10; i>-10; i--){
		addHeight[i] = addHeight[i-1];
	}
	addHeight[-10] = 0;
	
	m_addHeightWait = 25;
	
	rankingPos = 0;
	rankingFlag = false;
}

void DrawMainInfo::left(){
	if(rankingPos < 0){	/* �R�����g�\���̑O�ւ͍s���Ȃ� */
		return;
	}
	
	m_addHeightWait /= 2;	/* �ڍ׃����L���O�Ƃ������������ɂ͎��Ԃ����炵�Ă����� */
	
	if(rankingPos == 0 && rankingFlag){	/* 1�ʂ̂݁�1�`RANKING_DRAW_LEN�ʂ̂Ƃ� */
		rankingFlag = false;
	}else{	/* ���̑��̎� */
		rankingPos -= RANKING_DRAW_LEN;
		drawPosX -= W_WINDOW;
	}
}

void DrawMainInfo::right(){
	if(rankingPos + RANKING_DRAW_LEN >= RANKING_LEN){
		return;
	}
	
	m_addHeightWait /= 2;	/* �ڍ׃����L���O�Ƃ������������ɂ͎��Ԃ����炵�Ă����� */
	
	if(rankingPos == 0 && !rankingFlag){	/* 1�ʂ̂݁�1�`RANKING_DRAW_LEN�ʂ̂Ƃ� */
		rankingFlag = true;
	}else{	/* ���̑��̎� */
		rankingPos += RANKING_DRAW_LEN;
		drawPosX += W_WINDOW;
	}
}

void DrawMainInfo::find(const char *buf){
	strcpy(m_findStr, buf);
	findNext();
}

void DrawMainInfo::findNext(){
	if(strlen(m_findStr) > 0){	/* �������͂���Ă���Ό��� */
		vector<MusicInfo>::iterator tmpItr = musicInfoItr;
		do{
			musicInfoItr = nextInfo(musicInfoItr);
			if(musicInfoItr->titleCmp(m_findStr)){
				resetPos();
				/* �����������͍̂ŏ����璆�S�ɕ\�� */
				break;
			}
		}while(musicInfoItr != tmpItr);	/* ���̏ꏊ�ɖ߂��Ă���܂� */
	}
}

void DrawMainInfo::randomSelect(){
	musicInfoItr = randomInfo();
	resetPos();
}

/* ============================================================ */

/* rankingPos�ʂ���̃����L���O�i���̎��R�����g�j��(x,y)����`��B������h�ɐ��� */
void drawMainRanking(vector<MusicInfo>::iterator itr, int rankingPos,
		int x, int y, int h){
	int yMin = y, yMax = y+h;
	if(yMin < 0){
		yMin = 0;
	}
	if(yMax > 360){
		yMax = 360;
	}
	SetDrawArea(10, yMin, W_WINDOW-10, yMax);
	if(rankingPos >= 0){
		/* �����L���O�\���irankingPos����RANKING_DRAW_LEN�j */
		/* 1�ʂ����\�����Ȃ������B���̂Ŗ��Ȃ� */
		itr->drawRanking(x, y + 6, rankingPos, RANKING_DRAW_LEN);	/* 6==(60-48)/2 */
	}else{
		/* �R�����g�\�� */
		itr->drawComment(x, y + 6);
		itr->drawPlayData(x, y + (6+H_COMMENT+4));
	}
}

/* ���C����ʁi�ȑI����ʁj��`�� */
void drawMain(DrawMainInfo &dInfo, CChallenge &challenge,
		const char *name, int inputHandle, int fontHandleDefault){
	//if(dInfo.rankingPos == -RANKING_DRAW_LEN){
	int dy = 180 + (MUSIC_INFO_HEIGHT - dInfo.addHeight[0])/2
		+ (int)floor(dInfo.drawPosY + 0.5);
	SetDrawArea(10, 0, W_WINDOW-10, 360);	/* �`��͈͂𐧌� */
	{
		DrawBox(10, dy - MUSIC_INFO_HEIGHT, W_WINDOW - 10, dy + dInfo.addHeight[0], GetColor(16, 16, 32), TRUE);
		dInfo.musicInfoItr->draw(dy - MUSIC_INFO_HEIGHT, 255);	/* �^�C�g���ȂǕ\�� */
		DrawLine(40, dy, W_WINDOW - 40, dy, GetColor(64, 64, 64));
		
		drawMainRanking(dInfo.musicInfoItr, dInfo.rankingPos, dInfo.drawPosX, dy, dInfo.addHeight[0]);
		if(dInfo.drawPosX < 0){	/* �E�̂�`�� */
			drawMainRanking(dInfo.musicInfoItr, dInfo.rankingPos + RANKING_DRAW_LEN,
				dInfo.drawPosX + W_WINDOW, dy, dInfo.addHeight[0]);
		}else if(dInfo.drawPosX > 0){	/* ���̂�`�� */
			drawMainRanking(dInfo.musicInfoItr, dInfo.rankingPos - RANKING_DRAW_LEN,
				dInfo.drawPosX - W_WINDOW, dy, dInfo.addHeight[0]);
		}
		/* �I�񂾂��̂������Ă��Ȃ��āA2����E�������Ă���ꍇ�Ƃ������邪�A�킴�킴�`���Ȃ� */
	}
	SetDrawArea(10, 0, W_WINDOW-10, 360);	/* �`��͈͂𐧌� */
	/* �^�C�g���ȂǕ\���A�������ŋ�؂� */
	{	/* �O�̋� */
		vector<MusicInfo>::iterator itr = dInfo.musicInfoItr;
		int y = dy - MUSIC_INFO_HEIGHT;
		int cnt = 0;
		while(y >= 0){	/* �g���̊� */
			DrawLine(10, y, W_WINDOW - 10, y,
				GetColor(255/(cnt+2), 255/(cnt+2), 255/(cnt+2)));
			itr = prevInfo(itr);
			cnt++;
			y -= MUSIC_INFO_HEIGHT + dInfo.addHeight[-cnt];
			(*itr).draw(y, 255/(cnt+1));
		}
	}
	{	/* ��̋� */
		vector<MusicInfo>::iterator itr = dInfo.musicInfoItr;
		int y = dy + dInfo.addHeight[0];
		int cnt = 0;
		while(y <360){	/* �g���̊� */
			DrawLine(10, y, W_WINDOW - 10, y,
				GetColor(255/(cnt+2), 255/(cnt+2), 255/(cnt+2)));
			itr = nextInfo(itr);
			cnt++;
			(*itr).draw(y, 255/(cnt+1));
			y += MUSIC_INFO_HEIGHT + dInfo.addHeight[cnt];
		}
	}
	SetDrawArea(0, 0, W_WINDOW, H_WINDOW);	/* �`��͈͂����ɖ߂� */
	DrawBox(0, 360, W_WINDOW, H_WINDOW - 25, GetColor(24, 24, 24), TRUE);
	DrawLine(0, 360, W_WINDOW, 360, GetColor(170, 170, 170));
	
	if(inputHandle == -1){	/* �������ȊO */
		DrawStringToHandle(10, 370, "��/��: �ȑI��,   F: ����, R: �����_��,   F5: �ēǍ�,",
			GetColor(255, 255, 255), fontHandleDefault);
		
		DrawStringToHandle(10, 390, "��/��: ���/�����L���O�\��, Tab: �S��ʐؑ�,",
			GetColor(255, 255, 255), fontHandleDefault);
		
		{
			char *str = "Enter: ����";
			int width = GetDrawStringWidthToHandle(str, strlen(str), fontHandleDefault);
			DrawStringToHandle(W_WINDOW - 10 - width, 370, str,
				GetColor(255, 255, 255), fontHandleDefault);	/* �E����10�̂Ƃ���ɕ`�� */
		}
		{
			char *str = "!/?: Replay Save/Load";
			int width = GetDrawStringWidthToHandle(str, strlen(str), fontHandleDefault);
			DrawStringToHandle(W_WINDOW - 10 - width, 390, str,
				GetColor(255, 255, 255), fontHandleDefault);	/* �E����10�̂Ƃ���ɕ`�� */
		}
		
		DrawStringToHandle(10, H_WINDOW - 65, "[H]idden, [S]udden, [C]ircleStealth, [L]yricsStealth,",
			GetColor(255, 255, 255), fontHandleDefault);
		DrawStringToHandle(10, H_WINDOW - 45, "</>: Speed Down/Up, +/-: KeyShift,   F6/F7/F8: sin/cos/tan,",
			GetColor(255, 255, 255), fontHandleDefault);
		{
			char *str = "Reset to [D]efault";
			int width = GetDrawStringWidthToHandle(str, strlen(str), fontHandleDefault);
			DrawStringToHandle(W_WINDOW - 10 - width, H_WINDOW - 65, str,
				GetColor(255, 255, 255), fontHandleDefault);	/* �E����10�̂Ƃ���ɕ`�� */
		}
		{
			char *str = "Esc: �I��";
			int width = GetDrawStringWidthToHandle(str, strlen(str), fontHandleDefault);
			DrawStringToHandle(W_WINDOW - 10 - width, H_WINDOW - 45, str,
				GetColor(255, 255, 255), fontHandleDefault);	/* �E����10�̂Ƃ���ɕ`�� */
		}
	}else{
		//SetDrawArea(0, 360, W_WINDOW, H_WINDOW - 25);
		DrawStringToHandle(10, 370, "�����Ȗ�:",
			GetColor(255, 255, 255), fontHandleDefault);
		DrawStringToHandle(10, H_WINDOW - 45, "Enter: ����, �i������ɁjF3: ��������",
			GetColor(255, 255, 255), fontHandleDefault);
		DrawKeyInputModeString(W_WINDOW, H_WINDOW - 45);	//(W_WINDOW, H_WINDOW - 25);
		DrawKeyInputString(10, 390, inputHandle);	/* �����������\�� */
		//SetDrawArea(0, 0, W_WINDOW, H_WINDOW);	/* �`��͈͂����ɖ߂� */
	}
	
	{	/* ���O��\�� */
		char buf[256];
		int color;
		if(strlen(name) > 0){
			strcpy(buf, name);
			color = GetColor(255, 255, 255);
		}else{
			strcpy(buf, "(guest)");
			color = GetColor(128, 128, 128);
		}
		int width = GetDrawStringWidthToHandle(buf, strlen(buf), fontHandleDefault);
		DrawBox(W_WINDOW - 20 - width, H_WINDOW - 25, W_WINDOW, H_WINDOW, GetColor(16, 16, 32), TRUE);
		DrawStringToHandle(W_WINDOW - 10 - width, H_WINDOW - 20, buf, color, fontHandleDefault);
	}
	
	DrawLine(0, H_WINDOW - 25, W_WINDOW, H_WINDOW - 25, GetColor(170, 170, 170));
	
	if(challenge.test(CHALLENGE_STEALTH)){
		DrawStringToHandle(130, H_WINDOW - 20, "[ CStealth ]", GetColor(255, 32, 0), fontHandleDefault);
	}else{
		if(challenge.test(CHALLENGE_HIDDEN)){
			if(challenge.test(CHALLENGE_SUDDEN)){
				DrawStringToHandle(130, H_WINDOW - 20, "[ Hid.Sud. ]", GetColor(255, 128, 0), fontHandleDefault);
			}else{
				DrawStringToHandle(130, H_WINDOW - 20, "[  Hidden  ]", GetColor(255, 255, 0), fontHandleDefault);
			}
		}else{
			if(challenge.test(CHALLENGE_SUDDEN)){
				DrawStringToHandle(130, H_WINDOW - 20, "[  Sudden  ]", GetColor(255, 255, 0), fontHandleDefault);
			}else{
				DrawStringToHandle(130, H_WINDOW - 20, "[ Hid.Sud. ]", GetColor(64, 64, 64), fontHandleDefault);
			}
		}
	}
	{
		int color;
		if(challenge.test(CHALLENGE_LYRICS_STEALTH)){
			color = GetColor(255, 128, 0);
		}else{
			color = GetColor(64, 64, 64);
		}
		DrawStringToHandle(230, H_WINDOW - 20, "[ LStealth ]", color, fontHandleDefault);
	}
	{
		char buf[256];
		int color;
		sprintf(buf, "[ Speed x%3.1f ]", challenge.speed());
		if(challenge.speed() > 3.0){
			color = GetColor(255, 32, 0);
		}else if(challenge.speed() > 2.0){
			color = GetColor(255, 128, 0);
		}else if(challenge.speed() > 1.0){
			color = GetColor(255, 255, 0);
		}else if(challenge.speed() < 1.0){
			color = GetColor(128, 128, 255);
		}else{
			color = GetColor(64, 64, 64);
		}
		DrawStringToHandle(10, H_WINDOW - 20, buf, color, fontHandleDefault);
	}
	
	{
		char buf[256];
		int color;
		if(challenge.key() > 8){
			sprintf(buf, "[ Key +%2d ]", challenge.key());
			color = GetColor(255, 32, 0);
		}else if(challenge.key() > 4){
			sprintf(buf, "[ Key +%2d ]", challenge.key());
			color = GetColor(255, 128, 0);
		}else if(challenge.key() > 0){
			sprintf(buf, "[ Key +%2d ]", challenge.key());
			color = GetColor(255, 255, 0);
		}else if(challenge.key() < 0){
			sprintf(buf, "[ Key -%2d ]", -challenge.key());
			color = GetColor(128, 255, 0);
		}else{
			sprintf(buf, "[ Key %3d ]", challenge.key());
			color = GetColor(64, 64, 64);
		}
		DrawStringToHandle(330, H_WINDOW - 20, buf, color, fontHandleDefault);
	}
	
	if(challenge.test(CHALLENGE_SIN)){
		if(challenge.test(CHALLENGE_COS)){
			if(challenge.test(CHALLENGE_TAN)){
				DrawStringToHandle(430, H_WINDOW - 20, "[sct]", GetColor(255, 128, 0), fontHandleDefault);
			}else{
				DrawStringToHandle(430, H_WINDOW - 20, "[sc-]", GetColor(255, 128, 0), fontHandleDefault);
			}
		}else{
			if(challenge.test(CHALLENGE_TAN)){
				DrawStringToHandle(430, H_WINDOW - 20, "[s-t]", GetColor(255, 128, 0), fontHandleDefault);
			}else{
				DrawStringToHandle(430, H_WINDOW - 20, "[sin]", GetColor(255, 255, 0), fontHandleDefault);
			}
		}
	}else{
		if(challenge.test(CHALLENGE_COS)){
			if(challenge.test(CHALLENGE_TAN)){
				DrawStringToHandle(430, H_WINDOW - 20, "[-ct]", GetColor(255, 128, 0), fontHandleDefault);
			}else{
				DrawStringToHandle(430, H_WINDOW - 20, "[cos]", GetColor(255, 255, 0), fontHandleDefault);
			}
		}else{
			if(challenge.test(CHALLENGE_TAN)){
				DrawStringToHandle(430, H_WINDOW - 20, "[tan]", GetColor(255, 255, 0), fontHandleDefault);
			}else{
				DrawStringToHandle(430, H_WINDOW - 20, "[sct]", GetColor(64, 64, 64), fontHandleDefault);
			}
		}
	}
	/*
	if(challenge.test(CHALLENGE_SIN)){
		color = GetColor(255, 255, 0);
	}else{
		color = GetColor(64, 64, 64);
	}
	DrawStringToHandle(540, H_WINDOW - 20, "[s]", color, fontHandleDefault);
	
	if(challenge.test(CHALLENGE_COS)){
		color = GetColor(255, 255, 0);
	}else{
		color = GetColor(64, 64, 64);
	}
	DrawStringToHandle(570, H_WINDOW - 20, "[c]", color, fontHandleDefault);
	
	if(challenge.test(CHALLENGE_TAN)){
		color = GetColor(255, 255, 0);
	}else{
		color = GetColor(64, 64, 64);
	}
	DrawStringToHandle(600, H_WINDOW - 20, "[t]", color, fontHandleDefault);
	*/
}

void editChallenge(CChallenge &challenge, char ch){
	switch(ch){
	case 'h':
	case 'H':
		challenge.flip(CHALLENGE_HIDDEN);
		if(challenge.test(CHALLENGE_HIDDEN)){
			challenge.reset(CHALLENGE_STEALTH);
		}
		break;
	case 's':
	case 'S':
		challenge.flip(CHALLENGE_SUDDEN);
		if(challenge.test(CHALLENGE_SUDDEN)){
			challenge.reset(CHALLENGE_STEALTH);
		}
		break;
	case 'c':
	case 'C':
		challenge.flip(CHALLENGE_STEALTH);
		if(challenge.test(CHALLENGE_STEALTH)){
			challenge.reset(CHALLENGE_HIDDEN);
			challenge.reset(CHALLENGE_SUDDEN);
		}
		break;
	case 'l':
	case 'L':
		challenge.flip(CHALLENGE_LYRICS_STEALTH);
		break;
	case '>':
		challenge.speedUp();
		break;
	case '<':
		challenge.speedDown();
		break;
	case '+':
		challenge.keyUp();
		break;
	case '-':
		challenge.keyDown();
		break;
	case CTRL_CODE_F6:
		challenge.flip(CHALLENGE_SIN);
		break;
	case CTRL_CODE_F7:
		challenge.flip(CHALLENGE_COS);
		break;
	case CTRL_CODE_F8:
		challenge.flip(CHALLENGE_TAN);
		break;
	case 'd':
	case 'D':
		challenge.reset();	/* �S�`�������W���Ȃ��� */
		break;
	default:
		break;
	}
}

bool main2(bool &isWindowMode, const char *name){
	/* false��Ԃ��΁A�I���Atrue��Ԃ��΁AisWindowMode�ŃE�B���h�E��K�v�Ȃ�ύX���čēǂݍ��� */
	int fontHandleDefault = CreateFontToHandle(NULL, 16, 3);
	
	bool retValue;	/* ��ʃ��[�h��ς��đ�����ꍇtrue */
	
	DrawMainInfo dInfo;
	
	CChallenge challenge;
	
	int inputHandle = -1;	/* �����p���͂̃L�[���̓n���h�� */
	SetKeyInputStringFont(fontHandleDefault);
	SetKeyInputStringColor(
		GetColor( 255 , 255 , 255 ) ,
		GetColor( 255 , 255 , 255 ) ,
		GetColor( 255 , 255 ,   0 ) ,
		GetColor(   0 , 255 , 255 ) ,
		GetColor( 255 , 255 , 255 ) ,
		GetColor( 255 ,  0 ,  0 ) ,
		GetColor( 255 , 255 ,  20 )
	);
	
	CTyping typing;
	
	typing.setName(name);
	
	while(1){
		if(ProcessMessage() == -1){
			retValue = false;
			break;
		}
		
		//ClearDrawScreen();
		drawMain(dInfo, challenge, name, inputHandle, fontHandleDefault);
		dInfo.step();
		myScreenFlip();
		
		if(inputHandle == -1){	/* �������������͒��ł͂Ȃ� */
			char ch = GetKeyboardInput();
			switch(ch){
			case CTRL_CODE_ESC:	/* �I�� */
				retValue = false;
				goto L1;
			case CTRL_CODE_F5:	/* �ēǂݍ��� */
				retValue = true;
				goto L1;
			case CTRL_CODE_TAB:	/* Window ���� FullScreen */
				isWindowMode = !isWindowMode;	/* Window���ǂ������t�ɂ��� */
				retValue = true;
				goto L1;
			case CTRL_CODE_CR:	/* �Q�[���J�n */
				typing.setChallenge(challenge);	/* ���[�h�O�Ƀ`�������W��ݒ� */
				(*dInfo.musicInfoItr).load(typing);	/* �Q�[�����u�Ɂv���ʁE�n�C�X�R�A�̃t�@�C�������[�h */
				/* �������typing.nanka(musicInfoItr)�݂����ɂ������񂾂��A���낢��ʓ| */
				typing.mainLoop();	/* �`��Ƃ���CTyping�ɔC���� */
				typing.unload();	/* �I������ */
				
				dInfo.resetPos();
				/* �߂��Ă����Ƃ��ɁA�ڍ׃����L���O���甲���� */
				/* �i�Ȃ�ƂȂ��A1�ʂɃ����N�C�������̂ɂق��̂Ƃ��낪�\������Ă���߂����j */
				
				(*dInfo.musicInfoItr).readRanking();	/* �����L���O�͍X�V����Ă��邩������Ȃ� */
				break;
			/* �Ō�̃Q�[���̂��Đ� */
			/*
			case '@':
				(*musicInfoItr).load(typing);
				typing.replayLoop();
				typing.unload();
				break;
			*/
			case '!':	/* ���v���C��ۑ�*/
				typing.saveReplayLoop();
				break;
			case '?':	/* �ۑ����ꂽ���v���C���Đ� */
				if(!typing.loadReplayLoop()){
					break;	/* ���v���C�̓ǂݍ��݂Ɏ��s�����ꍇ�͖߂� */
				}
				typing.setChallenge(challenge);	/* ���[�h�O�Ƀ`�������W��ݒ� */
				(*dInfo.musicInfoItr).load(typing);
				typing.replayLoop();
				typing.unload();
				break;
			case 'r':	/* �����_���I�� */
			case 'R':
				dInfo.randomSelect();
				break;
			case 'f':	/* ���� */
			case 'F':
				/* �L�����Z������A���p�܂��͐����Ɍ���͂��Ȃ� */
				inputHandle = MakeKeyInput(64, TRUE, FALSE, FALSE);
				if(inputHandle == -1){
					throw __LINE__;
				}
				SetActiveKeyInput(inputHandle);
				break;
			case CTRL_CODE_F3:	/* �������� */
				dInfo.findNext();
				break;
			case CTRL_CODE_UP:	/* �O�̋Ȃ� */
				dInfo.prev();
				break;
			case CTRL_CODE_DOWN:	/* ��̋Ȃ� */
				dInfo.next();
				break;
			case CTRL_CODE_LEFT:	/* ��ʂ�\�� */
				dInfo.left();
				break;
			case CTRL_CODE_RIGHT:	/* ���ʂ�\�� */
				dInfo.right();
				break;
			default:
				editChallenge(challenge, ch);
				break;
			}
			
		}else{	/* �������������͒� */
			int tmp = CheckKeyInput(inputHandle);
			switch(tmp){
			case 0:
				break;
			case 1:	/* ���͏I�� */
				char buf[256];
				GetKeyInputString(buf, inputHandle);
				dInfo.find(buf);
				/* �����ŁA break; ���Ȃ� */
			case 2:
				/* ��̏ꍇ�ɉ����A�L�����Z�����ꂽ�ꍇ */
				DeleteKeyInput(inputHandle);
				inputHandle = -1;
				ClearKeyboardInput();	/* �L�[���́i�����ŏ������Ă���j�������Ă��� */
				break;
			default:	/* �����G���[�i-1�̂Ƃ��j */
				throw __LINE__;
			}
		}
	}
	
L1:
	DeleteFontToHandle(fontHandleDefault);
	return retValue;
}

/* flag �̓��X�g�̓ǂݍ��݂������������ǂ��� */
void drawTitle(int fontHandleTitle, int fontHandleCopyright, int fontHandleInfo, const char *strInfo){
	//drawBox(0, 0, W_WINDOW, H_WINDOW, Color(32, 32, 32);
	{
		const char *strTitle = "UTyping";
		int strWidth = GetDrawStringWidthToHandle(strTitle, strlen(strTitle), fontHandleTitle);
		DrawStringToHandle((W_WINDOW - strWidth) / 2, H_WINDOW / 3 - 24, strTitle,
			GetColor(255, 255, 255), fontHandleTitle, GetColor(170, 170, 170));
	}
	{
		const char *strCopyright = "(c)2007-2008 tos";
		int strWidth = GetDrawStringWidthToHandle(strCopyright, strlen(strCopyright), fontHandleCopyright);
		DrawStringToHandle(W_WINDOW - 10 - strWidth, H_WINDOW - 10 - 12, strCopyright,
			GetColor(255, 255, 255), fontHandleCopyright);
	}
	{
		int strWidth = GetDrawStringWidthToHandle(strInfo, strlen(strInfo), fontHandleInfo);
		DrawStringToHandle((W_WINDOW - strWidth) / 2, (H_WINDOW) * 2 / 3 - 12, strInfo,
			GetColor(255, 255, 255), fontHandleInfo);
	}
	/*
	DrawStringToHandle(10, 10, "�����f�[�^�\����啝�ɏ��������܂����B",
		GetColor(255, 255, 0), fontHandleInfo);
	DrawStringToHandle(10, 40, "���������������Ȃ����_������΋����Ă��������B",
		GetColor(255, 255, 0), fontHandleInfo);
	*/
}

int readList(){
	int count = 0;
	g_infoArray.clear();	/* �ǂݍ��ޑO�ɏ��� */
	FILE *fpList;
	fpList = fopen("UTyping_list.txt", "r");	/* ���X�g���J�� */
	if(fpList == NULL){
		throw "UTyping_list.txt ���J���܂���B";
	}
	char buf[256];
	while(fgetline(buf, fpList) != NULL){
		FILE *fpInfo;
		MusicInfo info;
		fpInfo = fopen(buf, "r");	/* ���t�@�C�����J�� */
		if(fpInfo == NULL){	/* �J���Ȃ��t�@�C���͖��� */
			continue;
		}
		char dirName[256];	/* �f�B���N�g���͏��t�@�C������Ƃ��� */
		getDirFromPath(dirName, buf);
		char buf0[256], buf1[256], buf2[256], *chk;
		fgetline(info.m_title, fpInfo);	/* 1�s�ڂɃ^�C�g�� */
		fgetline(info.m_artist, fpInfo);	/* 2�s�ڂɌ���� */
		fgetline(info.m_fumenAuthor, fpInfo);	/* 3�s�ڂɕ��ʍ�� */
		fgetline(buf0, fpInfo);
		fgetline(buf1, fpInfo);
		chk = fgetline(buf2, fpInfo);
		if(chk == NULL){	/* �S�s�ǂݍ��߂Ȃ����� */
			continue;
		}
		for(int i = 0; i < COMMENT_N_LINES; i++){	/* �c��̍s�̓R�����g */
			chk = fgetline(info.m_comment[i], fpInfo);
			if(chk == NULL){
				strcpy(info.m_comment[i],"");
			}
		}
		fclose(fpInfo);	/* ���t�@�C������� */
		int n = sscanf(buf0, "%d", &info.m_level);	/* 4�s�ڂɓ�Փx */
		if(n < 1){
			continue;
		}
		sprintf(info.m_fumenFileName, "%s%s", dirName, buf1);	/* 5�s�ڂɕ��ʃt�@�C���� */
		sprintf(info.m_rankingFileName, "%s%s", dirName, buf2);	/* 6�s�ڂɃn�C�X�R�A�t�@�C���� */
		count++;
		info.m_num = count;	/* �ʂ��ԍ������� */
		g_infoArray.push_back(info);
	}
	fclose(fpList);	/* ���X�g����� */
	
	for(vector<MusicInfo>::iterator i = g_infoArray.begin(); i != g_infoArray.end(); i++){
		(*i).readRanking();		/* �����L���O��ǂݍ��� */
		(*i).m_numAll = count;
	}
	
	return count;
}

void main1(bool &isWindowMode){
	char name[NAME_LEN + 1];
	int nameLen;
	name[0] = '\0';
	nameLen = 0;
	while(1){
		g_config.read();	/* config��ǂݍ��� */
		
		myDxLib_Init();
		
		/* VSync��҂���ݒ� */
		if(SetWaitVSyncFlag(g_config.f_waitVSync) == -1){
			throw "WaitVSync�̐ݒ�Ɏ��s���܂����B";
		}
		
		g_fontHandleDebug = CreateFontToHandle(NULL, 10, 3, DX_FONTTYPE_EDGE);
		
		{
			int fontHandleTitle = CreateFontToHandle("�l�r ����", 48, 3, DX_FONTTYPE_ANTIALIASING_EDGE);
			int fontHandleNormal = CreateFontToHandle("�l�r �S�V�b�N", 16, 2, DX_FONTTYPE_ANTIALIASING);
			int fontHandleInfo = CreateFontToHandle("�l�r ����", 24, 3, DX_FONTTYPE_ANTIALIASING);
			int fontHandleName = CreateFontToHandle("�l�r ����", 36, 2, DX_FONTTYPE_ANTIALIASING);
			//ClearDrawScreen();
			drawTitle(fontHandleTitle, fontHandleNormal, fontHandleInfo, "Now loading...");
			myScreenFlip();
			
			bool isCorrectID = checkUTypingUserID();
			
			int count = readList();
			
			if(count == 0){
				throw "�Ȃ���Ȃ����݂��܂���B";
			}
			
			while(1){
				if(ProcessMessage() == -1){
					return;
				}
				//SetDrawScreen(DX_SCREEN_BACK);
				//ClearDrawScreen();
				drawTitle(fontHandleTitle, fontHandleNormal, fontHandleInfo,
					isCorrectID ? "Press Enter key." : "Wrong UTyping user ID...");
				if(isCorrectID){
					DrawFormatStringToHandle(30, 375, GetColor(255, 255, 255), fontHandleNormal,
						"���O����͂��Ă������� :");
					if(nameLen > 0){
						DrawStringToHandle(60, 400, name, GetColor(255, 255, 255), fontHandleName);
					}else{
						DrawStringToHandle(60, 400, "�i���ݒ�j", GetColor(128, 128, 128), fontHandleName);
					}
				}
				myScreenFlip();
				char ch = GetKeyboardInput();
				if(ch == 0){
					continue;
				}
				if(!isCorrectID){	/* ID���s���ȏꍇ�́A�ǂ̃L�[�������Ă��I���B */
					return;
				}
				if(ch == CTRL_CODE_TAB){	/* Tab�������ƁAWindow ���� FullScreen */
					isWindowMode = !isWindowMode;	/* isWindowMode��ύX���čċN�� */
					break;
				}
				int ret = editBuffer(ch, name, nameLen);
				if(ret == EDIT_BUFFER_OK){	/* ���s�������ꂽ��ȑI����ʂ� */
					if(!main2(isWindowMode, name)){	/* false���߂����ꍇ�i�I������Ƃ��j */
						return;
					}
					break;	/* true���߂����ꍇ�i�ċN������Ƃ��j */
					
				}else if(ret == EDIT_BUFFER_CANCEL){	/* ���͂��L�����Z�����ꂽ */
					if(nameLen > 0){	/* ���͂��������ꍇ�͓��͂����� */
						name[0] = '\0';
						nameLen = 0;
					}else{	/* �����łȂ��ꍇ�͏I�� */
						return;
					}
				}
			}
			
			g_infoArray.clear();
			
			DeleteFontToHandle(fontHandleTitle);
			DeleteFontToHandle(fontHandleInfo);
			DeleteFontToHandle(fontHandleName);
			DeleteFontToHandle(fontHandleNormal);
		}
		DeleteFontToHandle(g_fontHandleDebug);
		if(ChangeWindowMode(isWindowMode) != DX_CHANGESCREEN_OK){
			/* WindowMode�̐؂�ւ��Ɏ��s���� */
			throw "Window ���� FullScreen �̐؂�ւ��Ɏ��s���܂����B";
		}
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPSTR lpCmdLine, int nCmdShow){
	
	try{
		g_config.read();	/* config��ǂݍ��� */
		bool isWindowMode = !g_config.f_fullScreen;
		ChangeWindowMode(isWindowMode);
		
		SetMainWindowText("UTyping");
		
		SetHookWinProc(KeyboardInputProc);
		
		main1(isWindowMode);
	}catch(int n){
		char buf[256];
		sprintf(buf, "%d �s�ڂŃG���[���������܂����B", n);
		outputError(buf);
	}catch(char *str){
		outputError(str);
	}
	myDxLib_End();
	return 0;
}

#if 0
int main(){
	return WinMain(0,0,0,0);
}
#endif
