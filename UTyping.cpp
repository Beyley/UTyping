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

#define NAME_LEN 16
#define TYPE_BUFFER_LEN NAME_LEN
/* �^�C�s���O�ɗp����o�b�t�@�̒��� */
/* ���O�̍ő咷�ɓ����� */

#define H_COMMENT H_RANKING
/* �R�����g�̕��i�����j��RANKING_LEN �ʕ���`�悷��Ƃ��Ɠ��� */

#define MUSIC_INFO_HEIGHT 60
/* �ȏ��̕��i�����j */

/* ============================================================ */

/* �`��ʒu�Ȃ� */

#define CIRCLE_SPEED 250.0
/* �����~�̕W���̑����i�h�b�g/�b�j */

#define X_SCORE (W_WINDOW - 25)
#define Y_SCORE 70

#define X_GAUGE (W_WINDOW - 10)
#define W_GAUGE 400
#define Y_GAUGE 10
#define H_GAUGE 40

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

/* ============================================================ */

#define W_WINDOW 640
#define H_WINDOW 480

/* ============================================================ */

#define INFTY 1000000000

enum{
EDIT_BUFFER_NONE,
EDIT_BUFFER_OK,
EDIT_BUFFER_CANCEL,
};

enum{
PHASE_READY,	/* �J�n�O */
PHASE_MAIN,	/* ���C�� */
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

/* �L�[���͂Ńo�b�t�@�𑀍삷�� */

#define editBuffer(ch, buf, len) editBuffer_1(ch, buf, len, sizeof(buf))

int editBuffer_1(char ch, char *buf, int &len, int bufSize){
	if(ch < CTRL_CODE_CMP){	/* �����R�[�h�łȂ��Ƃ� */
		switch(ch){
		case CTRL_CODE_CR:	/* ���s�Ȃ�m�� */
			return EDIT_BUFFER_OK;
		case CTRL_CODE_BS:	/* BackSpace�⍶�L�[��Delete�Ȃ� */
		case CTRL_CODE_LEFT:
		case CTRL_CODE_DEL:
			if(len > 0){
				len--;	/* �ꕶ���폜 */
			}
			buf[len] = '\0';
			break;
		case CTRL_CODE_ESC:
			return EDIT_BUFFER_CANCEL;
		}
		return 0;
	}
	if(len < bufSize - 1){
		buf[len++] = ch;
		buf[len] = '\0';
	}
	return 0;
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

class CConvertData{
public:
	CConvertData(const char *buf1, const char *buf3);
	bool match_front(const char *str);
	bool match(const char *str);
public:
	/* �܂��A���Ƃ���struct�ɂ�����肾�������炢���� */
	char m_str[9];	/* ���{�� �� ���[�}��, �S�p �� ���p �̕ϊ���� 4 �����ȓ�������A5�ȏ�ɂ��Ă����΂悢 */
	int m_len;
	/* str �ɂ͏����Ɏg���镶���񂪓����Ă��邪�A���̂����m�肳��钷�� */
	/* ��: �u��v�ɑ΂��� {"nb", 1}, �u�����v�ɑ΂��� {"lltu", 4} �Ȃ� */
};

CConvertData::CConvertData(const char *buf1, const char *buf3){
	strcpy(m_str, buf1);
	m_len = strlen(buf1) - strlen(buf3);
	/* �����ł́A�f�[�^��M�p���āAbuf3��buf1�̌���ƈ�v���邱�Ƃ����肷��B */
}

bool CConvertData::match_front(const char *str){	/* �f�[�^�̑O����str����v���邩 */
	for(int i=0; str[i]!='\0'; i++){
		if(str[i] != m_str[i]){
			return false;
		}
	}
	return true;
}

bool CConvertData::match(const char *str){
	return (strcmp(m_str, str) == 0);
}

/* ============================================================ */

class CTrieNode{
public:
	CTrieNode();
	~CTrieNode();
	void insert(const char *str, const CConvertData &data);	/* str�ł��ǂ������data������ */
	CTrieNode *find(const char *str);
private:
	CTrieNode *m_next[256];
public:
	/* ����private�ɂ���̖ʓ| */
	vector<CConvertData> m_data;
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

void CTrieNode::insert(const char *str, const CConvertData &data){
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

/* ============================================================ */

struct Lyrics{
public:
	char ch;	/* �ϊ����ꂽ�����i�������A�B�`abc123+-�Ȃǁj��1�o�C�g */
	double timeJust,timeUntil;	/* ���傤�ǂ̎��ԁA���̕����ȑO������łƂ��ɁA�ł��x������ */
	bool isBlockStart;	/* 1���߁i�^�C�~���O��������邩���܂�j�̍ŏ� */
	bool isTyped;	/* ���łɑł��ꂽ�� */
	bool isScoringTarget;	/* ���݃^�C�~���O���������Ώۂł��邩 */
	double scoringTime;
		/* �^�C�~���O��������ꂽ�����AisBlockStart && !isScoringTarget�̂Ƃ��ȊO�͈Ӗ��͂Ȃ� */
	//int r, g, b;	/* �ł��n�߂̃G�t�F�N�g�̐F�iscoringTime���L���̎��A�L���j */
	double finishedTime;	/* Block���ł��؂�ꂽ�����A���Ȃ�܂��AisBlockStart�̂Ƃ��ȊO�͈Ӗ��͂Ȃ� */
	double x, y, vx, vy;
		/* �ifinishedTime���񕉂̎��j�ł��؂�ꂽ���̒��˂����i�����̓����_���j��ێ� */
public:
	bool isJapanese1(); 
};

bool Lyrics::isJapanese1(){
	if(this->ch & 0x80){
		return true;
	}else{
		return false;
	}
}

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
		}else if((*ptr2 & 0x80) == 0){
			*ptr1++ = *ptr2++;
		}else{
			*ptr1++ = *ptr2++;
			if(*ptr2 == '\0'){
				break;
			}
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
	else return GetColor(85, 85, 85);
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
public:
	void load(const char *fumenFileName, const char *rankingFileName);
	void unload();
	void setName(const char *name);
	void setChallenge(CChallenge &challenge);
private:
	void keyboard(char ch, double timeCount);
	void keyboard_1(char ch, double time);
	bool idle(double timeCount);
private:
	void loadRanking(const char *fileName);
	void unloadRanking();
	
	void setText(const char *str, int color);
	
	void setTime();
	void synchronizeTime(int soundHandle, double frequencyRate);
	double getTime();
	double getTime(double timeCount);
	
	void phase(int phaseNum);
	
	bool input(char *typeBuffer, int &typeBufferLen, vector<Lyrics>::iterator &lyricsPosition,
		double time, bool isCheckOnly);
	bool input_1(char *typeBuffer, int &typeBufferLen, vector<Lyrics>::iterator &lyricsPosition,
		double time, bool isCheckOnly, bool isSaiki);	/* input�̎��ۂɌv�Z���镔�� */
	
	void scoreTyping(vector<Lyrics>::iterator lyBegin, vector<Lyrics>::iterator lyEnd);
	void scoreAccuracy(double time, vector<Lyrics>::iterator lyricsPosition);
	
	void drawGaugeInit();
	void drawGauge();
	
	int getDrawPosX(double timeDiff);
	int getDrawPosY(int x);
	
	void draw();
	void drawResult();
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
	vector<Lyrics>::iterator m_lyricsPosition;
	
	vector<Lyrics>::iterator m_lyricsPositionEnd;
	/* ���ݑł��Ă���̎����ǂ��܂ł��A�܂�A[m_lyricsPosition, m_lyricsPositionEnd) */
	
	vector<Lyrics>::iterator m_lyricsDrawLeft;
	vector<Lyrics>::iterator m_lyricsDrawRight;
	/* draw�ŁA���O�ɕ`�悳�ꂽ�͈� */
	
	double m_timeLength;	/* ���ʂ́i���Ԃ́j���� */
	
	vector<LyricsKanji> m_lyricsKanji;
	vector<LyricsKanji>::iterator m_lyricsKanjiPosition;
	
	vector<BeatLine> m_beatLine;
	vector<BeatLine>::iterator m_beatLineDrawLeft;
	vector<BeatLine>::iterator m_beatLineDrawRight;
	
	char m_typeBuffer[TYPE_BUFFER_LEN + 1];
	int m_typeBufferLen;
	
	int m_phase;
	
	double m_timeBegin;
	
	char m_name[NAME_LEN + 1];	/* �v���[���[�̖��O�i�ݒ肳��Ă�����j */
	
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
	
	CRanking m_ranking;	/* �n�C�X�R�A�̃f�[�^ */
	int m_rank;	/* �n�C�X�R�A�̒��ł̏��ʁA�����N�C�����Ȃ���� -1 */
	
	bool m_isReplay;
	vector<ReplayData> m_replayData;
	
	int m_scoreDraw;	/* �\������X�R�A�i��1/10�j�A�{���̃X�R�A�Ɣ�גx�������� */
	
	CEffect1 m_effect1;	/* �L�[���̓G�t�F�N�g */
	
	double m_gauge[4];	/* �Q�[�W�̌��ݕ`����钷�� */
	double m_gaugeRate;	/* ���̒���1�ɑ��������ʏ�̒��� */
	double m_gaugeLight[4];	/* �Q�[�W�̂��ꂼ����Ƃ炷���̋��� */
	
	char m_text[256];	/* ����i����уR���{���j��\�� */
	int m_textColor;
	double m_textTime;
	
	CChallenge m_challenge;
	
	char m_musicFileName[256];
	
	int m_idleCounter;	/* idle������ڂ̌Ăяo������m�邽�߂̃J�E���^ */
	
	int m_fontHandleNormal;
	int m_fontHandleBig;
	
	int m_soundHandleMusic;
};

CTyping::CTyping(){
	/* ���[�}�������̓R���X�g���N�^�œǂ� */
	if(!g_config.f_debugMode){	/* �ʏ�̂Ƃ� */
		loadDictionary("convert.dat");
	}else{	/* �f�o�b�O�� */
		char buf1[256], buf2[256], buf3[256];
		strcpy(buf2, "+");
		strcpy(buf3, "");
		for(char ch = ' ' + 1; ch < 127; ch++){
			buf1[0] = ch;
			buf1[1] = '\0';
			m_trie.insert(buf2, CConvertData(buf1, buf3));
		}
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
		m_trie.insert(buf2, CConvertData(buf1, buf3));
		/* ���{����L�[�Ƀ��[�}���̃f�[�^������ */
	}
	fclose(fp);
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
	m_lyricsKanji.clear();
	m_beatLine.clear();
	strcpy(m_musicFileName, "");
	
	/* �̎��̂����܂�̐��i=m_count�����̘a�̍ő�l�j�𐔂��邽�߂�0�� */
	m_countAll = 0;
	
	double frequencyRate = m_challenge.frequencyRate();
	/* KeyShift�ɂ����g���̔{�� */
	
	/* �ǂݍ��݊J�n */
	
	char tmpBuf[256];
	char buf[256], bufLast[256];
	strcpy(bufLast,"");
	double time, timeLast;
	bool flag = true;
	while(flag){
		if(fgetline(tmpBuf, fp) == NULL){
			m_timeLength = timeLast;	/* ���ʏI���������L�^ */
			flag = false;
			time = INFTY;
		}else{
			int n;
			switch(tmpBuf[0]){
			case '+':	/* �ł̎� */
				if(g_config.f_debugMode && g_config.f_debugBeat){
					/* �ł̎��͂��ׂĖ��� */
					continue;
				}
				n = sscanf(tmpBuf + 1, "%lf%s", &time, buf);
				if(n < 2){
					throw "[����] �������s���ł��B(+)";
				}
				time /= frequencyRate;
				if(g_config.f_debugMode){
					/* �̎���"+"�Ƃ��� */
					strcpy(buf, "+");
				}
				break;
			case '*':	/* �\������̎� */
				{
					char *ptr = strtok(tmpBuf + 1, " \t\n");	/* �X�y�[�X�ŋ�؂���܂ł̕��������o�� */
					if(ptr == NULL){
						throw "[����] �������s���ł��B(*)";
					}
					n = sscanf(ptr, "%lf", &time);	/* ������ǂ� */
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
				continue;
			case '=':	/* ���ߐ� */
			case '-':	/* ���� */
				{
					n = sscanf(tmpBuf + 1, "%lf", &time);	/* ������ǂ� */
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
				}
				if(g_config.f_debugMode && g_config.f_debugBeat){
					strcpy(buf, "+");
					/* ���q����"+"�̑ł̎��Ƃ��� */
					break;
				}
				continue;
			case '/':	/* ��؂� */
				n = sscanf(tmpBuf + 1, "%lf", &time);
				if(n < 1){
					throw "[����] �������s���ł��B(/)";
				}
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
				
				strcpy(buf, " ");
				break;
			case '@':
				sprintf(m_musicFileName, "%s%s", directoryName, tmpBuf + 1);	/* �Đ����鉹�y�t�@�C������ݒ� */
				continue;
			default:
				continue;
			}
		}
		Lyrics ly;
		ly.isBlockStart = true;
		ly.finishedTime = -INFTY;
		ly.isScoringTarget = true;
		for(char *ptr=bufLast; *ptr!='\0'; ptr++){
			ly.ch = *ptr;
			ly.timeJust = timeLast;
			ly.timeUntil = time;	/* ���̉̎���timeJust�܂� */
			if(ly.ch == ' '){
				ly.timeUntil = -INFTY;
				ly.isTyped = true;	/* ��؂�̕����͂��łɑł����Ƃ������� */
				ly.isScoringTarget = false;	/* ��؂���u�ł��ē��_�𓾂�v���Ƃ͂ł��Ȃ� */
			}else{
				ly.isTyped = false;
			}
			m_lyrics.push_back(ly);
			
			if(ly.isScoringTarget){	/* ����̂���̎��̐��𐔂��� */
				m_countAll++;
			}
			
			ly.isBlockStart = false;	/* �ŏ��̕����݂̂� true �ɂ��邽�� */
			ly.isScoringTarget = false;
		}
		timeLast = time;
		strcpy(bufLast, buf);
	}
	fclose(fp);
	/* �ł̎��̔ԕ��Ƃ��čŌ��' ',INFTY��ǉ����Ă��� */
	{
		Lyrics ly;
		ly.ch = ' ';
		ly.timeJust = INFTY;
		ly.timeUntil = INFTY;
		ly.isBlockStart = true;
		ly.isTyped = true;
		ly.isScoringTarget = false;
		m_lyrics.push_back(ly);
		
		/* ' '�Ȃ�1�i�ނ��Ƃւ̑΍�Ƃ��āA'\n'��ǉ����Ă��� */
		ly.ch = '\n';
		m_lyrics.push_back(ly);
	}
	
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
	m_lyricsPosition = m_lyrics.begin();
	m_lyricsKanjiPosition = m_lyricsKanji.begin();
	
	m_lyricsPositionEnd = m_lyricsPosition;	/* �ŏ��͔͈͂Ȃ��Ƃ������Ƃɂ��Ă����� */
	
	m_lyricsDrawLeft = m_lyrics.begin();
	m_lyricsDrawRight = m_lyrics.begin();
	/* �ŏ���draw�Őݒ肳��邩�牽�����Ă��悢�̂����A */
	/* �E�ɂ����i�܂Ȃ��̂ōŏ��ɂ��Ă����Ȃ���΂Ȃ�Ȃ� */
	
	/* ���R�͓��� */
	m_beatLineDrawLeft = m_beatLine.begin();
	m_beatLineDrawRight = m_beatLine.begin();
	
	/* �L�[���͂������� */
	m_typeBufferLen = 0;
	
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
	
	strcpy(m_text, "");
	
	m_scoreDraw = 0;
	
	m_effect1.clear();	/* �L�[���̓G�t�F�N�g�������� */
	
	drawGaugeInit();	/* �Q�[�W������ */
	
	loadRanking(rankingFileName);
	/* �����L���O�ǂݍ��� */
	
	m_soundHandleMusic = -1;
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
		if(time < 2.0){	/* �I������̌둀���h�~ */
			return;
		}
#if 0
		if(ch < CTRL_CODE_CMP){	/* �����R�[�h�łȂ��Ƃ� */
			switch(ch){
			case CTRL_CODE_CR:	/* ���s�Ȃ�m�� */
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
			case CTRL_CODE_BS:	/* BackSpace�⍶�L�[��Delete�Ȃ� */
			case CTRL_CODE_LEFT:
			case CTRL_CODE_DEL:
				if(m_typeBufferLen > 0){
					m_typeBufferLen--;	/* �ꕶ���폜 */
				}
				break;
			}
			return;
		}
#endif
		if(ch == ' '){	/* �X�y�[�X�̓A���_�[�o�[�ɕϊ��i�X�y�[�X�݂̂̈Ⴂ�����Ȃ����߁j */
			ch = '_';
		}
		int ret;
		if(strlen(m_name) > 0){
			char buf[1];
			int len = 0;
			ret = editBuffer(ch, buf, len);	/* �_�~�[�𑀍� */
			/* Enter�������ꂽ���ǂ����݂̂��d�v */
		}else{
			ret = editBuffer(ch, m_typeBuffer, m_typeBufferLen);
		}
		switch(ret){
		case EDIT_BUFFER_OK:
			if(m_challenge.isEasy() || m_isReplay || g_config.f_debugMode){
				/* �ȒP�ɂȂ�I�v�V�������g�p�����ꍇ�A�����L���O�ɂ̂��Ȃ� */
				/* ���v���C����f�o�b�O���͋L�^���Ȃ� */
				phase(PHASE_FINISHED);
			}else{	/* ���v���C���E�f�o�b�O���[�h�� */
				if(strlen(m_name) > 0){
					strcpy(m_typeBuffer, m_name);
					phase(PHASE_FINISHED);
				}
				if(m_typeBufferLen > 0){	/* ���O�����͂���Ă��邩�m�F */
					phase(PHASE_FINISHED);
				}
			}
			break;
		case EDIT_BUFFER_CANCEL:	/* �y���̉\���͂Ȃ��͂��z */
			break;
		}
		return;
#if 0
		if(m_typeBufferLen < TYPE_BUFFER_LEN){
			m_typeBuffer[m_typeBufferLen++] = ch;
		}
		return;
#endif
	}
	if(m_phase == PHASE_FINISHED){	/* �����L���O�\�����Ȃ� */
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
	if(ch == '\0' || ch == ' '){	/* ���낢�덢�肻�������A�łӖ����Ȃ����� */
		return;
	}
	
	if(time < (*m_lyricsPosition).timeJust - SEC_POOR){
		/* poor���O�Ȃ̂őłĂȂ� */
		return;
	}
	
	m_typeBuffer[m_typeBufferLen++] = ch;
	
	if(input(m_typeBuffer, m_typeBufferLen, m_lyricsPosition, time, false)){
	/* ���̓��͂����݂̈ʒu�œ����� */
		if(!m_isReplay){	/* ���v���C�ɋL�^ */
			ReplayData rd;
			rd.ch = ch;
			rd.time = time;
			m_replayData.push_back(rd);
		}
		/* �L�[���̓G�t�F�N�g */
		m_effect1.insert(X_CIRCLE, Y_CIRCLE, ch, GetColor(170, 170, 255), time);
		
		while((*m_lyricsPosition).ch == ' '){	/* �̎��̐؂�ڂ��w���Ă���Ȃ� */
			m_lyricsPosition++;	/* �i�߂� */
		}
		return;
	}
	m_typeBufferLen--;
	char tmpBuffer[TYPE_BUFFER_LEN + 1];
	int tmpLen;
	vector<Lyrics>::iterator tmpLyricsPosition;
	tmpBuffer[0] = ch;
	tmpLen = 1;
	tmpLyricsPosition = m_lyricsPosition;
	
	int comboBackUp = m_combo;	/* m_combo��ۑ����Ă��� */
	m_combo = 0;
	while(1){
		tmpLyricsPosition++;	/* �K��1�ȏ��΂��K�v������̂ōŏ���++ */
		if(!(*tmpLyricsPosition).isBlockStart){
			continue;
		}
		if(time >= (*tmpLyricsPosition).timeUntil){
			/* ��΂��āA���A���Ԑ؂�͑łĂȂ� */
			/* �����ł͂����Ȃ��Ă��Ainput�ł͂����邪�A�����̏d�����C�ɂȂ肻�� */
			continue;
		}
		if(time < (*tmpLyricsPosition).timeJust - SEC_POOR){
			/* poor�͈͂ɓ����Ă���Ƃ�����O�ɒT�����o�� */
			break;
		} 
		if(input(tmpBuffer, tmpLen, tmpLyricsPosition, time, false)){
		/* �V�����ʒu�ł��̓��͂������� */
			if(!m_isReplay){	/* ���v���C�ɋL�^ */
				ReplayData rd;
				rd.ch = ch;
				rd.time = time;
				m_replayData.push_back(rd);
			}
			/* �L�[���̓G�t�F�N�g */
			m_effect1.insert(X_CIRCLE, Y_CIRCLE, ch, GetColor(170, 170, 255), time);
			
			/* �V�����f�[�^���������� */
			strcpy(m_typeBuffer, tmpBuffer);
			m_typeBufferLen = tmpLen;	/* 1�Ƃ͌���Ȃ��͂��Ȃ̂� */
			m_lyricsPosition = tmpLyricsPosition;
			while((*m_lyricsPosition).ch == ' '){	/* �̎��̐؂�ڂ��w���Ă���Ȃ� */
				m_lyricsPosition++;	/* �i�߂� */
			}
			return;	/* m_combo�͌��ɖ߂��Ȃ� */
		}
	}
	m_combo = comboBackUp;	/* m_combo�����ɖ߂� */
	return;
}

bool CTyping::idle(double timeCount){	/* ���Ȃ���� true ��Ԃ� */
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
	if(m_phase != PHASE_MAIN){	/* ���������̓Q�[�����C���̂Ƃ��̏��� */
		return true;
	}
	
	double time = getTime(timeCount);
	bool flag = false;	/* m_lyricsPositionEnd���ω��������A */
	/* �܂�A�łĂȂ��Ȃ������Ƃ̍ă`�F�b�N���K�v�� */
	
	if(m_lyricsPositionEnd < m_lyricsPosition){
		m_lyricsPositionEnd = m_lyricsPosition;
		flag = true;
	}
	
	/* �܂��A���傤�ǂ̈ʒu�ɒB���Ă��Ȃ��ŏ��̉̎��܂Ői�� */
	while(time >= (*m_lyricsPositionEnd).timeJust){
		m_lyricsPositionEnd++;
		flag = true;
	}
	
	if(flag){	/* ����`�F�b�N����Əd����������Ȃ��̂ŁA�K�v�ȂƂ��Ɍ��肷�� */
		/* �u�����̃L�[�������Ό��݂̈ʒu�œ��͉\�v�ł͂Ȃ� */
		if(!input(m_typeBuffer, m_typeBufferLen, m_lyricsPosition, time, true)){
			/* ���͂��؂ꂽ���͔̂�΂��im_lyricsPosition���i��ŁA�R���{���؂��j */
			if(time >= (*m_lyricsPosition).timeUntil){
				/* �킴�킴�i�ނ��ǂ����`�F�b�N���Ă���̂́A�Ō��'\n'�ɂ���Ƃ����������� */
				do{
				m_lyricsPosition++;
				}while(time >= (*m_lyricsPosition).timeUntil);
				/* �{����m_lyricsPosition���i�񂾂�A�����ɗ��� */
				m_combo = 0;
				m_typeBufferLen = 0;
				/* ���͒��̂͂����g���邱�Ƃ͂Ȃ��i����A�����ŏ����Ă��܂��Ă悢�j */
			}
		}
	}
	
	if(time >= m_timeLength){	/* �Ō�̕��ʂ��ʉ� */
		phase(PHASE_RESULT);
		return true;
	}
	
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
	/* ���̖��͍�҂̃��C�u�����C���ɂ��������ꂽ */
//	if(g_config.f_debugMode && g_config.debugTime > 0){	/* �r������n�߂�f�o�b�O���[�h�̂Ƃ� */
//		musicTime = ((GetSoundCurrentTime(soundHandle) + g_config.debugTime) 
//			/ 1000.0) / frequencyRate;
//	}else{
//		/* �ʏ�A�y �o�O���Ȃ���΂�����������ł悢�͂� �z */
	musicTime = (GetSoundCurrentTime(soundHandle) / 1000.0) / frequencyRate;
//	}
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

double CTyping::getTime(){	/* �J�n��������̌o�ߕb���擾 */
	return myGetNowHiPerformanceCount() - m_timeBegin;
}

double CTyping::getTime(double timeCount){	/* timeCount�̊J�n��������̌o�ߕb���擾 */
	return timeCount - m_timeBegin;
}

/* ------------------------------------------------------------ */

void CTyping::phase(int phaseNum){
	if(phaseNum == PHASE_RESULT){	/* �I�����āA�X�R�A�\���� */
		setTime();	/* �\���J�n����Ă���̎��Ԃ𗘗p���邽�߂ɃZ�b�g */
		m_typeBuffer[0] = '\0';	/* ���O���͂Ɏg���̂Ńo�b�t�@���N���A */
		m_typeBufferLen = 0;
		
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
			m_phase = PHASE_EXIT;
			return;
		}else{
			CScore score(m_typeBuffer, m_score, m_scoreAccuracy, m_scoreTyping,
				m_countExcellent, m_countGood, m_countFair, m_countPoor, m_countPass,
				m_countAll, m_comboMax, m_challenge);
			m_rank = m_ranking.update(score, g_config.f_rankingCheckDate, g_config.f_rankingCheckChallenge);
			if(m_rank >= 0){
				m_ranking.write();
			}else{
				m_phase = PHASE_EXIT;	/* �����N�C�����Ȃ���ΏI�� */
				return;
			}
		}
	}
	m_phase = phaseNum;
}

/* ------------------------------------------------------------ */

bool CTyping::input(char *typeBuffer, int &typeBufferLen, vector<Lyrics>::iterator &lyricsPosition,
		double time, bool isCheckOnly){
	typeBuffer[typeBufferLen] = '\0';
	/* typeBufferLen���u���ۂɑł��������v�Ƃ��ėp���邽�� */
	/* ����������Ă����Ȃ��ƃo�b�t�@�I�[�o�[�������N���� */
	
	if(isCheckOnly){
		int tmpTypeBufferLen = typeBufferLen + 1;	/* typeBufferLen�͍X�V���Ȃ����Ȃ� */
		/* ���ۂɑł���������1�������邱�Ƃɂ���āA�C�ӂ�1��������ɑł����Ƃ��Ď��Ԑ����𔻒肷�� */
		return input_1(typeBuffer, tmpTypeBufferLen, lyricsPosition, time, true, false);
		/* �Ō��false�͍ċA�łȂ����Ƃ������B */
	}else{
		return input_1(typeBuffer, typeBufferLen, lyricsPosition, time, false, false);
	}
}

bool CTyping::input_1(char *typeBuffer, int &typeBufferLen, vector<Lyrics>::iterator &lyricsPosition,
		double time, bool isCheckOnly, bool isSaiki){
/* isSaiki�͊O����̌Ăяo���ł�false,������̌Ăяo���ł�true */
	if(isSaiki && strlen(typeBuffer) == 0){
	/* ���m�胍�[�}�����Ȃ��̂ɍċA���� */
		return true;
	}
	vector<Lyrics>::iterator tmpLyricsPosition = lyricsPosition;
	CTrieNode *trie = &m_trie;
	while(1){
		char buf[2];
		buf[0] = (*tmpLyricsPosition).ch;
		tmpLyricsPosition++;
		buf[1] = '\0';
		trie = trie->find(buf);	/* ���{��1�o�C�g�i�� */
		if(trie == NULL){	/* trie�����ǂ�Ȃ��A�܂�A�܂Ƃ߂đłĂ�͈͂𒴂����B */
			return false;
		}
		for(vector<CConvertData>::iterator i = trie->m_data.begin();
				i != trie->m_data.end(); i++){	/* ���[�}������T�� */
			if((*i).match_front(typeBuffer)){
				int tmpLen = typeBufferLen - (*i).m_len;
				/* ����ϊ�����镔���̎��̕�������ɑ΂��Ď��ۂɑł���Ă��镔���̒����B */
				/* ����͕��Ƃ��ɂȂ肤��B */
				if(tmpLen <= 0){	/* ����ɕK�v�Ȃ�L�[�𑫂��Ĉ�x�ɑłĂ�͈͂͂����܂� */
				/* ����́A{��΁c�c,nb�c�c}��n�����ł����󋵂Ȃǂ��܂܂��B */
					if(time >= (*(tmpLyricsPosition-1)).timeUntil){
						/* ���̍Ō�̕��������Ԑ؂ꂵ�Ă��Ă͂����Ȃ� */
						continue;
					}
				}
				char tmpTypeBuffer[TYPE_BUFFER_LEN + 1];
				strcpy(tmpTypeBuffer, (*i).m_str + (*i).m_len);	/* ���m�胍�[�}���ɂȂ�\��̕��� */
				if(input_1(tmpTypeBuffer, tmpLen, tmpLyricsPosition, time, isCheckOnly, true)){
				/* �ċA�̌��ʑłĂ邱�Ƃ����������Ƃ� */
					if(isCheckOnly){	/* ���ׂ邽�߂̂Ƃ��́A�������Ȃ��B */
						return true;
					}
					/* ���ׂ邽�߂ł͂Ȃ��A���ۂɑł��Ă���Ƃ� */
					if(typeBufferLen >= 1){
					/* �V�������߂̑ł��n�� */
						scoreAccuracy(time, lyricsPosition);
						/* ���傤�ǂ̃^�C�~���O�Ƃ̂��ꂩ��v�Z�������_��^����B */
					}
					
					/* �u���v�̏����A�S�Ă�3�o�C�g�ȏ�ւ̕ϊ��Ɍ��ʂ�����̂Œ��� */
					/* �y �Ă��A����́Aconvert.dat���g�����A�����ŗ�O�������Ȃ��Ă��ςނ悤�ɂ����� �z */
					if(typeBufferLen >= 2 && lyricsPosition + 2 < tmpLyricsPosition){
					/* 2�����ł��āA2�o�C�g��(���{���1������)�������x�ɑłĂ�͈͂ŁA����ɍ̓_�Ώۂł��� */
					/* �܂�A�u�����v�ɂ�����"kk"�̎��_�Łu���v���̓_�����B */
						scoreAccuracy(time, lyricsPosition + 2);
						/* ���傤�ǂ̃^�C�~���O�Ƃ̂��ꂩ��v�Z�������_��^����B */
					}
					if((*i).match(typeBuffer)){	/* ���S��v */
						/* �ϊ����ꂽ�̎��̕��������_��^���� */
						if(!isSaiki){
						/* �ċA��2�i�ڈȍ~�ł��^�C�s���O�_���v�Z�����2�d�ɂȂ�̂ŁA���ڂ̌Ăяo���̂ݏ��� */
							scoreTyping(lyricsPosition, tmpLyricsPosition);
						}
						
						for(vector<Lyrics>::iterator i = lyricsPosition; i != tmpLyricsPosition; i++){
							(*i).isTyped = true;	/* �ł����̎��͈̔͂��L�^ */
						}
						lyricsPosition = tmpLyricsPosition;	/* �V�����̎��̈ʒu�� */
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

void CTyping::scoreTyping(vector<Lyrics>::iterator lyBegin, vector<Lyrics>::iterator lyEnd){
/* [lyBegin, lyEnd)��ł����Ƃ��̓��_ */
	bool isJapanese2 = false;
	for(vector<Lyrics>::iterator i = lyBegin; i != lyEnd; i++){
		if(isJapanese2){
			isJapanese2 = false;
		}else{
			m_scoreTyping += SCORE_TYPING;
			if((*i).isJapanese1()){	/* ���{���1�o�C�g�ڂȂ� */
				isJapanese2 = true;
			}
		}
	}
	m_score = m_scoreTyping + m_scoreAccuracy;
}

void CTyping::scoreAccuracy(double time, vector<Lyrics>::iterator lyricsPosition){
	if(!(*lyricsPosition).isScoringTarget){
	/* �^�C�~���O�̍̓_�Ώۂł͂Ȃ��A�܂��́A�����̓_���ꂽ */
		return;
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
		
		m_gaugeLight[0] = 1.0;
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
		
		m_gaugeLight[1] = 1.0;
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
		
		m_gaugeLight[2] = 1.0;
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
		
		m_gaugeLight[3] = 1.0;
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

void CTyping::drawGaugeInit(){
	for(int i=0; i<4; i++){	/* �ŏ��̒l */
		m_gauge[i] = 0.0;
		m_gaugeLight[i] = 0.0;
	}
	m_gaugeRate = 20.0;	/* �ŏ��́A����1������̕\�����钷�� */
}

void CTyping::drawGauge(){	/* �Q�[�W��\�� */
	int y = Y_GAUGE;
	int h = H_GAUGE / 4;
	{
		double x[4];
		x[0] = m_countExcellent;
		x[1] = m_countGood;
		x[2] = m_countFair;
		x[3] = m_countPoor;
		for(int i=0; i<4; i++){	/* �Q�[�W�̎w���ׂ��l�ɋ߂Â��� */
			m_gauge[i] += (x[i] - m_gauge[i]) * 0.2;
		}
	}
	
	double x[4];	/* m_gauge[]����g�O����������x[]�ɓ���� */
	double rate = 20.0;
	while(1){
		bool flag = true;
		for(int i=0; i<4; i++){
			if(m_gauge[i] * rate > W_GAUGE){
				flag = false;
				//rate = W_GAUGE / x[i];
			}
		}
		if(flag){
			break;
		}
		/* �����邽�т�0.75�{�����Ă����A�����Ȃ��Ȃ�܂ł�� */
		rate *= 0.75;
		/*
		for(int i=0; i<4; i++){
			x[i] *= 0.75;
		}
		*/
	}
	m_gaugeRate += (rate - m_gaugeRate) * 0.2;
	for(int i=0; i<4; i++){
		x[i] = m_gauge[i] * m_gaugeRate;
	}
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);	/* �s������1/2 */
	DrawBox(X_GAUGE - W_GAUGE, y, X_GAUGE, y + 4 * h, GetColor(64, 64, 64), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 224);	/* �s������7/8 */
	//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	/* ���ꂼ��̒�����\�� */
	DrawBox((int)(X_GAUGE - x[0]), y        , X_GAUGE, y +     h, COLOR_EXCELLENT, TRUE);
	DrawBox((int)(X_GAUGE - x[1]), y +     h, X_GAUGE, y + 2 * h, COLOR_GOOD, TRUE);
	DrawBox((int)(X_GAUGE - x[2]), y + 2 * h, X_GAUGE, y + 3 * h, COLOR_FAIR, TRUE);
	DrawBox((int)(X_GAUGE - x[3]), y + 3 * h, X_GAUGE, y + 4 * h, COLOR_POOR, TRUE);
	
	/* �ŋߕύX���ꂽ���̂��Ƃ炷 */
	{
		double Light = m_gaugeLight[0];
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(192 * Light));
		Light *= 4.0;
		if(Light > 1.0){
			Light = 1.0;
		}
		int dh = (int)(h * 0.5 * (1.0 - Light)*(1.0 - Light));
		DrawBox(X_GAUGE - W_GAUGE, y         + dh, X_GAUGE, y +     h - dh, COLOR_EXCELLENT2, TRUE);
	}
	{
		double Light = m_gaugeLight[1];
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(192 * Light));
		Light *= 4.0;
		if(Light > 1.0){
			Light = 1.0;
		}
		int dh = (int)(h * 0.5 * (1.0 - Light)*(1.0 - Light));
		DrawBox(X_GAUGE - W_GAUGE, y +     h + dh, X_GAUGE, y + 2 * h - dh, COLOR_GOOD2, TRUE);
	}
	{
		double Light = m_gaugeLight[2];
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(192 * Light));
		Light *= 4.0;
		if(Light > 1.0){
			Light = 1.0;
		}
		int dh = (int)(h * 0.5 * (1.0 - Light)*(1.0 - Light));
		DrawBox(X_GAUGE - W_GAUGE, y + 2 * h + dh, X_GAUGE, y + 3 * h - dh, COLOR_FAIR2, TRUE);
	}
	{
		double Light = m_gaugeLight[3];
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(192 * Light));
		Light *= 4.0;
		if(Light > 1.0){
			Light = 1.0;
		}
		int dh = (int)(h * 0.5 * (1.0 - Light)*(1.0 - Light));
		DrawBox(X_GAUGE - W_GAUGE, y + 3 * h + dh, X_GAUGE, y + 4 * h - dh, COLOR_POOR2, TRUE);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	for(int i=0; i<4; i++){
		m_gaugeLight[i] *= 0.85;
	}
}

/* ------------------------------------------------------------ */

/* timeDiff: ���傤�ǂ̈ʒu�̉��b�ォ */
int CTyping::getDrawPosX(double timeDiff){
	/* CIRCLE_SPEED * m_challenge.speed() �������A�i�������A�������j */
	return X_CIRCLE + (int)(-timeDiff * (CIRCLE_SPEED * m_challenge.speed()));
}

int CTyping::getDrawPosY(int x){
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
		
		while((*m_lyricsDrawLeft).timeJust != INFTY){
			if(!(*m_lyricsDrawLeft).isBlockStart){	/* ���߂̍ŏ��łȂ��Ȃ�i�߂Ă悢 */
				m_lyricsDrawLeft++;
				continue;
			}
			double timeDiff = time - (*m_lyricsDrawLeft).timeJust;
			int posX = getDrawPosX(timeDiff);
			if(posX >= -(R_CIRCLE + 300)){
				/* �~���͂ݏo��̎���A�ł��؂��Ē��˂邱�Ƃ��l�����čL�߂Ɏ�� */
				/* ��ʂ���o�Ă��Ȃ� */
				break;
			}
			/* ���łɉ�ʂ���o�Ă��܂����̂ŁA�`��Ώۂ���폜 */
			m_lyricsDrawLeft++;
		}
		
		while((*m_lyricsDrawRight).timeJust != INFTY){
			if(!(*m_lyricsDrawRight).isBlockStart){	/* ���߂̍ŏ��łȂ��Ȃ�i�߂Ă悢 */
				m_lyricsDrawRight++;
				continue;
			}
			double timeDiff = time - (*m_lyricsDrawRight).timeJust;
			int posX = getDrawPosX(timeDiff);
			if(posX >= W_WINDOW + R_CIRCLE){
				/* ��ʂɓ����Ă��Ă��Ȃ� */
				break;
			}
			/* �V������ʂɓ����Ă����̂ŁA�`��Ώۂɒǉ� */
			m_lyricsDrawRight++;
		}
		
		for(vector<Lyrics>::iterator i = m_lyricsDrawRight; i != m_lyricsDrawLeft;){
			i--;	/* [Left, Right)�Ȃ̂ŁA�����Ńf�N�������g */
			if(!(*i).isBlockStart){	/* ���߂̍ŏ��łȂ��Ȃ疳�� */
				continue;
			}
			double timeDiff = time - (*i).timeJust;	/* ���傤�Ǒłʒu�ɂȂ��Ă��牽�b�ォ */
			int posX = getDrawPosX(timeDiff);
			int posY = getDrawPosY(posX);
			
			if((*i).ch == ' '){	/* �̎��̐؂�� */
				if(timeDiff < 0.0){	/* �؂�ڂ͔���ʒu�ɗ�����O�̂Ƃ������\�� */
					DrawCircle(posX, Y_CIRCLE + posY, R_CIRCLE, GetColor(128, 128, 128), FALSE);
					/* �D�F�̉~�𗬂� */
				}
				continue;
			}
			
			char buf[256];
			int len = 0;
			{
				vector<Lyrics>::iterator j = i;
				do{	/* �u���b�N���̖��^�C�v�̕�����buf�Ɋi�[ */
					if(!(*j).isTyped){
						buf[len++] = (*j).ch;
					}
					j++;
				}while(!(*j).isBlockStart);
			}
			buf[len] = '\0';
			if(len == 0){	/* ���ׂă^�C�v����Ă��� */
				if((*i).finishedTime < 0){
					/* ������ۑ� */
					(*i).finishedTime = time;
					/* �ʒu��ۑ� */
					(*i).x = posX;
					(*i).y = posY;
					double arg = (GetRand(32) - 16) / 20.0;
					/* -0.8�ȏ�0.8�ȉ�0.05���݂Ń����_�� */
					(*i).vx = 100.0 * sin(arg);
					(*i).vy = 100.0 * cos(arg);
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
					
#if 0
					double r,g,b,a;
					/* ���ʂɕω��������Ƃ��̐F�� */
					r = 255 * (1.0-sTime) * (1.0-sTime);	//(1.0-sTime*sTime);
					g = 0.0;
					b = 255 * sTime*(2.0-sTime);
					/* �G�t�F�N�g�̐F���d�˂� */
					a = (1.0-sTime)*(1.0-sTime);
					r += a * ((*i).r - r);
					g += a * ((*i).g - g);
					b += a * ((*i).b - b);
					Color = GetColor((int)r, (int)g, (int)b);
#endif
				}else{
					Color = GetColor(0, 0, 255);
				}
			}
			{
				if((*i).finishedTime >= 0.0){
					double fTime = (time - (*i).finishedTime)/0.25;
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
				}
				DrawCircle(posX, Y_CIRCLE + posY, R_CIRCLE - 1, Color, TRUE);	/* �����~ */
				DrawCircle(posX, Y_CIRCLE + posY, R_CIRCLE, GetColor(0, 0, 0), FALSE);	/* �����~�̗֊s */
#if 0
#define R_CIRCLE2 (R_CIRCLE - 4)
				DrawCircle(posX, Y_CIRCLE + posY, R_CIRCLE - 1, GetColor(255, 255, 255), TRUE);	/* �����~ */
				DrawCircle(posX, Y_CIRCLE + posY, R_CIRCLE2 - 1, Color, TRUE);	/* �����~ */
#endif
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
		/* ���ܑΏۂɂȂ��Ă��郍�[�}���̎�(m_lyricsPosition����m_lyricsPositionEnd�܂�)��\�� */
		/* Hidden �� Stealth �̂Ƃ��A�q���g�ɂȂ�̂�����邽�߁A */
		/* ������ł܂ł͗�O�I�ɕ\������Ȃ� */
		if(!( (m_challenge.test(CHALLENGE_HIDDEN) || m_challenge.test(CHALLENGE_STEALTH))
				&& m_typeBufferLen == 0 )){
			char buf[256];
			int len = 0;
			for(vector<Lyrics>::iterator i = m_lyricsPosition; i != m_lyricsPositionEnd; i++){
				if((*i).ch != ' '){
					buf[len++] = (*i).ch;
				}
			}
			buf[len] = '\0';
			//int strWidth = GetDrawStringWidthToHandle(buf, len, m_fontHandleBig);
			//m_effect1.insert(X_LYRICS_BIG, Y_LYRICS_BIG, buf, GetColor(255, 255, 255), time);
			//������Ȃ��قȂ��Ƃ���Ȃ��Ă΁A�����B
			DrawStringToHandle(X_LYRICS_BIG, Y_LYRICS_BIG, buf,
				GetColor(255, 255, 255), m_fontHandleBig);
		}
	}
	
	/* �^�C�v����������\�� */
	{
		m_typeBuffer[m_typeBufferLen] = '\0';
		//m_effect1.insert(X_BUFFER, Y_BUFFER, m_typeBuffer, GetColor(255, 255, 255), time);
		//������Ȃ��قȂ��Ƃ���Ȃ��Ă΁A�����B
		//int strLen = strlen(m_typeBuffer);
		//int strWidth = GetDrawStringWidthToHandle(m_typeBuffer, strLen, m_fontHandleBig);
		DrawStringToHandle(X_BUFFER, Y_BUFFER, m_typeBuffer,
			GetColor(255, 255, 255), m_fontHandleBig);
	}
	
	drawGauge();
	
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
			DrawFormatStringToHandle(30, 165, GetColor(255, 255, 255), m_fontHandleBig,
				"�ő� %d �R���{", m_comboMax);
		}else{
			DrawFormatStringToHandle(30, 165, GetColor(255, 255, 0), m_fontHandleBig,
				"�t���R���{");
		}
	}
	if(time >= 2.6){
		DrawFormatStringToHandle(30, 235, GetColor(255, 255, 255), m_fontHandleBig,
			"���_ :");
	}
	if(time >= 2.9){
		DrawFormatStringToHandle(320, 235, GetColor(255, 255, 255), m_fontHandleNormal,
			"�� : %7d �_", m_scoreAccuracy);
	}
	if(time >= 3.2){
		DrawFormatStringToHandle(320, 260, GetColor(255, 255, 255), m_fontHandleNormal,
			"�� : %7d �_", m_scoreTyping);
	}
	if(time >= 4.0){
		DrawFormatStringToHandle(30, 310, GetColor(255, 255, 255), m_fontHandleBig,
			"�����_ : %8d �_", m_score);
	}
	if(!(m_challenge.isEasy() || m_isReplay || g_config.f_debugMode)){	/* �ʏ펞�̂� */
		/* �ȒP�ɂȂ�I�v�V�������g�p�����ꍇ�⃊�v���C����f�o�b�O���@�łȂ���΁A */
		/* �����L���O�ɂ���邽�߁A���O����́B */
		if(strlen(m_name) > 0){	/* ���O���ݒ肳��Ă���ꍇ�A�����\�� */
			DrawStringToHandle(60, 400, m_name, GetColor(255, 255, 255), m_fontHandleBig);
		}else if(time >= 2.0){
			/* ���O���ݒ肳��ĂȂ��ꍇ�A��莞�Ԃ��o��������͊J�n */
			DrawFormatStringToHandle(30, 375, GetColor(255, 255, 255), m_fontHandleNormal,
				"���O����͂��Ă������� :");
			DrawStringToHandle(60, 400, m_typeBuffer, GetColor(255, 255, 255), m_fontHandleBig);
			/* ���͂��ꂽ�������\�� */
		}
	}
	if(m_phase == PHASE_FINISHED){
		if(m_rank >= 0){
			int strWidth;
			strWidth = GetDrawFormatStringWidthToHandle( m_fontHandleBig,
				"%d �ʂɃ����N�C�����܂���", m_rank + 1);
			int boxWidth = strWidth + 100;
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
			if(ch == CTRL_CODE_ESC){	/* Esc�ŃQ�[���𒆒f */
				goto L1;
			}
			//if(ch != 0 && ch >= CTRL_CODE_CMP){
			keyboard(ch, timeCount);
		}
		
g_check.rap(GetColor(0, 16, 0));
		if(!idle(timeCountTmp)){
			break;
		}
	}
L1:
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

class CMusicInfo{
public:
	CMusicInfo();
	CMusicInfo(const CMusicInfo &info);
	~CMusicInfo();
	
	void load(CTyping &typing);
	
	void readRanking();
	
	void draw(int y, int brightness);
	void drawComment(int x, int y);
	void drawRanking(int x, int y, int rankBegin, int rankLen);
	
	bool titleCmp(char *buf);
	//void renewFont();
private:
	void createFont();
	void deleteFont();
public:
	char m_title[256];
	char m_artist[256];
	char m_fumenAuthor[256];
	int m_level;
	char m_fumenFileName[256];
	char m_rankingFileName[256];
	char m_comment[COMMENT_N_LINES][256];
	
	int m_num;
	int m_numAll;
private:
	CRanking m_ranking;
	static int m_count;	/* ����Ă���CMusicInfo�̌� */
	static int m_fontHandleNormal;
	static int m_fontHandleTitle;
	static int m_fontHandleRanking;
};

int CMusicInfo::m_count = 0;
int CMusicInfo::m_fontHandleNormal;
int CMusicInfo::m_fontHandleTitle;
int CMusicInfo::m_fontHandleRanking;

CMusicInfo::CMusicInfo(){
//printfDx("%d++ ", m_count);
	if(m_count++ == 0){
		createFont();
	}
}

CMusicInfo::CMusicInfo(const CMusicInfo &info){
	*this = info;
//printfDx("%d++ ", m_count);
	if(m_count++ == 0){
		createFont();
	}
}

CMusicInfo::~CMusicInfo(){
	if(--m_count == 0){
		deleteFont();
	}
//printfDx("--%d ", m_count);
}

void CMusicInfo::load(CTyping &typing){
	typing.load(m_fumenFileName, m_rankingFileName);
}

/* �����L���O��ǂݍ��� */
void CMusicInfo::readRanking(){
	m_ranking.open(m_rankingFileName);
	m_ranking.read();
	m_ranking.close();
}

#define X_TITLE 30
#define Y_TITLE (30 - 30/2)
#define X_NUM (W_WINDOW - 25)
#define Y_NUM (60-44)
#define Y_ARTIST Y_NUM
#define X_LEVEL (W_WINDOW - 280)
#define Y_LEVEL (60-22)
#define X_F_AUTHOR (W_WINDOW - 30)
#define Y_F_AUTHOR Y_LEVEL

void CMusicInfo::draw(int y, int brightness){	/* �ȏ���y���獂��60�ŕ`�� */
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
	
	/* �^�C�g���i�Ō�ɏ����j */
	DrawStringToHandle(X_TITLE, y + Y_TITLE, m_title, color, m_fontHandleTitle);
}

void CMusicInfo::drawComment(int x, int y){
	for(int i = 0; i < COMMENT_N_LINES; i++){
		DrawStringToHandle(30+x, y+2 +20* i, m_comment[i], GetColor(255, 255, 255), m_fontHandleNormal);
	}
}


void CMusicInfo::drawRanking(int x, int y, int rankBegin, int rankLen){
	m_ranking.draw(x, y, rankBegin, rankLen, m_fontHandleRanking);
}
/*
void CMusicInfo::renewFont(){
	deleteFont();
	createFont();
}
*/

bool CMusicInfo::titleCmp(char *buf){
/* �y �֐����������B�v�C�� �z */
/* �^�C�g���̈ꕔ�ƈ�v���邩��Ԃ� */
	return (strstr(m_title, buf) != NULL) ? true : false;
}

void CMusicInfo::createFont(){
	m_fontHandleNormal = CreateFontToHandle("�l�r �S�V�b�N", 16, 3, DX_FONTTYPE_ANTIALIASING);
	m_fontHandleTitle = CreateFontToHandle("�l�r ����", 30, 3, DX_FONTTYPE_ANTIALIASING);
	m_fontHandleRanking = CreateFontToHandle(NULL, 16, 2);
}

void CMusicInfo::deleteFont(){	/* �t�H���g���폜 */
	DeleteFontToHandle(m_fontHandleNormal);
	DeleteFontToHandle(m_fontHandleTitle);
	DeleteFontToHandle(m_fontHandleRanking);
}

/* ============================================================ */

vector<CMusicInfo> g_infoArray;

vector<CMusicInfo>::iterator prevInfo(vector<CMusicInfo>::iterator itr){
/* 1�߂�B�ŏ�����1�߂�ƍŌ�ɁB */
	if(itr == g_infoArray.begin()){
		itr = g_infoArray.end();
	}
	itr--;
	return itr;
}

vector<CMusicInfo>::iterator nextInfo(vector<CMusicInfo>::iterator itr){
/* 1�i�ށB1�i��ōŌ�𒴂���ƍŏ��ɁB */
	itr++;
	if(itr == g_infoArray.end()){
		itr = g_infoArray.begin();
	}
	return itr;
}

/* ============================================================ */

struct DrawMainInfo{
public:
	int rankingPos;
	int rankingFlag;	/* true:�ڍ׃����L���O�Afalse:1�ʂ݂̂܂��́A�R�����g�\�� */
	int drawPosX;
	double drawPosY;
	int *addHeight;
private:
	int _addHeight[21];
	int addHeightWait;	/* 0����0�ȊO�ɂȂ鎞�ɑ҂B����𐔂��� */
public:
	DrawMainInfo();
	void reset();
	void step();
	void next();
	void prev();
	void left();
	void right();
};

DrawMainInfo::DrawMainInfo(){
	addHeight = _addHeight + 10;
	reset();
}

void DrawMainInfo::reset(){
	rankingPos = 0;
	rankingFlag = false;
	drawPosX = 0;
	drawPosY = 0.0;
	for(int i=-10; i<=10; i++){
		addHeight[i] = 0;
	}
	addHeightWait = 0;
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
				h = H_COMMENT;
			}
			if(addHeight[0] == 0 && addHeightWait < 25){
			/* ��0����J�����Ƃ��Ă��āA����ɕK�v�Ȏ��Ԍo�߂��Ă��Ȃ� */
				addHeightWait++;
				continue;
			}
		}
		addHeight[i] = h + (int)((addHeight[i] - h) * 0.80);
#if 0
		if(addHeight[i]<h){	/* �����L���O�����L����Ƃ� */
			addHeight[i] += 4;
			if(addHeight[i] > h){
				addHeight[i] = h;
			}
		}else{	/* �����L���O�������߂�Ƃ� */
			addHeight[i] -= 4;
			if(addHeight[i] < 0){
				addHeight[i] = 0;
			}
		}
#endif
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
	drawPosY += MUSIC_INFO_HEIGHT + (addHeight[0] + addHeight[1]) / 2.0;
	for(int i=-10; i<10; i++){
		addHeight[i] = addHeight[i+1];
	}
	addHeight[10] = 0;
	
	addHeightWait = 0;
	
	rankingPos = 0;
	rankingFlag = false;
}

void DrawMainInfo::prev(){
	drawPosY -= MUSIC_INFO_HEIGHT + (addHeight[0] + addHeight[-1]) / 2.0;
	for(int i=10; i>-10; i--){
		addHeight[i] = addHeight[i-1];
	}
	addHeight[-10] = 0;
	
	addHeightWait = 0;
	
	rankingPos = 0;
	rankingFlag = false;
}

void DrawMainInfo::left(){
	if(rankingPos < 0){	/* �R�����g�\���̑O�ւ͍s���Ȃ� */
		return;
	}
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
	if(rankingPos == 0 && !rankingFlag){	/* 1�ʂ̂݁�1�`RANKING_DRAW_LEN�ʂ̂Ƃ� */
		rankingFlag = true;
	}else{	/* ���̑��̎� */
		rankingPos += RANKING_DRAW_LEN;
		drawPosX += W_WINDOW;
	}
}

/* ============================================================ */

/* rankingPos�ʂ���̃����L���O�i���̎��R�����g�j��(x,y)����`��B������h�ɐ��� */
void drawMainRanking(vector<CMusicInfo>::iterator itr, int rankingPos,
		int x, int y, int h){
	int yMin = y, yMax =y+h;
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
		(*itr).drawRanking(x, y + 6, rankingPos, RANKING_DRAW_LEN);	/* 6==(60-48)/2 */
	}else{
		/* �R�����g�\�� */
		(*itr).drawComment(x, y + 6);
	}
}

/* ���C����ʁi�ȑI����ʁj��`�� */
void drawMain(vector<CMusicInfo>::iterator infoArrayItr, DrawMainInfo &dInfo,
		CChallenge &challenge, const char *name, int inputHandle, int fontHandleDefault){
	//if(dInfo.rankingPos == -RANKING_DRAW_LEN){
	int dy = 180 + (MUSIC_INFO_HEIGHT - dInfo.addHeight[0])/2
		+ (int)floor(dInfo.drawPosY + 0.5);
	SetDrawArea(10, 0, W_WINDOW-10, 360);	/* �`��͈͂𐧌� */
	{
		DrawBox(10, dy - MUSIC_INFO_HEIGHT, W_WINDOW - 10, dy + dInfo.addHeight[0], GetColor(32, 32, 64), TRUE);
		(*infoArrayItr).draw(dy - MUSIC_INFO_HEIGHT, 255);	/* �^�C�g���ȂǕ\�� */
		DrawLine(40, dy, W_WINDOW - 40, dy, GetColor(64, 64, 64));
		
		drawMainRanking(infoArrayItr, dInfo.rankingPos, dInfo.drawPosX, dy, dInfo.addHeight[0]);
		if(dInfo.drawPosX < 0){	/* �E�̂�`�� */
			drawMainRanking(infoArrayItr, dInfo.rankingPos + RANKING_DRAW_LEN,
				dInfo.drawPosX + W_WINDOW, dy, dInfo.addHeight[0]);
		}else if(dInfo.drawPosX > 0){	/* ���̂�`�� */
			drawMainRanking(infoArrayItr, dInfo.rankingPos - RANKING_DRAW_LEN,
				dInfo.drawPosX - W_WINDOW, dy, dInfo.addHeight[0]);
		}
	}
	SetDrawArea(10, 0, W_WINDOW-10, 360);	/* �`��͈͂𐧌� */
	/* �^�C�g���ȂǕ\���A�������ŋ�؂� */
	{	/* �O�̋� */
		vector<CMusicInfo>::iterator itr = infoArrayItr;
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
		vector<CMusicInfo>::iterator itr = infoArrayItr;
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
#if 0
	}else{
		DrawBox(10, 10, W_WINDOW - 10, 350, GetColor(32, 32, 64), TRUE);
		
		/* �㉺�ɐ������������Ă��� */
		DrawLine(10, 10, W_WINDOW - 10, 10, GetColor(128, 128, 128));
		DrawLine(10, 350, W_WINDOW - 10, 350, GetColor(128, 128, 128));
		
		/* �^�C�g���ȂǕ\�� */
		(*infoArrayItr).draw(20, 255);
		
		/* �������ŋ�؂� */
		DrawLine(40, 90, W_WINDOW - 40, 90, GetColor(64, 64, 64));
		
		if(dInfo.rankingPos >= 0){
			/* �����L���O�\���irankingPos����RANKING_DRAW_LEN�j */
			(*infoArrayItr).drawRanking(90 + 10, dInfo.rankingPos, RANKING_DRAW_LEN);
		}else{
			(*infoArrayItr).drawComment(90 + 10);
		}
	}
#endif
	DrawBox(0, 360, W_WINDOW, H_WINDOW - 25, GetColor(32, 32, 32), TRUE);
	DrawLine(0, 360, W_WINDOW, 360, GetColor(170, 170, 170));
	
	if(inputHandle == -1){	/* �������ȊO */
		DrawStringToHandle(10, 370, "��/��: �ȑI��, ��/��: �����L���O�\��,   F: �Ȍ���, Enter: �Ȍ���",
			GetColor(255, 255, 255), fontHandleDefault);
		
		DrawStringToHandle(10, 390, "Tab: Window ���� FullScreen, R: ���X�g�ē�,",
			GetColor(255, 255, 255), fontHandleDefault);
		
		{
			char *str = "!/?: Replay Save/Load";
			int width = GetDrawStringWidthToHandle(str, strlen(str), fontHandleDefault);
			DrawStringToHandle(W_WINDOW - 10 - width, 390, str,
				GetColor(255, 255, 255), fontHandleDefault);	/* �E����10�̂Ƃ���ɕ`�� */
		}
		
		DrawStringToHandle(10, H_WINDOW - 65, "[H]idden, [S]udden, [C]ircleStealth, [L]yricsStealth,",
			GetColor(255, 255, 255), fontHandleDefault);
		DrawStringToHandle(10, H_WINDOW - 45, "</>: Speed Down/Up, +/-: KeyShift,   Q: sin, W: cos, E: tan,",
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
		DrawBox(W_WINDOW - 20 - width, H_WINDOW - 25, W_WINDOW, H_WINDOW, GetColor(32, 32, 64), TRUE);
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
	case 'q':
	case 'Q':
		challenge.flip(CHALLENGE_SIN);
		break;
	case 'w':
	case 'W':
		challenge.flip(CHALLENGE_COS);
		break;
	case 'e':
	case 'E':
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
	
	vector<CMusicInfo>::iterator infoArrayItr = g_infoArray.begin();
	//int rankingPos = -RANKING_DRAW_LEN;
	/* -RANKING_DRAW_LEN�Ȃ�A1�ʂ̂ݕ\���B�������Ă����ƁARANKING_DRAW_LEN�ʂ��\������Ƃ��Ɠs�����ǂ��B */
	/* -2*RANKING_DRAW_LEN�Ȃ�R�����g�\�� */
	
	CTyping typing;
	
	typing.setName(name);
	
	while(1){
		if(ProcessMessage() == -1){
			retValue = false;
			break;
		}
		
		//ClearDrawScreen();
		drawMain(infoArrayItr, dInfo, challenge, name, inputHandle, fontHandleDefault);
		dInfo.step();
		myScreenFlip();
		
		if(inputHandle == -1){	/* �������������͒��ł͂Ȃ� */
			char ch = GetKeyboardInput();
			switch(ch){
			case CTRL_CODE_ESC:	/* �I�� */
				retValue = false;
				goto L1;
			case 'r':	/* �ēǂݍ��� */
			case 'R':
				retValue = true;
				goto L1;
			case CTRL_CODE_TAB:	/* Window ���� FullScreen */
				isWindowMode = !isWindowMode;	/* Window���ǂ������t�ɂ��� */
				retValue = true;
				goto L1;
			case CTRL_CODE_CR:	/* �Q�[���J�n */
				typing.setChallenge(challenge);	/* ���[�h�O�Ƀ`�������W��ݒ� */
				(*infoArrayItr).load(typing);	/* �Q�[�����u�Ɂv���ʁE�n�C�X�R�A�̃t�@�C�������[�h */
				/* �������typing.nanka(infoArrayItr)�݂����ɂ������񂾂��A���낢��ʓ| */
				typing.mainLoop();	/* �`��Ƃ���CTyping�ɔC���� */
				typing.unload();	/* �I������ */
				
				dInfo.reset();
				/* �߂��Ă����Ƃ��ɁA�ڍ׃����L���O���甲���� */
				/* �i�Ȃ�ƂȂ��A1�ʂɃ����N�C�������̂ɂق��̂Ƃ��낪�\������Ă���߂����j */
				
				(*infoArrayItr).readRanking();	/* �����L���O�͍X�V����Ă��邩������Ȃ� */
				break;
			/* �Ō�̃Q�[���̂��Đ� */
			/*
			case '@':
				(*infoArrayItr).load(typing);
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
				(*infoArrayItr).load(typing);
				typing.replayLoop();
				typing.unload();
				break;
			case 'f':
			case 'F':
				/* �L�����Z������A���p�܂��͐����Ɍ���͂��Ȃ� */
				inputHandle = MakeKeyInput(64, TRUE, FALSE, FALSE);
				if(inputHandle == -1){
					throw __LINE__;
				}
				SetActiveKeyInput(inputHandle);
				break;
			case CTRL_CODE_UP:	/* �O�̋Ȃ� */
				infoArrayItr = prevInfo(infoArrayItr);
				dInfo.prev();
				break;
			case CTRL_CODE_DOWN:	/* ��̋Ȃ� */
				infoArrayItr = nextInfo(infoArrayItr);
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
				{	/* ���� */
					vector<CMusicInfo>::iterator tmpItr = infoArrayItr;
					do{
						infoArrayItr = nextInfo(infoArrayItr);
						if(infoArrayItr->titleCmp(buf)){
							dInfo.reset();
							/* �����������͍̂ŏ����璆�S�ɕ\�� */
							break;
						}
					}while(infoArrayItr != tmpItr);	/* ���̏ꏊ�ɖ߂��Ă���܂� */
				}
				/* �����ŁA break; ���Ȃ� */
			case 2:
				/* ��̏ꍇ�ɉ����A�L�����Z�����ꂽ�ꍇ */
				DeleteKeyInput(inputHandle);
				inputHandle = -1;
				ClearKeyboardInput();	/* �L�[���͂������Ă��� */
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
		const char *strCopyright = "(c)2007 tos";
		int strWidth = GetDrawStringWidthToHandle(strCopyright, strlen(strCopyright), fontHandleCopyright);
		DrawStringToHandle(W_WINDOW - 10 - strWidth, H_WINDOW - 10 - 12, strCopyright,
			GetColor(255, 255, 255), fontHandleCopyright);
	}
	{
		int strWidth = GetDrawStringWidthToHandle(strInfo, strlen(strInfo), fontHandleInfo);
		DrawStringToHandle((W_WINDOW - strWidth) / 2, (H_WINDOW) * 2 / 3 - 12, strInfo,
			GetColor(255, 255, 255), fontHandleInfo);
	}
	DrawStringToHandle(10, 10, "�����L���O�t�@�C���̌`�����V�����Ȃ�܂����B",
		GetColor(255, 255, 0), fontHandleInfo);
	DrawStringToHandle(10, 40, "���`���ƌ݊����͂���܂����A���e�X�g���̂��߁A",
		GetColor(255, 255, 0), fontHandleInfo);
	DrawStringToHandle(10, 70, "�f�[�^�������Ȃ��Ƃ����ۏ؂͂ł��܂���B",
		GetColor(255, 255, 0), fontHandleInfo);
	DrawStringToHandle(10, 100, "���������������B",
		GetColor(255, 255, 0), fontHandleInfo);
		
	DrawStringToHandle(10, 220, "�����L���O�̏d�����肪���ǂ��ꂽ���߁A",
		GetColor(255, 255, 255), fontHandleInfo);
	DrawStringToHandle(10, 250, "config�ɂ��Ă�ǂނ��Ƃ������߂��܂��B",
		GetColor(255, 255, 255), fontHandleInfo);
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
		CMusicInfo info;
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
	
	for(vector<CMusicInfo>::iterator i = g_infoArray.begin(); i != g_infoArray.end(); i++){
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
