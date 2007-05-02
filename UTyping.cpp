#include "DxLib.h"

#include <stdio.h>
#include <string.h>
#include <vector>

/*
#define DEBUG_MODE 1
//*/

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
/* �y RANKING_LEN �̖񐔂łȂ���΂Ȃ�Ȃ��z */

/* ============================================================ */

#define INFTY 1000000000

enum{
PHASE_READY,	/* �J�n�O */
PHASE_MAIN,	/* ���C�� */
PHASE_RESULT,	/* �X�R�A�\�� */
PHASE_FINISHED,	/* ���O���͌�i�n�C�X�R�A���������j */
PHASE_EXIT,	/* �I������̂ŏ����Ă��炤 */
};

/* ============================================================ */

/* �G���Ȋ֐��i�\��j */

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

/* ============================================================ */

class CScore{
public:
	CScore();
	CScore(const char *n,int s,int sa,int st,int ce,int cg,int cf,int cp,int cx,int ca,int cm);
	void read(FILE *fp);
	void write(FILE *fp);
	
	void draw(int y, int num);
	
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

void CScore::draw(int y, int num){
	char buf[256];
	int width;
	sprintf(buf, "%2d: %-8s %10d �_(%10d+%10d), ", num,
		m_name, m_score, m_scoreAccuracy, m_scoreTyping);
	DrawString(40, y + 6, buf, GetColor(255, 255, 255));
	sprintf(buf, "(%4d/%4d/%4d/%4d/%4d), �ő� %4d �R���{",
		m_countExcellent, m_countGood, m_countFair, m_countPoor, m_countPass, m_comboMax);
	width = GetDrawStringWidth(buf, strlen(buf));
	DrawString(600 - width, y + 26, buf, GetColor(255, 255, 255));
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
	
	void draw(int y, int rankBegin);
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

/* rankBegin����RANKING_DRAW_LEN�ʕ��\�� */
void CRanking::draw(int y, int rankBegin){
	SetFontSize(16);
	for(int i = 0; i < RANKING_DRAW_LEN; i++){
		m_score[rankBegin + i].draw(y + 48 * i, rankBegin + i + 1);
	}
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
//printf("%s %d\n",m_str,m_len);
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
	//void chk();
private:
	CTrieNode *m_next[256];
public:
	/* ����private�ɂ���̖ʓ| */
	vector<CConvertData> m_data;
};
/*
void CTrieNode::chk(){
	for(vector<CConvertData>::iterator i=m_data.begin(); i != m_data.end(); i++){
		printf("%s(%d) ", (*i).m_str, (*i).m_len);
	}
	printf("|\n");
}
*/
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
//printf("[%s %s %d]",str,data.m_str,data.m_len);
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
	string str;	/* �̎� */
	double timeBegin;	/* �\�����n�܂鎞�� */
	double timeEnd;	/* �\�����I��鎞�� */
};

/* ============================================================ */

class CTyping{
public:
	CTyping();
	~CTyping();
	// void loadDictionary(const char *fileName);
	/* ���[�}�������̓R���X�g���N�^�œǂ� */
	void load(const char *fileName);
	void loadRanking(const char *fileName);
	void keyboard(char ch);
	bool idle();
private:
	void setText(const char *str, int color);
	void setTime();
	double getTime();
	void phase(int phaseNum);
	bool input(char *typeBuffer, int &typeBufferLen,
		vector<Lyrics>::iterator &lyricsPosition, double time, bool saiki);
	void scoreTyping(vector<Lyrics>::iterator lyBegin, vector<Lyrics>::iterator lyEnd);
	void scoreAccuracy(double time, vector<Lyrics>::iterator lyricsPosition);
public:
	void draw();
	void drawResult();
private:
	CTrieNode m_trie;
	
	vector<Lyrics> m_lyrics;
	vector<Lyrics>::iterator m_lyricsPosition;
	
	double m_timeLength;	/* ���ʂ́i���Ԃ́j���� */
	
	char m_typeBuffer[TYPE_BUFFER_LEN + 1];
	int m_typeBufferLen;
	
	vector<LyricsKanji> m_lyricsKanji;
	vector<LyricsKanji>::iterator m_lyricsKanjiPosition;
	
	int m_phase;
	
	int m_timeBegin;
	
	int m_score;
	int m_scoreTyping;	/* �^�C�s���O�ɂ�链�_ */
	int m_scoreAccuracy;	/* ���x�ɂ�链�_ */
	int m_combo;
	
	int m_comboMax;
	
	int m_countExcellent;
	int m_countGood;
	int m_countFair;
	int m_countPoor;
	int m_countPass;
	int m_countAll;
	
	CRanking m_ranking;	/* �n�C�X�R�A�̃f�[�^ */
	int m_rank;	/* �n�C�X�R�A�̒��ł̏��ʁA�����N�C�����Ȃ���� -1 */
	
	char m_text[256];
	int m_textColor;
	double m_textTime;
	
	char m_musicFileName[256];
	
	int m_fontHandleNormal;
	int m_fontHandleBig;
};

CTyping::CTyping(){
	FILE *fp;
	fp = fopen("convert.dat","r");
	if(fp == NULL){
		throw __LINE__;
	}
	char buf[256], buf1[256], buf2[256], buf3[256];
	while(fgets(buf, sizeof(buf), fp)!=NULL){
		int n=sscanf(buf, "%s%s%s", buf1, buf2, buf3);
		/* ���[�}���A���{��A�c�郍�[�}�� */
		if(n<2){
			throw __LINE__;
		}
		int len;
		if(n == 2){
			strcpy(buf3, "");
		}
		m_trie.insert(buf2, CConvertData(buf1, buf3));
		/* ���{����L�[�Ƀ��[�}���̃f�[�^������ */
	}
	fclose(fp);
	
	m_fontHandleNormal = CreateFontToHandle(NULL, 16, 3);
	m_fontHandleBig = CreateFontToHandle("�l�r ����", 36, 2);
	
	//ClearInputCharBuf();	/* �L�[���͂����� */
	
	//timeBeginPeriod(1);	/* timeGetTime�̐��x���グ�� */
}

CTyping::~CTyping(){
	DeleteFontToHandle(m_fontHandleNormal);	/* �t�H���g���폜 */
	DeleteFontToHandle(m_fontHandleBig);
	
	if(CheckMusic()){	/* ���y������Ă���~�߂� */
		StopMusic();
	}
	//timeEndPeriod(1);	/* timeGetTime�̐��x�����ɖ߂� */
}

void CTyping::load(const char *fileName){
	char directoryName[256];
	getDirFromPath(directoryName, fileName);
	FILE *fp;
	fp = fopen(fileName,"r");
	if(fp == NULL){
		throw __LINE__;
	}
	
	/* �ǂݍ��ޑO�ɑS���� */
	m_lyrics.clear();
	m_lyricsKanji.clear();
	strcpy(m_musicFileName, "");
	
	/* �̎��̂����܂�̐��i=m_count�����̘a�̍ő�l�j�𐔂��邽�߂�0�� */
	m_countAll = 0;
	
	/* �ǂݍ��݊J�n */

#if 0
	/* �ԕ��Ƃ��čŏ���' ',0��ǉ����Ă��� */
	{
		Lyrics ly;
		ly.ch = ' ';
		ly.timeJust = 0;
		ly.timeUntil = 0;
		ly.isBlockStart = true;
		ly.isTyped = true;
		ly.isScoringTarget = false;
		m_lyrics.push_back(ly);
	}
#endif
	
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
					throw __LINE__;
				}
				break;
			case '*':	/* �\������̎� */
				{
					char *ptr = strtok(tmpBuf + 1, " \t\n");	/* �X�y�[�X�ŋ�؂���܂ł̕��������o�� */
					if(ptr == NULL){
						throw __LINE__;
					}
					n = sscanf(ptr, "%lf", &time);	/* ������ǂ� */
					if(n < 1){
						throw __LINE__;
					}
					ptr = strtok(NULL, "");	/* �c��̕��������o�� */
					LyricsKanji lk;
					lk.str = string(ptr);
					lk.timeBegin = time;
					lk.timeEnd = INFTY;	/* �I��肪�ݒ肳��Ȃ���΁A�\�����ꑱ���� */
					m_lyricsKanji.push_back(lk);
				}
				continue;
			case '/':	/* ��؂� */
				n = sscanf(tmpBuf + 1, "%lf", &time);
				if(n < 1){
					throw __LINE__;
				}
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
//printf("%s(%3.1f)",buf,time);
		}
		Lyrics ly;
		ly.isBlockStart = true;
		ly.isScoringTarget = true;
		for(char *ptr=bufLast; *ptr!='\0'; ptr++){
			ly.ch = *ptr;
			ly.timeJust = timeLast;
			ly.timeUntil = time;	/* ���̉̎���timeJust�܂� */
//printf("%c ,%lf,%lf\n",ly.ch,ly.timeJust,ly.timeUntil);
			if(ly.ch == ' '){
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
//printf("]\n");
		timeLast = time;
		strcpy(bufLast, buf);
	}
	fclose(fp);
//printf("\n");
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
		lk.str = "";
		lk.timeBegin = INFTY;
		lk.timeEnd = INFTY;
		m_lyricsKanji.push_back(lk);
	}
	
	/* ���ݑł��Ă���̎��̈ʒu */
	m_lyricsPosition = m_lyrics.begin();
	m_lyricsKanjiPosition = m_lyricsKanji.begin();
	
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
}

void CTyping::loadRanking(const char *fileName){
	m_ranking.open(fileName);
	m_ranking.read();
}

void CTyping::keyboard(char ch){
	if(m_phase == PHASE_READY){	/* �J�n�O�Ȃ� */
		phase(PHASE_MAIN);
		PlayMusic(m_musicFileName, DX_PLAYTYPE_BACK);	/* ���y�𗬂��n�߂� */
		setTime();	/* �n�܂����������o���� */
		return;
	}
	if(m_phase == PHASE_RESULT){	/* �X�R�A�\�����Ȃ� */
		/* ���O�L���̂��߂Ƀo�b�t�@��ǂݏ��� */
		if(ch < CTRL_CODE_CMP){	/* �����R�[�h�łȂ��Ƃ� */
			switch(ch){
			case CTRL_CODE_CR:	/* ���s�Ȃ�m�� */
				if(m_typeBufferLen > 0){	/* ���O�����͂���Ă��邩�m�F */
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
	double time = getTime();
	
	if(time < (*m_lyricsPosition).timeJust - SEC_POOR){
		/* poor���O�Ȃ̂őłĂȂ� */
//printf("!\n!\n");
		return;
	}
	
	m_typeBuffer[m_typeBufferLen++] = ch;
	m_typeBuffer[m_typeBufferLen] = '\0';
	
	if(input(m_typeBuffer, m_typeBufferLen, m_lyricsPosition, time, false)){
	/* ���̓��͂����݂̈ʒu�œ����� */
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
	tmpBuffer[1] = '\0';
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
		return true;
	}
	if(m_phase != PHASE_MAIN){	/* ���������̓Q�[�����C���̂Ƃ��̏��� */
		return true;
	}
	
	/* ���y�̍Đ����I�����Ă���Ȃ�X�R�A��ʂ� */
	/*
	if(CheckMusic() <= 0){
		phase(PHASE_RESULT);
		return true;
	}
	*/
	double time = getTime();
	
	if(time >= m_timeLength){	/* �Ō�̕��ʂ��ʉ� */
		phase(PHASE_RESULT);
	}
	
	for(vector<Lyrics>::iterator i = m_lyricsPosition; ; i++){
		if((*i).ch == ' '){	/* �ł��x��Ă���̎������ǂ�����A�傫�ȋ�؂�܂ŗ��Ă��܂��� */
			m_combo = 0;	/* �R���{�͓r�؂�� */
			m_typeBufferLen = 0;
			m_lyricsPosition = i;	/* ���͈ʒu�������܂ňړ����Ă��܂� */
			break;
		}
		if((*i).timeUntil >= time){
			break;
		}
	}
	return true;
}

void CTyping::setText(const char *str, int color){	/* �\���������ݒ� */
	m_textTime = getTime();
	strcpy(m_text, str);
	m_textColor = color;
}

void CTyping::setTime(){	/* �J�n������ݒ� */
	m_timeBegin = GetNowCount();
}

double CTyping::getTime(){	/* �J�n��������̌o�ߕb���擾 */
	return (GetNowCount() - m_timeBegin) / 1000.0;
}

void CTyping::phase(int phaseNum){	/* �I�����āA�X�R�A�\���� */
	if(phaseNum == PHASE_RESULT){
		setTime();	/* �\���J�n����Ă���̎��Ԃ𗘗p���邽�߂ɃZ�b�g */
		m_typeBufferLen = 0;	/* ���O���͂Ɏg���̂Ńo�b�t�@���N���A */
		
		/* �ʉ߂������𐔂��Ă��� */
		m_countPass = m_countAll - m_countExcellent - m_countGood - m_countFair - m_countPoor;
	}
	if(phaseNum == PHASE_FINISHED){
		CScore score(m_typeBuffer, m_score, m_scoreAccuracy, m_scoreTyping,
			m_countExcellent, m_countGood, m_countFair, m_countPoor, m_countPass,
			m_countAll, m_comboMax);
		m_rank = m_ranking.update(score);
		if(m_rank >= 0){
			m_ranking.write();
		}else{
			m_phase = PHASE_EXIT;	/* �����N�C�����Ȃ���ΏI�� */
			return;
		}
	}
	m_phase = phaseNum;
}

bool CTyping::input(char *typeBuffer, int &typeBufferLen,
		vector<Lyrics>::iterator &lyricsPosition, double time, bool saiki){
/* saiki�͊O����̌Ăяo���ł�false,������̌Ăяo���ł�true */
	if(strlen(typeBuffer) == 0){
	/* ���m�胍�[�}�����Ȃ��̂ɍċA���� */
		return true;
	}
	vector<Lyrics>::iterator tmpLyricsPosition = lyricsPosition;
	CTrieNode *trie = &m_trie;
	while(1){
		char buf[2];
		buf[0] = (*tmpLyricsPosition).ch;
		tmpLyricsPosition++;
//printf("%c",buf[0]);
		buf[1] = '\0';
//if(trie!=NULL)//debug
		trie = trie->find(buf);	/* ���{��1�o�C�g�i�� */
		if(trie == NULL){
//printf("//");
			return false;
		}
//trie->chk();
		for(vector<CConvertData>::iterator i = trie->m_data.begin();
				i != trie->m_data.end(); i++){	/* ���[�}������T�� */
//printf("[%s]",(*i).m_str);
			if((*i).match_front(typeBuffer)){
				int tmpLen = typeBufferLen - (*i).m_len;
				/* ����ϊ�����镔���̎��̕�������ɑ΂��Ď��ۂɑł���Ă��镔���̒����B */
				/* ����͕��Ƃ��ɂȂ肤��B */
				if(tmpLen <= 0){	/* ����ɕK�v�Ȃ�L�[�𑫂��Ĉ�x�ɑłĂ�͈͂͂����܂� */
				/* ����́A{��΁c�c,nb�c�c}��n�����ł����󋵂Ȃǂ��܂܂��B */
//printf("[%f,%f -%f]\n",time,(*(tmpLyricsPosition-1)).timeJust,(*(tmpLyricsPosition-1)).timeUntil);
					if(time >= (*(tmpLyricsPosition-1)).timeUntil){
						/* ���̍Ō�̕��������Ԑ؂ꂵ�Ă��Ă͂����Ȃ� */
						continue;
					}
				}
				char tmpTypeBuffer[TYPE_BUFFER_LEN + 1];
				strcpy(tmpTypeBuffer, (*i).m_str + (*i).m_len);	/* ���m�胍�[�}���ɂȂ�\��̕��� */
				if(input(tmpTypeBuffer, tmpLen, tmpLyricsPosition, time, true)){
				/* �ċA�̌��ʑłĂ邱�Ƃ����������Ƃ� */
					if(typeBufferLen >= 1 && (*lyricsPosition).isScoringTarget){
					/* �V�������߂̑ł��n��(=���_�ΏۂɂȂ��Ă���) */
						scoreAccuracy(time, lyricsPosition);
						/* ���傤�ǂ̃^�C�~���O�Ƃ̂��ꂩ��v�Z�������_��^����B */
					}
					
					/* �u���v�̏����A�S�Ă�3�o�C�g�ȏ�ւ̕ϊ��Ɍ��ʂ�����̂Œ��� */
					if(typeBufferLen >= 2 && lyricsPosition + 2 < tmpLyricsPosition &&
							(*(lyricsPosition + 2)).isScoringTarget){
					/* 2�����ł��āA2�o�C�g��(���{���1������)�������x�ɑłĂ�͈͂ŁA����ɍ̓_�Ώۂł��� */
					/* �܂�A�u�����v�ɂ�����"kk"�̎��_�Łu���v���̓_�����B */
						scoreAccuracy(time, lyricsPosition + 2);
						/* ���傤�ǂ̃^�C�~���O�Ƃ̂��ꂩ��v�Z�������_��^����B */
					}
					if((*i).match(typeBuffer)){	/* ���S��v */
						/* �ϊ����ꂽ�̎��̕��������_��^���� */
						if(!saiki){
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
//printf("!!\n");
					return true;
				}else{
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
	double timeDiff = time - (*lyricsPosition).timeJust;
#ifdef DEBUG_MODE
	double tmp = timeDiff;
#endif
	int scoreCombo = SCORE_COMBO * m_combo;	/* �R���{���𑝂₷�O�ɃR���{���{�[�i�X���v�Z */
	if(scoreCombo > SCORE_COMBO_MAX){
		scoreCombo = SCORE_COMBO;
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
#ifndef DEBUG_MODE
	if(m_combo >= 10){	/* �R���{��10�𒴂�����R���{����\�� */
		sprintf(buf, "%s %d", strAccuracy, m_combo);
	}else{
		sprintf(buf, "%s", strAccuracy);
	}
#else
	sprintf(buf, "%+lf", tmp);
#endif
	setText(buf, color);
	
	m_scoreAccuracy += score;	/* ���_���Z */
	m_score = m_scoreTyping + m_scoreAccuracy;
	(*lyricsPosition).isScoringTarget = false;	/* 2��ȏ㓾�_��^�����邱�Ƃ͂Ȃ� */
}

#define Y_INFO 10
#define Y_INFO2 40
#define X_LYRICS_KANJI 10
#define Y_LYRICS_KANJI 265
#define Y_LYRICS_KANJI_NEXT 300
#define Y_CIRCLE 190
#define R_CIRCLE 30
#define X_ACCURACY (100 - R_CIRCLE)
#define Y_ACCURACY 100
#define Y_LYRICS (Y_CIRCLE + R_CIRCLE + 10)
#define Y_LYRICS_BIG 340
#define Y_BUFFER 390

void CTyping::draw(){
	if(m_phase >= PHASE_RESULT){	/* �X�R�A�\���ȍ~ */
		drawResult();
		return;
	}
	double time;
	if(m_phase == PHASE_READY){
		SetFontSize(36);
		DrawString(50, 400, "�Ȃɂ��L�[�������ăX�^�[�g", GetColor(255, 255, 255));
		//return;
		time = 0.0;	/* �n�܂�O��0�b�Ŏ~�܂��Ă��� */
	}else{
		time = getTime();	/* �J�n��������̌o�ߕb���擾 */
	}
	//*
	DrawFormatStringToHandle(10, Y_INFO, GetColor(255, 255, 255), m_fontHandleNormal,
		"%10d �_", m_score);
	DrawFormatStringToHandle(10, Y_INFO2, GetColor(255, 255, 255), m_fontHandleNormal,
		"%10d �R���{", m_combo);
	/*///*
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
	
	/* �\������̎����o�� */
	while(time >= (*m_lyricsKanjiPosition).timeEnd){	/* �\���I�����Ă���Ȃ玟�ɐi�� */
		m_lyricsKanjiPosition++;
	}
	{
		int strWidth = GetDrawFormatStringWidthToHandle(m_fontHandleNormal, "Next: ");
		DrawFormatStringToHandle(X_LYRICS_KANJI, Y_LYRICS_KANJI_NEXT,
			GetColor(255, 255, 255), m_fontHandleNormal, "Next: ");
		if(time >= (*m_lyricsKanjiPosition).timeBegin){	/* �\���J�n���Ă���Ȃ� */
			DrawFormatStringToHandle(X_LYRICS_KANJI + strWidth, Y_LYRICS_KANJI,
				GetColor(255, 255, 255), m_fontHandleNormal,
				"%s", (*m_lyricsKanjiPosition).str.c_str());	/* �o�� */
			DrawFormatStringToHandle(X_LYRICS_KANJI + strWidth, Y_LYRICS_KANJI_NEXT,
				GetColor(255, 255, 255), m_fontHandleNormal,
				"%s", (*(m_lyricsKanjiPosition + 1)).str.c_str());	/* Next���o�� */
		}else{
			DrawFormatStringToHandle(X_LYRICS_KANJI + strWidth, Y_LYRICS_KANJI_NEXT,
				GetColor(255, 255, 255), m_fontHandleNormal,
				"%s", (*m_lyricsKanjiPosition).str.c_str());	/* Next���o�� */
		}
	}
	
	for(vector<Lyrics>::iterator i = m_lyrics.end() - 1; i != m_lyrics.begin() - 1; i--){
		if(!(*i).isBlockStart){	/* ���߂̍ŏ��łȂ��Ȃ疳�� */
			continue;
		}
		if((*i).ch == '\n'){	/* �Ō�ɂ���ԕ�������\�����Ȃ� */
			continue;
		}
		double timeDiff = time - (*i).timeJust;	/* ���傤�Ǒłʒu�ɂȂ��Ă��牽�b�ォ */
		int pos = (int)(100 - timeDiff * 250.0);
		if(pos < -32 || pos > 640+32){	/* ��ʊO */
			continue;
		}
		
		if((*i).ch == ' '){	/* �̎��̐؂�� */
			if(timeDiff < 0.0){	/* �؂�ڂ͔���ʒu�ɗ�����O�̂Ƃ������\�� */
				DrawCircle(pos, Y_CIRCLE, R_CIRCLE, GetColor(128, 128, 128), false);	/* �D�F�̉~�𗬂� */
			}
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
		DrawCircle(pos, Y_CIRCLE, R_CIRCLE - 1, Color, TRUE);	/* �����~ */
		DrawCircle(pos, Y_CIRCLE, R_CIRCLE, GetColor(0, 0, 0), FALSE);	/* �����~�̗֊s */
		int strWidth = GetDrawStringWidthToHandle(buf, len, m_fontHandleNormal);
		DrawStringToHandle(pos - strWidth / 2, Y_LYRICS, buf,
			GetColor(255, 255, 255), m_fontHandleNormal);	/* �����~�ɏ�����镶�� */
	}
	DrawCircle(100, Y_CIRCLE, R_CIRCLE, GetColor(255, 255, 255), FALSE);	/* ����ʒu�̉~ */
	
	{	/* ���b�Z�[�W�\�� */
		double timeDiff = time - m_textTime;
		if(timeDiff <= 0.5){	/* ���b�Z�[�W�i�D�ǉs�j��0.5�b�ԕ\������� */
			//int strLen = strlen(m_text);
			//int strWidth = GetDrawStringWidthToHandle(m_text, strLen, m_fontHandleBig);
			int strX;
			//strX = 100 - strWidth / 2;
			strX = X_ACCURACY;	//100 - R_CIRCLE;	/* �~�̍��[ */
			int strY;	/* �\�������Y���W */
			strY = Y_ACCURACY;
			if(timeDiff < 0.05){	/* 0.05�b��ɂ͒�ʒu */
				strY += ((0.05 - timeDiff) / 0.05) * 10;	/* 10����������o�Ă��� */
			}
			DrawStringToHandle(strX, strY, m_text,
				m_textColor, m_fontHandleBig);
		}
	}
	
	/* ���ܑΏۂɂȂ��Ă���̎�(m_lyricsPosition���玟�̉̎��܂�)��\�� */
	if((*m_lyricsPosition).ch != '\n'){	/* �S�Ẳ̎��̌�ł͂Ȃ� */
		char buf[256];
		int len = 0;
		{
			vector<Lyrics>::iterator i = m_lyricsPosition;
			do{	/* �u���b�N���̖��^�C�v�̕�����buf�Ɋi�[ */
				buf[len++] = (*i).ch;
				i++;
			}while(!(*i).isBlockStart);
		}
		buf[len] = '\0';
		int strWidth = GetDrawStringWidthToHandle(buf, len, m_fontHandleBig);
		DrawStringToHandle(100 - strWidth / 2, Y_LYRICS_BIG, buf,
			GetColor(255, 255, 255), m_fontHandleBig);
	}
	
	/* �^�C�v����������\�� */
	{
		m_typeBuffer[m_typeBufferLen] = '\0';
		int strLen = strlen(m_typeBuffer);
		int strWidth = GetDrawStringWidthToHandle(m_typeBuffer, strLen, m_fontHandleBig);
		DrawStringToHandle(100 - strWidth / 2, Y_BUFFER, m_typeBuffer,
			GetColor(255, 255, 255), m_fontHandleBig);
	}
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
		DrawFormatStringToHandle(30, 150, GetColor(255, 255, 255), m_fontHandleBig,
			"�ő� %d �R���{", m_comboMax);
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
	if(time >= 4.4){
		DrawFormatStringToHandle(30, 375, GetColor(255, 255, 255), m_fontHandleNormal,
			"���O����͂��Ă������� :");
	}
	m_typeBuffer[m_typeBufferLen] = '\0';
	DrawFormatStringToHandle(60, 400, GetColor(255, 255, 255), m_fontHandleBig,
		"%s", m_typeBuffer);
	if(m_phase == PHASE_FINISHED){
		if(m_rank >= 0){
			int strWidth;
			strWidth = GetDrawFormatStringWidthToHandle( m_fontHandleBig,
				"%d �ʂɃ����N�C�����܂���", m_rank + 1);
			int boxWidth = strWidth + 100;
			if(boxWidth > 640){
				boxWidth = 640;
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

class CInfo{
public:
	void load(CTyping &typing);
	void loadRanking(CRanking &ranking);
public:
	char m_title[256];
	char m_artist[256];
	char m_fumenAuthor[256];
	char m_fumenFileName[256];
	char m_rankingFileName[256];
private:
	
};

void CInfo::load(CTyping &typing){
	typing.load(m_fumenFileName);
	typing.loadRanking(m_rankingFileName);
}

void CInfo::loadRanking(CRanking &ranking){
	ranking.open(m_rankingFileName);
	ranking.read();
}

/* ============================================================ */

void main2(CTyping &typing){
	while(1){
		if(ProcessMessage() == -1){
			return;
		}
		if(!typing.idle()){
			break;
		}
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
		typing.draw();
		ScreenFlip();
		char ch = GetInputChar(TRUE);
		if(ch == CTRL_CODE_ESC){	/* Esc�Œ��f */
			return;
		}
		//if(ch != 0 && ch >= CTRL_CODE_CMP){
		if(ch != 0){	/* �o�b�t�@�ɉ����Ȃ���Ί֐����Ă΂Ȃ� */
			typing.keyboard(ch);
		}
	}
}

vector<CInfo> g_infoArray;

vector<CInfo>::iterator prevInfo(vector<CInfo>::iterator itr){
	if(itr == g_infoArray.begin()){
		itr = g_infoArray.end();
	}
	itr--;
	return itr;
}

vector<CInfo>::iterator nextInfo(vector<CInfo>::iterator itr){
	itr++;
	if(itr == g_infoArray.end()){
		itr = g_infoArray.begin();
	}
	return itr;
}

void main1(){
	{
		g_infoArray.clear();
		FILE *fp;
		fp = fopen("UTyping_list.txt", "r");
		if(fp == NULL){
			throw __LINE__;
		}
		char buf[256];
		while(fgetline(buf, fp) != NULL){
			FILE *fpInfo;
			CInfo info;
			fpInfo = fopen(buf, "r");	/* ���t�@�C�����J�� */
			if(fp == NULL){	/* �J���Ȃ��t�@�C���͖��� */
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
			g_infoArray.push_back(info);
		}
		fclose(fp);
	}
	vector<CInfo>::iterator infoArrayItr = g_infoArray.begin();
	CRanking ranking;
	(*infoArrayItr).loadRanking(ranking);
	int rankingPos = 0;
	
	while(1){
		if(ProcessMessage() == -1){
			return;
		}
		char ch = GetInputChar(TRUE);
		switch(ch){
		case CTRL_CODE_ESC:	/* �I�� */
			goto L1;
		case CTRL_CODE_UP:	/* 1�߂�B�ŏ�����1�߂�ƍŌ�ɁB */
			infoArrayItr = prevInfo(infoArrayItr);
			(*infoArrayItr).loadRanking(ranking);
			rankingPos = 0;
			break;
		case CTRL_CODE_DOWN:	/* 1�i�ށB1�i��ōŌ�𒴂���ƍŏ��ɁB */
			infoArrayItr = nextInfo(infoArrayItr);
			(*infoArrayItr).loadRanking(ranking);
			rankingPos = 0;
			break;
		case CTRL_CODE_LEFT:	/* ��ʂ�\�� */
			if(rankingPos == 0){
				rankingPos = RANKING_LEN;
			}
			rankingPos -= RANKING_DRAW_LEN;
			break;
		case CTRL_CODE_RIGHT:	/* ���ʂ�\�� */
			rankingPos += RANKING_DRAW_LEN;
			if(rankingPos == RANKING_LEN){
				rankingPos = 0;
			}
			break;
		case CTRL_CODE_CR:	/* �Q�[���J�n */
			ranking.close();
			{
				CTyping typing;
				(*infoArrayItr).load(typing);	/* �Q�[�����u�Ɂv���ʁE�n�C�X�R�A�̃t�@�C�������[�h */
				main2(typing);
			}
			(*infoArrayItr).loadRanking(ranking);
			break;
		default:
			break;
		}
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
		
		DrawLine(10, 80, 630, 80, GetColor(96, 96, 96));
		DrawLine(40, 160, 600, 160, GetColor(64, 64, 64));
		DrawLine(10, 400, 630, 400, GetColor(96, 96, 96));
		
		/* �^�C�g�������� */
		SetFontSize(24);
		DrawString(50, 40 - 12, (*prevInfo(infoArrayItr)).m_title, GetColor(128, 128, 128));
		DrawString(50, 120 - 12, (*infoArrayItr).m_title, GetColor(255, 255, 255));
		DrawString(50, 440 - 12, (*nextInfo(infoArrayItr)).m_title, GetColor(128, 128, 128));
		
		SetFontSize(16);
		int width;
		width = GetDrawFormatStringWidth("%s", (*infoArrayItr).m_artist);
		DrawFormatString(590 - width, 154 - 38, GetColor(255, 255, 255),
			"%s", (*infoArrayItr).m_artist);
		
		width = GetDrawFormatStringWidth("(���ʍ쐬�@%s)", (*infoArrayItr).m_fumenAuthor);
		DrawFormatString(590 - width, 154 - 16, GetColor(255, 255, 255),
			"(���ʍ쐬�@%s)", (*infoArrayItr).m_fumenAuthor);
		
		ranking.draw(160, rankingPos);
		
		ScreenFlip();
	}
L1:
	return;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPSTR lpCmdLine, int nCmdShow){
	ChangeWindowMode(TRUE);
#ifndef DEBUG_MODE
	SetMainWindowText("UTyping (c)2007 tos");
#else
	SetMainWindowText("UTyping (c)2007 tos -- (Debug Mode)");
#endif
	if(DxLib_Init() == -1){
		return -1;
	}
	try{
		main1();
	}catch(int n){
		FILE *fp;
		fp = fopen("�G���[���O.txt","a");
		fprintf(fp, "%d �s�ڂŃG���[���������܂����B\n", n);
		fclose(fp);
	}
	DxLib_End();
	return 0;
}
