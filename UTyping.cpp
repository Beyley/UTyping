#include "DxLib.h"

#include <stdio.h>
#include <string.h>

#include <vector>
#include <deque>
#include <bitset>

#include "utuid.h"

/*
#define chkI LONGLONG txxx,tyyy;
#define chkB txxx=GetNowHiPerformanceCount();
#define chkE tyyy=GetNowHiPerformanceCount(); {int t=(int)(tyyy-txxx); if(t>=10)printfDx((t>=16384)?"--------%5d--------\n":"%5d\n", t);}
*/

using namespace std;

#define COLOR_EXCELLENT GetColor(255, 255, 0)
#define COLOR_GOOD GetColor(0, 255, 0)
#define COLOR_FAIR GetColor(0, 128, 255)
#define COLOR_POOR GetColor(128, 128, 128)

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

#define TYPE_BUFFER_LEN 8
/* �^�C�s���O�ɗp����o�b�t�@�̒��� */
/* ���O�̍ő咷�ɂ��e������ */

#define RANKING_LEN 20
/* �����L���O�ɋL�^���鏇�ʂ����ʂ܂ł� */

#define RANKING_DRAW_LEN 5
/* ���ʂ������L���O��\�����邩 */
/* RANKING_LEN �̖񐔂��ƕ\���ɖ��ʂ��Ȃ� */

/* ============================================================ */

/* �`��ʒu�Ȃ� */

#define CIRCLE_SPEED 250.0
/* �����~�̕W���̑����i�h�b�g/�b�j */

#define Y_INFO 10
#define Y_INFO2 40

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

#define Y_LYRICS_BIG 350

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

enum{
CHALLENGE_HIDDEN,	/* �B���� */
CHALLENGE_SUDDEN,	/* �ˑR������ */
CHALLENGE_STEALTH,	/* �����Ȃ��� */
CHALLENGE_LYRICS_STEALTH,	/* �̎��������Ȃ��� */
CHALLENGE_SIN,
CHALLENGE_COS,
CHALLENGE_TAN,

CHALLENGE_NUM
};

/* ============================================================ */

/* �G���Ȋ֐��i�\��j */

bool readInt(int &n, FILE *fp);
void writeInt(int n, FILE *fp);
bool readChar8(char *str, FILE *fp);
void writeChar8(char *str, FILE *fp);

void getDirFromPath(char *directoryName, const char *fileName);

//char *fgetline(char buf[], FILE *fp);

void getOrdinal(char *buf, int n);
/* �������擾 */

void getDateStr(char *buf);
/* ���t�Ǝ�����buf�Ɋi�[ */

void outputError(char *str);
/* �G���[�o�� */

HANDLE newThread(LPTHREAD_START_ROUTINE lpStartAddress,LPVOID lpParameter);
void deleteThread(HANDLE handle);

bool readInt(int &n, FILE *fp){
	int b[4];
	for(int i=0; i<4; i++){
		b[i] = getc(fp);
		if(b[i] == EOF){
			return false;
		}
	}
	n = b[0] | b[1]<<8 | b[2]<<16 | b[3]<<24;
	return true;
}

void writeInt(int n, FILE *fp){
	int b[4];
	for(int i=0; i<4; i++){
		b[i] = n >> (8*i);
	}
	for(int i=0; i<4; i++){
		putc(b[i], fp);
	}
	return;
}

bool readChar8(char *str, FILE *fp){
	int b[8];
	for(int i=0; i<8; i++){
		b[i] = getc(fp);
		if(b[i] == EOF){
			return false;
		}
	}
	for(int i=0; i<8; i++){
		str[i] = b[i];
	}
	str[8] = '\0';
	return true;
}

void writeChar8(char *str, FILE *fp){
	int b[8];
	for(int i=0; i<8; i++){
		b[i] = str[i];
	}
	for(int i=1; i<8; i++){
		if(b[i-1] == '\0'){
			b[i] = '\0';
		}
	}
	for(int i=0; i<8; i++){
		putc(b[i], fp);
	}
	return;
}

void getDirFromPath(char *directoryName, const char *fileName){
	strcpy(directoryName, fileName);
	for(int i = strlen(directoryName) - 1; i >= 0; i--){	/* ��납�炽�ǂ� */
		if(directoryName[i] == '/' || directoryName[i] == '\\'){	/* '/'��'\'�����ꂽ�� */
			directoryName[i + 1] = '\0';	/* �����܂łȂ̂ŁA����'\0'�� */
			return;
		}
	}
	/* �Ō�܂Ō���Ȃ������� */
	directoryName[0] = '\0';
	return;
}

#define fgetline(buf, fp) fgetline_1(buf, sizeof(buf), fp)
/* fp����1�s�ǂށB�������A�Ō��'\n'�͍폜 */

char *fgetline_1(char *buf, size_t size, FILE *fp){
	if(fgets(buf, size, fp) == NULL){
		return NULL;
	}
	int len = strlen(buf) - 1;
	if(buf[len] == '\n'){
		buf[len] = '\0';
	}
	return buf; 
}

void getOrdinal(char *buf, int n){
	switch(n % 10){	/* 1�̈ʂŔ��� */
	case 1:
		sprintf(buf, "%2dst", n);
		break;
	case 2:
		sprintf(buf, "%2dnd", n);
		break;
	case 3:
		sprintf(buf, "%2drd", n);
		break;
	default:
		sprintf(buf, "%2dth", n);
		break;
	}
	if((n / 10) % 10 == 1){	/* ����11,12,13��th�ɂ��� */
		sprintf(buf, "%2dth", n);
	}
}

void getDateStr(char *buf){
	DATEDATA date;
	GetDateTime(&date);
	sprintf(buf, "%d/%02d/%02d %02d:%02d:%02d", date.Year, date.Mon,
		date.Day, date.Hour, date.Min, date.Sec);
}

void outputError(char *str){
	char buf[256];
	getDateStr(buf);
	FILE *fp;
	fp = fopen("�G���[���O.txt","a");
	fprintf(fp, "[%s] %s\n", buf, str);
	fclose(fp);
}

//#ifdef UTYPING_USE_MULTI_THREAD

/* DWORD WINAPI (*func)(LPVOID)��V�����X���b�h�Ƃ��č�� */
HANDLE newThread(LPTHREAD_START_ROUTINE lpStartAddress,LPVOID lpParameter){
	DWORD dwID;
	return CreateThread(NULL,0,lpStartAddress,lpParameter,0,&dwID);
}

void deleteThread(HANDLE handle){
	if(!TerminateThread(handle, 0)){
		throw __LINE__;
	}
	CloseHandle(handle);
}

//#endif

/* ============================================================ */

/* Config */

class UTypingConfig{
public:
	UTypingConfig();
	void init();
	void read();
public:
	bool f_useMultiThread;
	int loadSoundType;
	
	bool f_debugMode;
};

UTypingConfig g_config;

UTypingConfig::UTypingConfig(){
	init();
}

void UTypingConfig::init(){
	f_useMultiThread = true;
	loadSoundType = DX_SOUNDDATATYPE_MEMPRESS;
	f_debugMode = false;
}

void UTypingConfig::read(){
	FILE *fp;
	fp = fopen("UTyping_config.txt", "r");
	if(!fp){
		/* config���Ȃ���΁A���ɉ������Ȃ��ł悢�B */
		/* config���Ȃ����Ƃ��G���[�ɂ���͔̂���������B */
		return;
	}
	char buf[256];
	while(fgetline(buf, fp)){
		char *ptr1 = strtok(buf, "=");
		if(ptr1 == NULL){
			continue;
		}
		char *ptr2 = strtok(NULL, "");
		if(ptr2 == NULL){
			ptr2 = "";	/* ""���w�����Ă��� */
		}
		if(!strcmp(ptr1, "UseMultiThread")){
			if(!strcmp(ptr2, "true")){
				f_useMultiThread = true;
			}else if(!strcmp(ptr2, "false")){
				f_useMultiThread = false;
			}else{
				throw "[config] UseMultiThread �� true �܂��� false �Ŏw�肵�Ȃ���΂Ȃ�Ȃ��B�i�f�t�H���g: true �j";
			}
		}else if(!strcmp(ptr1, "LoadSoundType")){
			if(!strcmp(ptr2, "0")){	/* �����ǂݍ��ݎ��Ԃ������A�Đ����ׂ͌y�� */
				loadSoundType = DX_SOUNDDATATYPE_MEMNOPRESS;	/* �ǂݍ��ނƂ��Ɉ��k��W�J */
			}else if(!strcmp(ptr2, "1")){	/* ���X�̓ǂݍ��ݎ��ԂƍĐ����� */
				loadSoundType = DX_SOUNDDATATYPE_MEMPRESS;	/* �Đ�����Ƃ��Ɉ��k��W�J */
			}else if(!strcmp(ptr2, "2")){	/* �ǂݍ��ݎ��Ԃ͒Z���A�Đ����ׂ͏d�� */
				loadSoundType = DX_SOUNDDATATYPE_FILE;	/* �������Ƀ��[�h���Ȃ� */
			}else{
				throw "[config] LoadSoundType �� 0, 1 �܂��� 2 �Ŏw�肵�Ȃ���΂Ȃ�Ȃ��B�i�f�t�H���g: 1 �j";
			}
		}else if(!strcmp(ptr1, "DebugMode")){
			if(!strcmp(ptr2, "true")){
				f_debugMode = true;
			}else if(!strcmp(ptr2, "false")){
				f_debugMode = false;
			}else{
				throw "[config] DebugMode �� true �܂��� false �Ŏw�肵�Ȃ���΂Ȃ�Ȃ��B�i�f�t�H���g: false �j";
			}
		}else{
			throw "[config] �ݒ�ł���l�� UseMultiThread, LoadSoundType, DebugMode �ł���B";
		}
	}
	fclose(fp);
}

/* ============================================================ */

/* UTypingUserID�𒲂ׂ� */

bool checkUTypingUserID(){
	FILE *fp;
	fp = fopen("UTyping_ID.txt", "r");
	if(!fp){
		throw "UTyping_ID.txt ���J���܂���B";
	}
	int ID[3];
	getUTypingUserID(ID);
	for(int i=0; i<3; i++){
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

#define CCHECK_N_DATA (H_WINDOW/8)

class CCheck{
public:
	CCheck();
	void begin();
	void end();
	void draw(int t, int Color);	/* t �}�C�N���b�𒷂� 1 �ŕ`�� */
private:
	int m_counter;
	int m_data[CCHECK_N_DATA];
	LONGLONG m_timeCount;
};

CCheck::CCheck(){
	m_counter = 0;
	for(int i=0; i<CCHECK_N_DATA; i++){
		m_data[i] = 0;
	}
}

void CCheck::begin(){
	m_timeCount=GetNowHiPerformanceCount();
}

void CCheck::end(){
	m_data[m_counter] = GetNowHiPerformanceCount() - m_timeCount;
	m_counter++;
	if(m_counter == CCHECK_N_DATA){
		m_counter = 0;
	}
}

void CCheck::draw(int t, int Color){
	for(int i=0; i<CCHECK_N_DATA; i++){
		DrawBox(0, i * 8, (m_data[(i + m_counter) % CCHECK_N_DATA] / t),
			(i + 1) * 8, Color, TRUE);
	}
}

/* ============================================================ */

//#ifdef UTYPING_USE_MULTI_THREAD
/* �}���`�X���b�h�p */

struct KeyboardInput{
	char ch;
	LONGLONG timeCount;	/* �L�[�������ꂽ���� */
};

deque<KeyboardInput> g_deqKeyboardInput;
CRITICAL_SECTION g_csKeyboardInput;

/* �L�[��������������K�v�Ƃ��Ȃ��i�Ԃ��Ȃ��j�o�[�W���� */
char MT_GetKeyboardInput(){
	char ch;
	EnterCriticalSection(&g_csKeyboardInput);
	{
		if(g_deqKeyboardInput.empty()){	/* �L���[���� */
			ch = 0;
		}else{	/* �L���[����łȂ��̂ł�����擾 */
			KeyboardInput &ki = g_deqKeyboardInput.front();
			ch = ki.ch;
			//timeCount = ki.timeCount;
			g_deqKeyboardInput.pop_front();	/* �ǂ񂾂�폜 */
		}
	}
	LeaveCriticalSection(&g_csKeyboardInput);
	return ch;
}

/* �L�[��������������Ԃ��o�[�W���� */
char MT_GetKeyboardInput(LONGLONG &timeCount){
	char ch;
	EnterCriticalSection(&g_csKeyboardInput);
	{
		if(g_deqKeyboardInput.empty()){	/* �L���[���� */
			ch = 0;
		}else{	/* �L���[����łȂ��̂ł�����擾 */
			KeyboardInput &ki = g_deqKeyboardInput.front();
			ch = ki.ch;
			timeCount = ki.timeCount;
			g_deqKeyboardInput.pop_front();	/* �ǂ񂾂�폜 */
		}
	}
	LeaveCriticalSection(&g_csKeyboardInput);
	return ch;
}

DWORD WINAPI KeyboardInputThread(LPVOID lpParameter){
	ClearInputCharBuf();	/* �ŏ��Ƀo�b�t�@�������Ă��� */
	while(1){
		KeyboardInput ki;
		ki.ch = GetInputChar(TRUE);	/* �L�[���́iTRUE:��x�ǂݍ��񂾂�����j */
		//ki.ch = GetInputCharWait(TRUE);	/* �L�[���́iTRUE:��x�ǂݍ��񂾂�����j */
		if(ki.ch == 0){	/* �u�Ȃ��v�ƋA���Ă�����I��� */
			Sleep(0);
			//WaitKey();
			continue;
		}
		ki.timeCount = GetNowHiPerformanceCount();
		EnterCriticalSection(&g_csKeyboardInput);
		{
			g_deqKeyboardInput.push_back(ki);	/* �L���[�ɒǉ� */
		}
		LeaveCriticalSection(&g_csKeyboardInput);
	}
}

//#else
/* �V���O���X���b�h�p */

char ST_GetKeyboardInput(){
	return GetInputChar(TRUE);
}

char ST_GetKeyboardInput(LONGLONG &timeCount){
	timeCount = GetNowHiPerformanceCount();
	return GetInputChar(TRUE);
}

//#endif

/* config���Q�Ƃ��ĕ��� */

char GetKeyboardInput(){
	if(g_config.f_useMultiThread){
		return MT_GetKeyboardInput();
	}else{
		return ST_GetKeyboardInput();
	}
}

char GetKeyboardInput(LONGLONG &timeCount){
	if(g_config.f_useMultiThread){
		return MT_GetKeyboardInput(timeCount);
	}else{
		return ST_GetKeyboardInput(timeCount);
	}
}


/* ============================================================ */

class CScore{
public:
	CScore();
	CScore(const char *n,int s,int sa,int st,int ce,int cg,int cf,int cp,int cx,int ca,int cm);
	void read(FILE *fp);
	void write(FILE *fp);
	
	void draw(int y, char *str, int fontHandle);
	
	bool nameCmp(CScore &score);
	
	bool operator ==(CScore &score){
		return m_score == score.m_score;
	}
	bool operator !=(CScore &score){
		return m_score != score.m_score;
	}
	bool operator <(CScore &score){
		return m_score < score.m_score;
	}
	bool operator <=(CScore &score){
		return m_score <= score.m_score;
	}
	bool operator >(CScore &score){
		return m_score > score.m_score;
	}
	bool operator >=(CScore &score){
		return m_score >= score.m_score;
	}
private:
	char m_name[TYPE_BUFFER_LEN + 1];
	int m_score;
	int m_scoreAccuracy, m_scoreTyping;
	int m_countExcellent, m_countGood, m_countFair, m_countPoor, m_countPass, m_countAll;
	int m_comboMax;
};

CScore::CScore(){
	strcpy(m_name, "_");
	m_score = 0;
	m_scoreAccuracy = 0;
	m_scoreTyping = 0;
	m_countExcellent = 0;
	m_countGood = 0;
	m_countFair = 0;
	m_countPoor = 0;
	m_countPass = 0;
	m_countAll = 0;
	m_comboMax = 0;
}

CScore::CScore(const char *n,int s,int sa,int st,int ce,int cg,int cf,int cp,int cx,int ca,int cm){
	strcpy(m_name, n);
	m_score = s;
	m_scoreAccuracy = sa;
	m_scoreTyping = st;
	m_countExcellent = ce;
	m_countGood = cg;
	m_countFair = cf;
	m_countPoor = cp;
	m_countPass = cx;
	m_countAll = ca;
	m_comboMax = cm;
}

void CScore::read(FILE *fp){
	strcpy(m_name, "_");
	m_score = 0;
	m_scoreAccuracy = 0;
	m_scoreTyping = 0;
	m_countExcellent = 0;
	m_countGood = 0;
	m_countFair = 0;
	m_countPoor = 0;
	m_countPass = 0;
	m_countAll = 0;
	m_comboMax = 0;
	readChar8(m_name, fp);
	readInt(m_score, fp);
	readInt(m_scoreAccuracy, fp);
	readInt(m_scoreTyping, fp);
	readInt(m_countExcellent, fp);
	readInt(m_countGood, fp);
	readInt(m_countFair, fp);
	readInt(m_countPoor, fp);
	readInt(m_countPass, fp);
	readInt(m_countAll, fp);
	readInt(m_comboMax, fp);
/*
	fscanf(fp, "%s%d%d%d%d%d%d%d%d%d%d", m_name, &m_score, &m_scoreAccuracy, &m_scoreTyping,
		&m_countExcellent, &m_countGood, &m_countFair, &m_countPoor, &m_countPass,
		&m_countAll, &m_comboMax);
*/
}

void CScore::write(FILE *fp){
/*
	fprintf(fp, "%s %d %d %d %d %d %d %d %d %d %d\n", m_name, m_score, m_scoreAccuracy, m_scoreTyping,
		m_countExcellent, m_countGood, m_countFair, m_countPoor, m_countPass,
		m_countAll, m_comboMax);
*/
	writeChar8(m_name, fp);
	writeInt(m_score, fp);
	writeInt(m_scoreAccuracy, fp);
	writeInt(m_scoreTyping, fp);
	writeInt(m_countExcellent, fp);
	writeInt(m_countGood, fp);
	writeInt(m_countFair, fp);
	writeInt(m_countPoor, fp);
	writeInt(m_countPass, fp);
	writeInt(m_countAll, fp);
	writeInt(m_comboMax, fp);
}

void CScore::draw(int y, char *str, int fontHandle){
	char buf[256];
	int width;
	sprintf(buf, "%s: %-8s %10d �_(%10d +%10d ), ", str,
		m_name, m_score, m_scoreAccuracy, m_scoreTyping);
	DrawStringToHandle(40, y + 6, buf, GetColor(255, 255, 255), fontHandle);
	if(m_comboMax >= 0){
		sprintf(buf, "�ő� %4d �R���{, (%4d/%4d/%4d/%4d/%4d)", m_comboMax,
			m_countExcellent, m_countGood, m_countFair, m_countPoor, m_countPass);
	}else{	/* m_comboMax == -1 �Ńt���R���{���� */
		sprintf(buf, "�t���R���{, (%4d/%4d/%4d/%4d/%4d)",
			m_countExcellent, m_countGood, m_countFair, m_countPoor, m_countPass);
	}
	width = GetDrawStringWidthToHandle(buf, strlen(buf), fontHandle);
	DrawStringToHandle((W_WINDOW - 40) - width, y + 26, buf, GetColor(255, 255, 255), fontHandle);
}

bool CScore::nameCmp(CScore &score){
	return strcmp(m_name, score.m_name) == 0;
}


/* ============================================================ */

class CRanking{
public:
	CRanking();
	~CRanking();
	int update(CScore &score);
	void open(const char *fileName);
	void close();
	void read();
	void write();
	
	void draw(int y, int rankBegin, int rankLen, int fontHandle);
private:
	FILE *m_fp;
	CScore m_score[RANKING_LEN];
};

CRanking::CRanking(){
	m_fp = NULL;	/* �܂��t�@�C�����J���Ă��Ȃ� */
}

CRanking::~CRanking(){
	close();
}

/* �����N�C���Ȃ珇��(0�`RANKING_LEN-1)��Ԃ��B�����łȂ���� -1 ��Ԃ� */
int CRanking::update(CScore &score){
	int lastRank = RANKING_LEN - 1;	/* ���łɓ����Ă��鏇�ʁA�����Ă��Ȃ��ꍇ�́ARANKING_LEN - 1���s�����ǂ� */
	/* �f�[�^�����炷�Ƃ��ɁA�����N�O�������Ƃ��Ă��A�Ō�̃����N�������Ƃ��Ă������ɂȂ邩��B */
	for(int i = 0; i < RANKING_LEN; i++){
		if(score.nameCmp(m_score[i])){
			lastRank = i;
			break;
		}
	}
	for(int i = 0; i < RANKING_LEN; i++){
		if(score > m_score[i]){	/* i�ʂ̃X�R�A���^�ɑ傫���̂ŁAi�ʂɃ����N�C�� */
			if(i > lastRank){	/* ���łɂ��鎩���̃X�R�A��舫���i�u�����v�Ƃ������̔���ɂȂ�j */
				return -1;
			}
			
			/* i�ʈȍ~�A�O�̎����̃f�[�^�ȑO�̃f�[�^������1�����炷 */
			for(int j = lastRank; j > i; j--){
				m_score[j] = m_score[j-1];
			}
			m_score[i] = score;
			return i;
		}
	}
	return -1;
}

void CRanking::open(const char *fileName){
	if(m_fp != NULL){
		close();
	}
	/* �o�C�i���ŊJ���̂�"b"������ */
	m_fp = fopen(fileName, "r+b");	/* ����΂�����J���āA */
	if(m_fp == NULL){
		m_fp = fopen(fileName, "w+b");	/* �Ȃ���΍쐬 */
		if(m_fp == NULL){
			throw __LINE__;
		}
	}
}

void CRanking::close(){
	if(m_fp != NULL){
		fclose(m_fp);
		m_fp = NULL;
	}
}

void CRanking::read(){
	rewind(m_fp);
	for(int i=0; i<RANKING_LEN; i++){
		m_score[i].read(m_fp);
	}
	rewind(m_fp);
	return;
}

void CRanking::write(){
	rewind(m_fp);
	for(int i=0; i<RANKING_LEN; i++){
		m_score[i].write(m_fp);
	}
	rewind(m_fp);
}

/* rankBegin����rankLen�ʕ��\�� */
void CRanking::draw(int y, int rankBegin, int rankLen, int fontHandle){
	char buf[256];
	for(int i = 0; i < rankLen; i++){
		int j = rankBegin + i;	/* �������� */
		if(j >= RANKING_LEN){	/* �L�^����Ă��郉���L���O�̊O */
			break;
		}
		getOrdinal(buf, j + 1);	/* ���ۂ́A0�Ԗڂ�1�� etc. */
		m_score[j].draw(y + 48 * i, buf, fontHandle);
	}
}

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
	m_fontHandle = CreateFontToHandle("�l�r ����", CEFFECT1_FONT_SIZE, 2);
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
	bool isJapanese1(); 
public:
	char ch;	/* �ϊ����ꂽ�����i�������A�B�`abc123+-�Ȃǁj��1�o�C�g */
	double timeJust,timeUntil;	/* ���傤�ǂ̎��ԁA���̕����ȑO������łƂ��ɁA�ł��x������ */
	bool isBlockStart;	/* 1���߁i�^�C�~���O��������邩���܂�j�̍ŏ� */
	bool isTyped;	/* ���łɑł��ꂽ�� */
	bool isScoringTarget;	/* ���݃^�C�~���O���������Ώۂł��邩 */
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
	char str[256];	/* �̎� */
	double timeBegin;	/* �\�����n�܂鎞�� */
	double timeEnd;	/* �\�����I��鎞�� */
};

/* ============================================================ */

struct BeatLine{
	int type;
	double time;
};

/* ============================================================ */

class CChallenge{
public:
	CChallenge();
	
	bool isEasy();
	
	void reset();
	void set(int pos);
	void reset(int pos);
	void flip(int pos);
	bool test(int pos);
	
	void speedUp();
	void speedDown();
	double speed();	/* ����鑬���̕W���Ƃ̔��Ԃ� */
	
	void keyUp();
	void keyDown();
	int key();
	double frequencyRate();	/* ���g�������{����邩 */
private:
	bitset<CHALLENGE_NUM> m_flags;
	int m_speed;	/* �~������鑬���̕W���Ƃ̔��10�{(int�ł�����) */
	int m_key;
};

CChallenge::CChallenge(){
	reset();
}

bool CChallenge::isEasy(){
	if(m_key < 0){	/* �Đ����x��x������ */
		return true;
	}
	return false;
}

void CChallenge::reset(){
	m_flags.reset();
	m_speed = 10;
	m_key = 0;
}

void CChallenge::set(int pos){
	m_flags.set(pos);
}

void CChallenge::reset(int pos){
	m_flags.reset(pos);
}

void CChallenge::flip(int pos){
	m_flags.flip(pos);
}

bool CChallenge::test(int pos){
	return m_flags.test(pos);
}

void CChallenge::speedUp(){
	if(m_speed < 20){
		m_speed++;
	}else if(m_speed < 30){
		m_speed += 2;
	}else if(m_speed < 40){
		m_speed += 5;
	}
}

void CChallenge::speedDown(){
	if(m_speed > 30){
		m_speed -= 5;
	}else if(m_speed > 20){
		m_speed -= 2;
	}else if(m_speed > 5){
		m_speed--;
	}
}

double CChallenge::speed(){
	return m_speed / 10.0;
}

void CChallenge::keyUp(){
	if(m_key < 12){
		m_key++;
	}
}

void CChallenge::keyDown(){
	if(m_key > -12){
		m_key--;
	}
}

int CChallenge::key(){
	return m_key;
}

double CChallenge::frequencyRate(){
	return pow(2.0, m_key / 12.0);
}

/* ============================================================ */

class CTyping{
public:
	CTyping();
	~CTyping();
	// void loadDictionary(const char *fileName);
	/* ���[�}�������̓R���X�g���N�^�œǂ� */
	void load(const char *fumenFileName, const char *rankingFileName);
	void unload();
	void setChallenge(CChallenge &challenge);
	void keyboard(char ch, LONGLONG timeCount);
	bool idle();
private:
	void loadRanking(const char *fileName);
	void unloadRanking();
	
	void setText(const char *str, int color);
	
	void setTime();
	void synchronizeTime(int soundHandle, double frequencyRate);
	double getTime();
	double getTime(LONGLONG timeCount);
	
	void phase(int phaseNum);
	
	bool input(char *typeBuffer, int &typeBufferLen, vector<Lyrics>::iterator &lyricsPosition,
		double time, bool isCheckOnly);
	bool input_1(char *typeBuffer, int &typeBufferLen, vector<Lyrics>::iterator &lyricsPosition,
		double time, bool isCheckOnly, bool isSaiki);	/* input�̎��ۂɌv�Z���镔�� */
	
	void scoreTyping(vector<Lyrics>::iterator lyBegin, vector<Lyrics>::iterator lyEnd);
	void scoreAccuracy(double time, vector<Lyrics>::iterator lyricsPosition);
	
	int getDrawPosX(double timeDiff);
	int getDrawPosY(int x);
public:
	void draw();
	void drawResult();
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
	
	LONGLONG m_timeBegin;
	
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
	
	CEffect1 m_effect1;	/* �L�[���̓G�t�F�N�g */
	
	char m_text[256];
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
//#ifndef DEBUG_MODE
	if(!g_config.f_debugMode){	/* �ʏ�̂Ƃ� */
		FILE *fp;
		fp = fopen("convert.dat","r");
		if(fp == NULL){
			throw "convert.dat ���J���܂���B";
		}
		char buf[256], buf1[256], buf2[256], buf3[256];
		while(fgets(buf, sizeof(buf), fp)!=NULL){
			int n=sscanf(buf, "%s%s%s", buf1, buf2, buf3);
			/* ���[�}���A���{��A�c�郍�[�}�� */
			if(n<2){
				throw "convert.dat �̌`�����s���ł��B";
			}
			int len;
			if(n == 2){
				strcpy(buf3, "");
			}
			m_trie.insert(buf2, CConvertData(buf1, buf3));
			/* ���{����L�[�Ƀ��[�}���̃f�[�^������ */
		}
		fclose(fp);
	}else{	/* �f�o�b�O�� */
//#else
		char buf1[256], buf2[256], buf3[256];
		strcpy(buf2, "+");
		strcpy(buf3, "");
		for(char ch = ' ' + 1; ch < 127; ch++){
			buf1[0] = ch;
			buf1[1] = '\0';
			m_trie.insert(buf2, CConvertData(buf1, buf3));
		}
	}
//#endif
	
	/* �`�������W���������iDefault�͉����ςȂ��Ƃ��Ȃ��j */
	/* �`�������W�͋ȃ��[�h�̂Ƃ����������Ȃ��̂� */
	m_challenge.reset();
	
	m_idleCounter = 0;
	
	m_fontHandleNormal = CreateFontToHandle(NULL, 16, 3);
	m_fontHandleBig = CreateFontToHandle("�l�r ����", 36, 2);
	
	m_soundHandleMusic = -1;	/* �����ǂݍ��񂾂肵�Ă͂��Ȃ� */
}

CTyping::~CTyping(){
	unload();
	DeleteFontToHandle(m_fontHandleNormal);	/* �t�H���g���폜 */
	DeleteFontToHandle(m_fontHandleBig);
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
				n = sscanf(tmpBuf + 1, "%lf%s", &time, buf);
				if(n < 2){
					throw "���ʂ��s���ł��B(+)";
				}
				time /= frequencyRate;
//#ifdef DEBUG_MODE
				if(g_config.f_debugMode){
					strcpy(buf, "+");
				}
//#endif
				break;
			case '*':	/* �\������̎� */
				{
					char *ptr = strtok(tmpBuf + 1, " \t\n");	/* �X�y�[�X�ŋ�؂���܂ł̕��������o�� */
					if(ptr == NULL){
						throw "���ʂ��s���ł��B(*)";
					}
					n = sscanf(ptr, "%lf", &time);	/* ������ǂ� */
					if(n < 1){
						throw "���ʂ��s���ł��B(*)";
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
						throw "���ʂ��s���ł��B(=,-)";
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
				continue;
			case '/':	/* ��؂� */
				n = sscanf(tmpBuf + 1, "%lf", &time);
				if(n < 1){
					throw "���ʂ��s���ł��B(/)";
				}
				time /= frequencyRate;
				strcpy(buf, " ");
				
				if(!m_lyricsKanji.empty()){	/* �\������̎������݂��Ă��� */
					LyricsKanji &lk = *(m_lyricsKanji.end() - 1);	/* ���̂Ȃ��ōŌ�̂��̂𑀍� */
					if(lk.timeEnd == INFTY){	/* �I���������ݒ肳��Ă��Ȃ���΁A���݂ɐݒ� */
						lk.timeEnd = time;
					}
				}
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
	
	m_effect1.clear();	/* �L�[���̓G�t�F�N�g�������� */
	
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

void CTyping::setChallenge(CChallenge &challenge){
	m_challenge = challenge;
}

void CTyping::keyboard(char ch, LONGLONG timeCount){
	if(m_phase == PHASE_READY){	/* �J�n�O�Ȃ� */
		if(m_soundHandleMusic != -1){	/* ���y�����łɓǂݍ��܂ꂽ */
			phase(PHASE_MAIN);
			PlaySoundMem(m_soundHandleMusic, DX_PLAYTYPE_BACK);	/* ���y�𗬂��n�߂� */
			setTime();	/* �n�܂����������o���� */
		}
		return;
	}
	if(m_phase == PHASE_RESULT){	/* �X�R�A�\�����Ȃ� */
		/* ���O�L���̂��߂Ƀo�b�t�@��ǂݏ��� */
		if(ch < CTRL_CODE_CMP){	/* �����R�[�h�łȂ��Ƃ� */
			switch(ch){
			case CTRL_CODE_CR:	/* ���s�Ȃ�m�� */
				if(!g_config.f_debugMode){
					if(m_typeBufferLen > 0){	/* ���O�����͂���Ă��邩�m�F */
						phase(PHASE_FINISHED);
					}
				}else{	/* �f�o�b�O���[�h�� */
					phase(PHASE_FINISHED);
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
		if(ch == ' '){	/* �X�y�[�X�̓A���_�[�o�[�ɕϊ��i�����L���O�t�@�C���̎d�l�̂��߁j */
			ch = '_';
		}
		if(m_typeBufferLen < TYPE_BUFFER_LEN){
			m_typeBuffer[m_typeBufferLen++] = ch;
		}
		return;
	}
	if(m_phase == PHASE_FINISHED){	/* �����L���O�\�����Ȃ� */
		if(ch == CTRL_CODE_CR){	/* Enter�ŏI�� */
			phase(PHASE_EXIT);
		}
		return;
	}
	
	if(m_phase != PHASE_MAIN){	/* ���������̓Q�[�����C���̂Ƃ��̏��� */
		return;
	}
	if(ch == '\0' || ch == ' '){	/* ���낢�덢�肻�������A�łӖ����Ȃ����� */
		return;
	}
	double time = getTime(timeCount);	/* �L�[�{�[�h�����������̎��Ԃ��擾 */
	
	if(time < (*m_lyricsPosition).timeJust - SEC_POOR){
		/* poor���O�Ȃ̂őłĂȂ� */
		return;
	}
	
	m_typeBuffer[m_typeBufferLen++] = ch;
	
	if(input(m_typeBuffer, m_typeBufferLen, m_lyricsPosition, time, false)){
	/* ���̓��͂����݂̈ʒu�œ����� */
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
		if((*tmpLyricsPosition).isBlockStart){
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
	}
	m_combo = comboBackUp;	/* m_combo�����ɖ߂� */
	return;
}

bool CTyping::idle(){	/* ���Ȃ���� true ��Ԃ� */
	if(m_phase == PHASE_EXIT){	/* �I�����邱�ƂɂȂ��Ă��� */
		return false;
	}
	if(m_phase == PHASE_READY){	/* �J�n���Ă��Ȃ���Γ��ɂ��邱�Ƃ͖��� */
		if(m_soundHandleMusic == -1){	/* �܂����y��ǂݍ���łȂ�������ǂݍ��� */
			SetCreateSoundDataType(g_config.loadSoundType);	/* config���݂ēǂݍ��ݕ������� */
			m_soundHandleMusic = LoadSoundMem(m_musicFileName);
			
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
	
//static CCheck check;
	if(m_idleCounter++ % 20 == 0){	/* 20���1����s */
//check.begin();
		synchronizeTime(m_soundHandleMusic, m_challenge.frequencyRate());
//check.end();
	}
//check.draw(1);
	
	double time = getTime();
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
	
	return true;
}

void CTyping::loadRanking(const char *fileName){
	m_ranking.open(fileName);
	m_ranking.read();
}

void CTyping::unloadRanking(){
	m_ranking.close();
}

void CTyping::setText(const char *str, int color){	/* �\���������ݒ� */
	m_textTime = getTime();
	strcpy(m_text, str);
	m_textColor = color;
}

void CTyping::setTime(){	/* �J�n������ݒ� */
	m_timeBegin = GetNowHiPerformanceCount();
}

/* ���y�̃X�^�[�g�ƊJ�n�������������Ȃ�悤�ɁA�J�n�����𒲐� */
void CTyping::synchronizeTime(int soundHandle, double frequencyRate){
	if(soundHandle == -1 || CheckSoundMem(soundHandle) != 1){
	/* ���y���ǂݍ��܂�Ă��Ȃ����A�Đ�����Ă��Ȃ����̂Ƃ� */
		return;
	}
	
	int musicTime = (int)(GetSoundCurrentTime(soundHandle) * 1000 / frequencyRate);
	/* �y GetSoundCurrentTime ���g���Ă���̂Œ��ӂ��K�v��������Ȃ��z */
	int realTime = (int)(GetNowHiPerformanceCount() - m_timeBegin);
	int diff = realTime - musicTime;
	/* �����ɁAdiff���傫������Ƃ��ɕω���}����Ƃ������Ƃ�����������Ȃ� */
	/* diff���������Ƃ��ɖ�������Ƃ�����������Ȃ� */
	m_timeBegin += diff;
}

double CTyping::getTime(){	/* �J�n��������̌o�ߕb���擾 */
	return (GetNowHiPerformanceCount() - m_timeBegin) / 1000000.0;
}

double CTyping::getTime(LONGLONG timeCount){	/* timeCount�̊J�n��������̌o�ߕb���擾 */
	return (int)(timeCount - m_timeBegin) / 1000000.0;
}

void CTyping::phase(int phaseNum){
	if(phaseNum == PHASE_RESULT){	/* �I�����āA�X�R�A�\���� */
		setTime();	/* �\���J�n����Ă���̎��Ԃ𗘗p���邽�߂ɃZ�b�g */
		m_typeBufferLen = 0;	/* ���O���͂Ɏg���̂Ńo�b�t�@���N���A */
		
		/* �ʉ߂������𐔂��Ă��� */
		m_countPass = m_countAll - m_countExcellent - m_countGood - m_countFair - m_countPoor;
		
		/* �R���{��������ʒu���ŏI�������i�Ō�̕����������Ƒł��؂����j */
		if(m_combo == m_countAll){
			m_comboMax = -1;	/* �ő�R���{�����u�t���R���{�v�Ƃ��Ă��� */
		}
	}
	if(phaseNum == PHASE_FINISHED){
//#ifndef DEBUG_MODE
		if(!g_config.f_debugMode){
			CScore score(m_typeBuffer, m_score, m_scoreAccuracy, m_scoreTyping,
				m_countExcellent, m_countGood, m_countFair, m_countPoor, m_countPass,
				m_countAll, m_comboMax);
			if(m_challenge.isEasy()){	/* �ȒP�ɂȂ�I�v�V�������g�p�����ꍇ�A�����L���O�ɂ̂��Ȃ� */
				m_rank = -1;
			}else{
				m_rank = m_ranking.update(score);
			}
			if(m_rank >= 0){
				m_ranking.write();
			}else{
				m_phase = PHASE_EXIT;	/* �����N�C�����Ȃ���ΏI�� */
				return;
			}
		}else{
//#else
			m_phase = PHASE_EXIT;
			return;
		}
//#endif
	}
	m_phase = phaseNum;
}

bool CTyping::input(char *typeBuffer, int &typeBufferLen, vector<Lyrics>::iterator &lyricsPosition,
		double time, bool isCheckOnly){
	typeBuffer[typeBufferLen] = '\0';
	/* typeBufferLen���u���ۂɑł��������v�Ƃ��ėp���邽�� */
	
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
	}else if(timeDiff < SEC_GOOD){
		score = SCORE_GOOD + scoreCombo;
		m_countGood++;
		strAccuracy = "��";
		color = COLOR_GOOD;
	}else if(timeDiff < SEC_FAIR){
		score = SCORE_FAIR + scoreCombo;
		m_countFair++;
		strAccuracy = "��";
		color = COLOR_FAIR;
	}else{
		score = SCORE_POOR;
		m_countPoor++;
		m_combo = 0;	/* �R���{���r�؂�Ă��� */
		strAccuracy = "�s��";
		color = COLOR_POOR;
	}
	if(m_combo > m_comboMax){	/* �R���{���̍ő�l���X�V */
		m_comboMax = m_combo;
	}
//#ifndef DEBUG_MODE
	if(!g_config.f_debugMode){
		if(m_combo >= 10){	/* �R���{��10�𒴂�����R���{����\�� */
			sprintf(buf, "%s %d", strAccuracy, m_combo);
		}else{
			sprintf(buf, "%s", strAccuracy);
		}
	}else{	/* �f�o�b�O�� */
//#else
		sprintf(buf, "%+lf", tmp);	/* ���Ԃ̂����\�� */
		if(-SEC_FAIR < tmp && tmp < 0.0){
			//color = GetColor((int)255 * (-tmp/SEC_FAIR), (int)255 * (1 - (-tmp/SEC_FAIR)), 0);
			color = GetColor(255, 0, 0);
		}else if(0.0 <= tmp && tmp < SEC_FAIR){
			//color = GetColor(0, (int)255 * (1 - (tmp/SEC_FAIR)), (int)255 * (tmp/SEC_FAIR));
			color = GetColor(0, 0, 255);
		}
	}
//#endif
	setText(buf, color);
	
	m_scoreAccuracy += score;	/* ���_���Z */
	m_score = m_scoreTyping + m_scoreAccuracy;
	(*lyricsPosition).isScoringTarget = false;	/* 2��ȏ㓾�_��^�����邱�Ƃ͂Ȃ� */
}

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

void CTyping::draw(){
	if(m_phase >= PHASE_RESULT){	/* �X�R�A�\���ȍ~ */
		drawResult();
		return;
	}
	double time;
	if(m_phase == PHASE_READY){
		if(m_soundHandleMusic == -1){
			DrawStringToHandle(50, 70, "Now loading...", GetColor(255, 255, 255), m_fontHandleBig);
		}else{
			DrawStringToHandle(50, 70, "Press any key to start.", GetColor(255, 255, 255), m_fontHandleBig);
		}
		time = 0.0;	/* �n�܂�O��0�b�Ŏ~�܂��Ă��� */
	}else{
		time = getTime();	/* �J�n��������̌o�ߕb���擾 */
	}
	//*
	DrawFormatStringToHandle(10, Y_INFO, GetColor(255, 255, 255), m_fontHandleNormal,
		"%10d �_", m_score);
	DrawFormatStringToHandle(10, Y_INFO2, GetColor(255, 255, 255), m_fontHandleNormal,
		"%10d �R���{", m_combo);
	/*/
	DrawFormatStringToHandle(10, Y_INFO, GetColor(255, 255, 255), m_fontHandleNormal,
		"���_: %10d, %10d �R���{", m_score, m_combo);
	DrawFormatStringToHandle(10, Y_INFO2, GetColor(255, 255, 255), m_fontHandleNormal,
		"( %d + %d ), ( %d / %d / %d / %d), %d",
		m_scoreAccuracy, m_scoreTyping, m_countExcellent, m_countGood, m_countFair, m_countPoor, m_comboMax);
	//*/
#if 0
	{	/* ���݂̉̎��̑傫�Ȃ����܂��\�� */
		vector<Lyrics>::iterator i = m_lyricsPosition;
		while((*i).ch != ' '){	/* �u���b�N�̍ŏ��łȂ���Ζ߂� */
			i--;
		}
		string str;
		i++;
		if((*i).ch != '\n'){
			do{
				str += (*i).ch;
				i++;
			}while((*i).ch != ' ');	/* ���̃u���b�N���n�܂�܂� */
			DrawStringToHandle(10, 60, str.c_str(),
				GetColor(255, 255, 255), m_fontHandleNormal);
		}
	}
#endif
	
	/* ���ߐ��A������\�� */
	while(1){
		double timeDiff = time - (*m_beatLineDrawLeft).time;
		int posX = getDrawPosX(timeDiff);
		if(posX >= 0){
			/* ��ʂ���o�Ă��Ȃ� */
			break;
		}
		/* ���łɉ�ʂ���o�Ă��܂����̂ŁA�`��Ώۂ���폜 */
		m_beatLineDrawLeft++;
	}
	while(1){
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
		
		for(;; m_lyricsDrawLeft++){
			if(!(*m_lyricsDrawLeft).isBlockStart){	/* ���߂̍ŏ��łȂ��Ȃ�i�߂Ă悢 */
				continue;
			}
			double timeDiff = time - (*m_lyricsDrawLeft).timeJust;
			int posX = getDrawPosX(timeDiff);
			if(posX >= -(R_CIRCLE + 50)){	/* �~���͂ݏo��̎����l�����čL�߂Ɏ�� */
				/* ��ʂ���o�Ă��Ȃ� */
				break;
			}
			/* ���łɉ�ʂ���o�Ă��܂����̂ŁA�`��Ώۂ���폜 */
		}
		
		for(;; m_lyricsDrawRight++){
			if(!(*m_lyricsDrawRight).isBlockStart){	/* ���߂̍ŏ��łȂ��Ȃ�i�߂Ă悢 */
				continue;
			}
			double timeDiff = time - (*m_lyricsDrawRight).timeJust;
			int posX = getDrawPosX(timeDiff);
			if(posX >= W_WINDOW + (R_CIRCLE + 50)){	/* �~���͂ݏo��̎����l�����čL�߂Ɏ�� */
				/* ��ʂɓ����Ă��Ă��Ȃ� */
				break;
			}
			/* �V������ʂɓ����Ă����̂ŁA�`��Ώۂɒǉ� */
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
					DrawCircle(posX, Y_CIRCLE + posY, R_CIRCLE, GetColor(128, 128, 128), false);
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
				continue;
			}
			
			int Color;
			if((*i).isScoringTarget){	/* �܂��^�C�~���O�_��������ĂȂ� */
				Color = GetColor(255, 0, 0);
			}else{
				Color = GetColor(0, 0, 255);
			}
			DrawCircle(posX, Y_CIRCLE + posY, R_CIRCLE - 1, Color, TRUE);	/* �����~ */
			DrawCircle(posX, Y_CIRCLE + posY, R_CIRCLE, GetColor(0, 0, 0), FALSE);	/* �����~�̗֊s */
			
			/* �~�̉��̉̎� */
			if(!m_challenge.test(CHALLENGE_LYRICS_STEALTH)){	/* LyricsStealth�Ȃ�\�����Ȃ� */
				int strWidth = GetDrawStringWidthToHandle(buf, len, m_fontHandleNormal);
				DrawStringToHandle(posX - strWidth / 2, Y_LYRICS + posY, buf,
					GetColor(255, 255, 255), m_fontHandleNormal);	/* �����~�ɏ�����镶�� */
			}
		}
		SetDrawArea(0, 0, W_WINDOW, H_WINDOW);	/* �`��͈͂����ɖ߂� */
	}
	
	DrawCircle(X_CIRCLE, Y_CIRCLE, R_CIRCLE, GetColor(255, 255, 255), FALSE);	/* ����ʒu�̉~ */
	
	{	/* ���b�Z�[�W�\�� */
		double timeDiff = time - m_textTime;
		if(timeDiff <= 0.5){	/* ���b�Z�[�W�i�D�ǉs�j��0.5�b�ԕ\������� */
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
			DrawStringToHandle(strX, strY, m_text,
				m_textColor, m_fontHandleBig);
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
			DrawStringToHandle(X_LYRICS_KANJI, Y_LYRICS_KANJI,
				(*m_lyricsKanjiPosition).str,
				GetColor(255, 255, 255), m_fontHandleNormal);	/* �o�� */
			DrawStringToHandle(X_LYRICS_KANJI, Y_LYRICS_KANJI_NEXT,
				(*(m_lyricsKanjiPosition + 1)).str,
				GetColor(255, 255, 255), m_fontHandleNormal);	/* Next���o�� */
		}else{
			DrawStringToHandle(X_LYRICS_KANJI, Y_LYRICS_KANJI_NEXT,
				(*m_lyricsKanjiPosition).str,
				GetColor(255, 255, 255), m_fontHandleNormal);	/* Next���o�� */
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
			DrawStringToHandle(X_CIRCLE - R_CIRCLE, Y_LYRICS_BIG, buf,
				GetColor(255, 255, 255), m_fontHandleBig);
		}
	}
	
	/* �^�C�v����������\�� */
	{
		m_typeBuffer[m_typeBufferLen] = '\0';
		//int strLen = strlen(m_typeBuffer);
		//int strWidth = GetDrawStringWidthToHandle(m_typeBuffer, strLen, m_fontHandleBig);
		DrawStringToHandle(X_CIRCLE - R_CIRCLE, Y_BUFFER, m_typeBuffer,
			GetColor(255, 255, 255), m_fontHandleBig);
	}
	
	m_effect1.draw(time);	/* �L�[���̓G�t�F�N�g */
}

void CTyping::drawResult(){
	double time = getTime();
	DrawFormatStringToHandle(30, 10, GetColor(255, 255, 255), m_fontHandleBig,
		"���� :");
	if(time >= 0.5){
		DrawFormatStringToHandle(320, 10, COLOR_EXCELLENT, m_fontHandleNormal,
			"�@�D : %5d / %d", m_countExcellent, m_countAll);
	}
	if(time >= 0.7){
		DrawFormatStringToHandle(320, 35, COLOR_GOOD, m_fontHandleNormal,
			"�@�� : %5d / %d", m_countGood, m_countAll);
	}
	if(time >= 0.9){
		DrawFormatStringToHandle(320, 60, COLOR_FAIR, m_fontHandleNormal,
			"�@�� : %5d / %d", m_countFair, m_countAll);
	}
	if(time >= 1.1){
		DrawFormatStringToHandle(320, 85, COLOR_POOR, m_fontHandleNormal,
			"�s�� : %5d / %d", m_countPoor, m_countAll);
	}
	if(time >= 1.3){
		DrawFormatStringToHandle(320, 110, GetColor(255, 255, 255), m_fontHandleNormal,
			"�ʉ� : %5d / %d", m_countPass, m_countAll);
	}
	if(time >= 2.3){
		if(m_comboMax >= 0){
			DrawFormatStringToHandle(30, 150, GetColor(255, 255, 255), m_fontHandleBig,
				"�ő� %d �R���{", m_comboMax);
		}else{
			DrawFormatStringToHandle(30, 150, GetColor(255, 255, 0), m_fontHandleBig,
				"�t���R���{");
		}
	}
	if(time >= 3.3){
		DrawFormatStringToHandle(30, 210, GetColor(255, 255, 255), m_fontHandleBig,
			"���_ :");
	}
	if(time >= 3.5){
		DrawFormatStringToHandle(320, 210, GetColor(255, 255, 255), m_fontHandleNormal,
			"�� : %10d �_", m_scoreAccuracy);
	}
	if(time >= 3.7){
		DrawFormatStringToHandle(320, 235, GetColor(255, 255, 255), m_fontHandleNormal,
			"�� : %10d �_", m_scoreTyping);
	}
	if(time >= 4.2){
		DrawFormatStringToHandle(30, 275, GetColor(255, 255, 255), m_fontHandleBig,
			"�����_ : %10d �_",m_score);
	}
	if(!g_config.f_debugMode){	/* �ʏ펞 */
		if(time >= 4.4){
			DrawFormatStringToHandle(30, 375, GetColor(255, 255, 255), m_fontHandleNormal,
				"���O����͂��Ă������� :");
		}
		m_typeBuffer[m_typeBufferLen] = '\0';
		DrawStringToHandle(60, 400, m_typeBuffer, GetColor(255, 255, 255), m_fontHandleBig);
		/* ���͂��ꂽ�������\�� */
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
			DrawBox(320 - boxWidth / 2, 200, 320 + boxWidth/2, 280, GetColor(255, 255, 255), TRUE);
			DrawFormatStringToHandle(320 - strWidth / 2, 240 - 18,
				GetColor(0, 0, 0), m_fontHandleBig,
				"%d �ʂɃ����N�C�����܂���", m_rank + 1);
		}
	}
	return;
}

/* ============================================================ */

void main3(CTyping &typing){
//CCheck check;
//CCheck check2;
//CCheck check3;
	/* �`����(draw)�A�L�[�{�[�h����(keyboard)�A���Ԑ؂ꏈ���ȂǎG�p(idle) */
	/* �E���Ԑ؂ꏈ���̓L�[�{�[�h�������I��点����ɂ��Ȃ���΂Ȃ�Ȃ� */
	/* �E�ŏ��� idle �ŉ��y��ǂݍ��ށi������x�d���j�@�@�̂ŁA���̏��͌Œ� */
//check2.begin();
//check3.begin();
	ClearDrawScreen();
	while(1){
		if(ProcessMessage() == -1){
			return;
		}
//check.begin();
		typing.draw();
//check.end();
//check3.end();
		ScreenFlip();
//check2.end();
//check2.begin();
		ClearDrawScreen();
//check2.draw(16666/100, GetColor(32, 32, 64));
//check3.draw(16666/100, GetColor(64, 64, 32));
//check.draw(16666/100, GetColor(64, 32, 32));
//check3.begin();
		while(1){
			LONGLONG timeCount;
			char ch = GetKeyboardInput(timeCount);
			if(ch == 0){	/* �o�b�t�@����ɂȂ�΃L�[���͏����I�� */
				break;
			}
			if(ch == CTRL_CODE_ESC){	/* Esc�ŃQ�[���𒆒f */
				goto L1;
			}
			//if(ch != 0 && ch >= CTRL_CODE_CMP){
			typing.keyboard(ch, timeCount);
		}
		if(!typing.idle()){
			break;
		}
	}
L1:
	return;
}

/* ============================================================ */

class CMusicInfo{
public:
	CMusicInfo();
	CMusicInfo(const CMusicInfo &info);
	~CMusicInfo();
	
	void load(CTyping &typing);
	
	void readRanking();
	
	void draw(int y, int color);
	void drawRanking(int y, int rankBegin, int rankLen);
	
	//void renewFont();
private:
	void createFont();
	void deleteFont();
public:
	char m_title[256];
	char m_artist[256];
	char m_fumenAuthor[256];
	char m_fumenFileName[256];
	char m_rankingFileName[256];
	
	int m_num;
	int m_numAll;
private:
	CRanking m_ranking;
	static int m_count;	/* ����Ă���CMusicInfo�̌� */
	static int m_fontHandleNormal;
	static int m_fontHandleTitle;
};

int CMusicInfo::m_count = 0;
int CMusicInfo::m_fontHandleNormal;
int CMusicInfo::m_fontHandleTitle;

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

void CMusicInfo::draw(int y, int color){	/* �ȏ���y���獂��60�ŕ`�� */
	/* �^�C�g�� */
	DrawStringToHandle(30, y + (30 - 30/2), m_title, color, m_fontHandleTitle);
	
	int width;
	/* �A�[�e�B�X�g */
	width = GetDrawFormatStringWidthToHandle(m_fontHandleNormal, "%s", m_artist);
	DrawFormatStringToHandle((W_WINDOW - 100) - width, y + (60-44), color, m_fontHandleNormal,
		"%s", m_artist);
	
	/* ���ʍ�� */
	width = GetDrawFormatStringWidthToHandle(m_fontHandleNormal, "(���ʍ쐬�@%s)", m_fumenAuthor);
	DrawFormatStringToHandle((W_WINDOW - 100) - width, y + (60-22), color, m_fontHandleNormal,
		"(���ʍ쐬�@%s)", m_fumenAuthor);
	
	/* �ʂ��ԍ� */
	width = GetDrawFormatStringWidthToHandle(m_fontHandleNormal, "%d/%d", m_num, m_numAll);
	DrawFormatStringToHandle((W_WINDOW - 30) - width, y + (60-44), color, m_fontHandleNormal,
		"%d/%d", m_num, m_numAll);
}

void CMusicInfo::drawRanking(int y, int rankBegin, int rankLen){
	m_ranking.draw(y, rankBegin, rankLen, m_fontHandleNormal);
}
/*
void CMusicInfo::renewFont(){
	deleteFont();
	createFont();
}
*/
void CMusicInfo::createFont(){
	m_fontHandleNormal = CreateFontToHandle(NULL, 16, 3);
	m_fontHandleTitle = CreateFontToHandle("�l�r ����", 30, 3, DX_FONTTYPE_ANTIALIASING);
}

void CMusicInfo::deleteFont(){	/* �t�H���g���폜 */
	DeleteFontToHandle(m_fontHandleNormal);
	DeleteFontToHandle(m_fontHandleTitle);
}

/* ============================================================ */

vector<CMusicInfo> g_infoArray;

vector<CMusicInfo>::iterator prevInfo(vector<CMusicInfo>::iterator itr){
	if(itr == g_infoArray.begin()){
		itr = g_infoArray.end();
	}
	itr--;
	return itr;
}

vector<CMusicInfo>::iterator nextInfo(vector<CMusicInfo>::iterator itr){
	itr++;
	if(itr == g_infoArray.end()){
		itr = g_infoArray.begin();
	}
	return itr;
}

/* ============================================================ */

/* ���C����ʁi�ȑI����ʁj��`�� */
void drawMain(vector<CMusicInfo>::iterator infoArrayItr, int rankingPos,
		CChallenge &challenge, int fontHandleDefault){
	if(rankingPos >= 0){
		DrawBox(10, 10, W_WINDOW - 10, 350, GetColor(32, 32, 64), TRUE);
		
		/* �㉺�ɐ������������Ă��� */
		DrawLine(10, 10, W_WINDOW - 10, 10, GetColor(128, 128, 128));
		DrawLine(10, 350, W_WINDOW - 10, 350, GetColor(128, 128, 128));
		
		/* �^�C�g���ȂǕ\�� */
		(*infoArrayItr).draw(20, GetColor(255, 255, 255));
		
		/* �������ŋ�؂� */
		DrawLine(40, 90, W_WINDOW - 40, 90, GetColor(64, 64, 64));
		
		/* �����L���O�\���irankingPos����RANKING_DRAW_LEN�j */
		(*infoArrayItr).drawRanking(90 + 10, rankingPos, RANKING_DRAW_LEN);
	}else{
		DrawBox(10, 120, W_WINDOW - 10, 240, GetColor(32, 32, 64), TRUE);
		/* �^�C�g���ȂǕ\�� */
		(*prevInfo(prevInfo(infoArrayItr))).draw(0, GetColor(85, 85, 85));
		          (*prevInfo(infoArrayItr)).draw(60, GetColor(128, 128, 128));
		                    (*infoArrayItr).draw(120, GetColor(255, 255, 255));
		          (*nextInfo(infoArrayItr)).draw(240, GetColor(128, 128, 128));
		(*nextInfo(nextInfo(infoArrayItr))).draw(300, GetColor(85, 85, 85));
		
		/* �������ŋ�؂� */
		DrawLine(10,  60, W_WINDOW - 10,  60, GetColor(85, 85, 85));
		DrawLine(10, 120, W_WINDOW - 10, 120, GetColor(128, 128, 128));
		DrawLine(40, 180, W_WINDOW - 40, 180, GetColor(64, 64, 64));
		DrawLine(10, 240, W_WINDOW - 10, 240, GetColor(128, 128, 128));
		DrawLine(10, 300, W_WINDOW - 10, 300, GetColor(85, 85, 85));
		
		/* �����L���O�\���i1�ʂ̂݁j */
		(*infoArrayItr).drawRanking(180 + (60-48)/2, 0, 1);
	}
	DrawBox(0, 360, W_WINDOW, H_WINDOW - 25, GetColor(32, 32, 32), TRUE);
	DrawLine(0, 360, W_WINDOW, 360, GetColor(170, 170, 170));
	DrawLine(0, H_WINDOW - 25, W_WINDOW, H_WINDOW - 25, GetColor(170, 170, 170));
	
	DrawStringToHandle(10, 370, "��/��: �ȑI��, ��/��: �����L���O�\��,   Tab: Window ���� FullScreen,",
		GetColor(255, 255, 255), fontHandleDefault);
	
	{
		char *str = "Enter: �Ȍ���, R: ���X�g�ē�, Esc: �I��";
		int width = GetDrawStringWidthToHandle(str, strlen(str), fontHandleDefault);
		DrawStringToHandle(W_WINDOW - 10 - width, 390, str,
			GetColor(255, 255, 255), fontHandleDefault);	/* �E����10�̂Ƃ���ɕ`�� */
	}
	
	DrawStringToHandle(10, H_WINDOW - 65, "H: Hidden, S: Sudden, C: CircleStealth, L: LyricsStealth,",
		GetColor(255, 255, 255), fontHandleDefault);
	DrawStringToHandle(10, H_WINDOW - 45, "</>: Speed Down/Up, +/-: KeyShift",
		GetColor(255, 255, 255), fontHandleDefault);
	{
		char *str = "D: Reset to Default";
		int width = GetDrawStringWidthToHandle(str, strlen(str), fontHandleDefault);
		DrawStringToHandle(W_WINDOW - 10 - width, H_WINDOW - 65, str,
			GetColor(255, 255, 255), fontHandleDefault);	/* �E����10�̂Ƃ���ɕ`�� */
	}
	{
		char *str = "Q: sin, W: cos, E: tan,";
		int width = GetDrawStringWidthToHandle(str, strlen(str), fontHandleDefault);
		DrawStringToHandle(W_WINDOW - 10 - width, H_WINDOW - 45, str,
			GetColor(255, 255, 255), fontHandleDefault);	/* �E����10�̂Ƃ���ɕ`�� */
	}
	int color;
	if(challenge.test(CHALLENGE_STEALTH)){
		DrawStringToHandle(10, H_WINDOW - 20, "[ CircleStealth ]", GetColor(255, 128, 0), fontHandleDefault);
	}else{
		if(challenge.test(CHALLENGE_HIDDEN)){
			color = GetColor(255, 255, 0);
		}else{
			color = GetColor(64, 64, 64);
		}
		DrawStringToHandle(10, H_WINDOW - 20, "[ Hidden ]", color, fontHandleDefault);
		
		if(challenge.test(CHALLENGE_SUDDEN)){
			color = GetColor(255, 255, 0);
		}else{
			color = GetColor(64, 64, 64);
		}
		DrawStringToHandle(100, H_WINDOW - 20, "[ Sudden ]", color, fontHandleDefault);
	}
	if(challenge.test(CHALLENGE_LYRICS_STEALTH)){
		color = GetColor(255, 255, 0);
	}else{
		color = GetColor(64, 64, 64);
	}
	DrawStringToHandle(190, H_WINDOW - 20, "[ LStealth ]", color, fontHandleDefault);
	
	{
		char buf[256];
		sprintf(buf, "[ Speed x%3.1f ]", challenge.speed());
		if(challenge.speed() >= 2.0){
			color = GetColor(255, 128, 0);
		}else if(challenge.speed() > 1.0){
			color = GetColor(255, 255, 0);
		}else if(challenge.speed() < 1.0){
			color = GetColor(128, 255, 0);
		}else{
			color = GetColor(64, 64, 64);
		}
		DrawStringToHandle(300, H_WINDOW - 20, buf, color, fontHandleDefault);
	}
	
	{
		char buf[256];
		if(challenge.key() >= 6){
			sprintf(buf, "[ Key +%2d ]", challenge.key());
			color = GetColor(255, 128, 0);
		}else if(challenge.key() > 0){
			sprintf(buf, "[ Key +%2d ]", challenge.key());
			color = GetColor(255, 255, 0);
		}else if(challenge.key() < 0){
			sprintf(buf, "[ Key -%2d ]", -challenge.key());
			color = GetColor(128, 255, 0);
		}else{
			sprintf(buf, "[ Key %3d]", challenge.key());
			color = GetColor(64, 64, 64);
		}
		DrawStringToHandle(420, H_WINDOW - 20, buf, color, fontHandleDefault);
	}
	
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

bool main2(bool &isWindowMode){	/* false��Ԃ��΁A�I���Atrue��Ԃ��΁AisWindowMode�ŃE�B���h�E��K�v�Ȃ�ύX���čēǂݍ��� */
	int fontHandleDefault = CreateFontToHandle(NULL, 16, 3);
	
	bool retValue;	/* ��ʃ��[�h��ς��đ�����ꍇtrue */
	
	CChallenge challenge;
	
	vector<CMusicInfo>::iterator infoArrayItr = g_infoArray.begin();
	int rankingPos = -RANKING_DRAW_LEN;
	/* ���Ȃ�A1�ʂ̂ݕ\���B-RANKING_DRAW_LEN�ɂ��Ă����ƁARANKING_DRAW_LEN�ʂ��\������Ƃ��Ɠs�����ǂ��B */
	
	CTyping typing;
	
	while(1){
		if(ProcessMessage() == -1){
			retValue = false;
			break;
		}
		
		ClearDrawScreen();
		drawMain(infoArrayItr, rankingPos, challenge, fontHandleDefault);
		ScreenFlip();
		
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
			main3(typing);
			typing.unload();	/* �I������ */
			
			rankingPos = -RANKING_DRAW_LEN;
			/* �߂��Ă����Ƃ��ɁA�ڍ׃����L���O���甲���� */
			/* �i�Ȃ�ƂȂ��A1�ʂɃ����N�C�������̂ɂق��̂Ƃ��낪�\������Ă���߂����j */
			
			(*infoArrayItr).readRanking();	/* �����L���O�͍X�V����Ă��邩������Ȃ� */
			break;
		case CTRL_CODE_UP:	/* 1�߂�B�ŏ�����1�߂�ƍŌ�ɁB */
			infoArrayItr = prevInfo(infoArrayItr);
			rankingPos = -RANKING_DRAW_LEN;
			break;
		case CTRL_CODE_DOWN:	/* 1�i�ށB1�i��ōŌ�𒴂���ƍŏ��ɁB */
			infoArrayItr = nextInfo(infoArrayItr);
			rankingPos = -RANKING_DRAW_LEN;
			break;
		case CTRL_CODE_LEFT:	/* ��ʂ�\�� */
			if(rankingPos < 0){
				break;
			}
			rankingPos -= RANKING_DRAW_LEN;
			break;
		case CTRL_CODE_RIGHT:	/* ���ʂ�\�� */
			if(rankingPos + RANKING_DRAW_LEN >= RANKING_LEN){
				break;
			}
			rankingPos += RANKING_DRAW_LEN;
			break;
		default:
			editChallenge(challenge, ch);
			break;
		}
	}
	
L1:
	DeleteFontToHandle(fontHandleDefault);
	return retValue;
}

/* flag �̓��X�g�̓ǂݍ��݂������������ǂ��� */
void drawTitle(int fontHandleTitle, int fontHandleInfo, char *strInfo){
	//drawBox(0, 0, W_WINDOW, H_WINDOW, Color(32, 32, 32);
	char *strTitle = "UTyping";
	{
		int strWidth = GetDrawStringWidthToHandle(strTitle, strlen(strTitle), fontHandleTitle);
		DrawStringToHandle((W_WINDOW - strWidth) / 2, H_WINDOW / 3 - 24, strTitle,
			GetColor(255, 255, 255), fontHandleTitle, GetColor(170, 170, 170));
	}
	{
		int strWidth = GetDrawStringWidthToHandle(strInfo, strlen(strInfo), fontHandleInfo);
		DrawStringToHandle((W_WINDOW - strWidth) / 2, (H_WINDOW) * 2 / 3 - 12, strInfo,
			GetColor(255, 255, 255), fontHandleInfo);
	}
}

void main1(){
	bool isWindowMode = true;
	while(1){
		{
			SetDrawScreen(DX_SCREEN_BACK);
			/* �_�u���o�b�t�@��p���� */
			
			int fontHandleTitle = CreateFontToHandle("�l�r ����", 48, 3, DX_FONTTYPE_ANTIALIASING_EDGE);
			int fontHandleInfo = CreateFontToHandle("�l�r ����", 24, 3, DX_FONTTYPE_ANTIALIASING);
			ClearDrawScreen();
			drawTitle(fontHandleTitle, fontHandleInfo, "Now loading...");
			ScreenFlip();
			
			bool isCorrectID = checkUTypingUserID();
			
			int count = 0;
			
			g_infoArray.clear();
			FILE *fp;
			fp = fopen("UTyping_list.txt", "r");
			if(fp == NULL){
				throw "UTyping_list.txt ���J���܂���B";
			}
			char buf[256];
			while(fgetline(buf, fp) != NULL){
				FILE *fpInfo;
				CMusicInfo info;
				fpInfo = fopen(buf, "r");	/* ���t�@�C�����J�� */
				if(fpInfo == NULL){	/* �J���Ȃ��t�@�C���͖��� */
					continue;
				}
				char dirName[256];	/* �f�B���N�g���͏��t�@�C������Ƃ��� */
				getDirFromPath(dirName, buf);
				char buf1[256], buf2[256], *chk;
				fgetline(info.m_title, fpInfo);	/* 1�s�ڂɃ^�C�g�� */
				fgetline(info.m_artist, fpInfo);	/* 2�s�ڂɌ���� */
				fgetline(info.m_fumenAuthor, fpInfo);	/* 3�s�ڂɕ��ʍ�� */
				fgetline(buf1, fpInfo);
				chk = fgetline(buf2, fpInfo);
				fclose(fpInfo);
				if(chk == NULL){	/* �S�s�ǂݍ��߂Ȃ����� */
					continue;
				}
				sprintf(info.m_fumenFileName, "%s%s", dirName, buf1);	/* 4�s�ڂɕ��ʃt�@�C���� */
				sprintf(info.m_rankingFileName, "%s%s", dirName, buf2);	/* 5�s�ڂɃn�C�X�R�A�t�@�C���� */
				count++;
				info.m_num = count;	/* �ʂ��ԍ������� */
				g_infoArray.push_back(info);
			}
			fclose(fp);
			if(g_infoArray.empty()){
				"�Ȃ���Ȃ����݂��܂���B";
			}
			
			for(vector<CMusicInfo>::iterator i = g_infoArray.begin(); i != g_infoArray.end(); i++){
				(*i).readRanking();		/* �����L���O��ǂݍ��� */
				(*i).m_numAll = count;
			}
			
			while(1){
				if(ProcessMessage() == -1){
					return;
				}
				SetDrawScreen(DX_SCREEN_BACK);
				ClearDrawScreen();
				drawTitle(fontHandleTitle, fontHandleInfo, isCorrectID ? "Press Enter key." : "Wrong UTyping user ID...");
				ScreenFlip();
				char ch = GetKeyboardInput();
				if(ch == 0){
					continue;
				}
				if(!isCorrectID){	/* ID���s���ȏꍇ�́A�ǂ̃L�[�������Ă��I���B */
					return;
				}
				if(ch == CTRL_CODE_ESC){	/* Esc�������ƏI�� */
					return;
				}
				if(ch == CTRL_CODE_TAB){	/* Tab�������ƁAWindow ���� FullScreen */
					isWindowMode = !isWindowMode;	/* isWindowMode��ύX���čċN�� */
					break;
				}
				if(ch == CTRL_CODE_CR){	/* ���s�������ꂽ��ȑI����ʂ� */
					if(!main2(isWindowMode)){	/* false���߂����ꍇ�i�I������Ƃ��j */
						return;
					}
					break;	/* true���߂����ꍇ�i�ċN������Ƃ��j */
				}
			}
			DeleteFontToHandle(fontHandleTitle);
			DeleteFontToHandle(fontHandleInfo);
			
		}
		ChangeWindowMode(isWindowMode);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPSTR lpCmdLine, int nCmdShow){
	try{
//SetGraphMode(W_WINDOW, H_WINDOW, 32);
		ChangeWindowMode(TRUE);
		g_config.read();
		if(!g_config.f_debugMode){
			SetMainWindowText("UTyping (c)2007 tos");
		}else{	/* �f�o�b�O���[�h�ł��邱�Ƃ���ɏo�� */
			SetMainWindowText("UTyping (c)2007 tos -- (Debug Mode)");
		}

		if(DxLib_Init() == -1){
			return -1;
		}
		if(g_config.f_useMultiThread){
			InitializeCriticalSection(&g_csKeyboardInput);
			HANDLE hThreadKI;
			hThreadKI = newThread(KeyboardInputThread, NULL);
			main1();
			deleteThread(hThreadKI);
			DeleteCriticalSection(&g_csKeyboardInput);
		}else{
			main1();
		}
	}catch(int n){
		char buf[256];
		sprintf(buf, "%d �s�ڂŃG���[���������܂����B", n);
		outputError(buf);
	}catch(char *str){
		outputError(str);
	}
	DxLib_End();
	return 0;
}
