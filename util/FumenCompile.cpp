#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <string>

using namespace std;

struct Info{
	double speed;	/* 1���߂ɂ����鎞�ԁi���m�ɂ́A�l������4�ɂ����鎞�ԁj */
	int base;	/* �����������P�ʂœ��͂��Ă��邩 */
	double time;
	int beat_nu, beat_de;	/* �ꏬ�߂̕��q/���� */
	int beat_int;	/* ���݂̔����A�[�� */
	double beat_frac;
	/* �������A�[����(-1,0]�Ɏ��B��F2.25���ځi16������5�j��2,-0.75 */
};

void setSpeed(Info &info, char *str){
	int k;
	double t;
	/* ����=���� ��ǂ� */
	char *ptr = strtok(str, "= \t\n");
	if(ptr == NULL){
		throw __LINE__;
	}
	int n = sscanf(ptr, "%d", &k);
	if(n < 1){
		throw __LINE__;
	}
	ptr = strtok(NULL, " \t\n");
	if(ptr == NULL){
		throw __LINE__;
	}
	n = sscanf(ptr, "%lf", &t);
	if(n < 1){
		throw __LINE__;
	}
	if(k <= 0){	/* k��������k�͐����� */
		throw __LINE__;
	}
	if(t <= 0.0){	/* �e���|�����łȂ��Ƃ��������� */
		throw __LINE__;
	}
	info.speed =  60.0 / (t / k);
	return;
}

void setBase(Info &info, char *str){
	int k;
	int n = sscanf(str, "%d", &k);
	if(n < 1){
		throw __LINE__;
	}
	if(k <= 0){	/* k��������k�͐����� */
		throw __LINE__;
	}
	info.base = k;
	return;
}

void setBeat(Info &info, char *str){
	int nu, de;
	/* ����/���� ��ǂ� */
	char *ptr = strtok(str, "/ \t\n");
	if(ptr == NULL){
		throw __LINE__;
	}
	int n = sscanf(ptr, "%d", &nu);
	if(n < 1){
		throw __LINE__;
	}
	if(nu <= 0){	/*  0�ȉ��Ŕ��q���� */
		info.beat_nu = 0;
		info.beat_de = 0;
		info.beat_int = 0;
		info.beat_frac = 0.0;
		return;
	}
	ptr = strtok(NULL, ": \t\n");
	if(ptr == NULL){
		throw __LINE__;
	}
	n = sscanf(ptr, "%d", &de);
	if(n < 1){
		throw __LINE__;
	}
	if(de <= 0){	/* ����͐����� */
		throw __LINE__;
	}
	
	ptr = strtok(NULL, " \t\n");
	if(ptr == NULL){
		info.beat_int = 0;	/* �ʒu��ݒ肵�Ȃ��ꍇ�A0���ڂɂȂ� */
		info.beat_frac = 0.0;
	}else{
		double d;
		n = sscanf(ptr, "%lf", &d);
		if(n < 1){
			throw __LINE__;
		}
		info.beat_int = ceil(d);
		info.beat_frac = d - info.beat_int;
	}
	info.beat_nu = nu;
	info.beat_de = de;
	
	info.beat_int %= info.beat_nu;
	return;
}

void timeAdd(Info &info, vector<pair<double, int> > &timeArray){
	if(info.speed <= 0.0){
		throw __LINE__;
	}
	if(info.base <= 0){
		throw __LINE__;
	}
	double length = 1.0 / info.base;
	double dTime, dBeat;
	dTime = length * info.speed;
	if(info.beat_de > 0){	/* ���q����`����Ă��� */
		dBeat = length * info.beat_de;
		while(info.beat_frac + dBeat > -0.001){	/* �덷���ꉞ�C�ɂȂ�B0.999���ȍ~�Ƀe���|���ς�邱�Ƃ͂܂��Ȃ����낤�B */
			timeArray.push_back(make_pair(
				info.time - info.speed * (info.beat_frac / info.beat_de),	/* info.time ���瑫��Ȃ������₷�ibeat_frac�͕��j */
				(info.beat_int == 0) ? -1 : -2));
			
			info.time += info.speed / info.beat_de;	/* �ꔏ�����Ԃ�i�߂� */
			//length -= 1.0 / info.beat_de;
			dTime -= info.speed / info.beat_de;
			
			info.beat_int++;
			if(info.beat_int == info.beat_nu){
				info.beat_int = 0;
			}
			dBeat -= 1.0;
		}
		info.beat_frac += dBeat;
	}
	info.time += dTime;
	return;
}

void processCommand(char *str, Info &info){
	switch(str[0]){
	case 't':	/* t4=120 : �l���������ꕪ��120�� */
		setSpeed(info, str + 1);
		break;
	case 'b':	/* ���͂���n��������n��ݒ� */
	case 'l':	/* ���͂���n��������n��ݒ� */
		setBase(info, str + 1);
		break;
	case 'B':	/* ���q��ݒ�i��FB4/4�j�AB0�ŉ��� */
		setBeat(info, str + 1);
		break;
	case 'r':	/* r0.3��0.3�b�i�߂� */
		{
		double t;
		int n = sscanf(str + 1, "%lf", &t);
		if(n < 1){
			throw __LINE__;
		}
		info.time += t;
		}
		break;
	default:
		throw __LINE__;
	}
}

int main1(char *fileName){
	char directoryName[256];
	strcpy(directoryName, fileName);
	for(int i = strlen(directoryName) - 1; i >= 0; i--){	/* ��납�炽�ǂ� */
		if(directoryName[i] == '/' || directoryName[i] == '\\'){	/* '/'��'\'�����ꂽ�� */
			directoryName[i + 1] = '\0';	/* �����܂łȂ̂ŁA����'\0'�� */
			goto L2;
		}
	}
	/* �Ō�܂Ō���Ȃ������� */
	directoryName[0] = '\0';
L2:
	
	FILE *fp;
	fp = fopen(fileName, "r");
	if(!fp){
		throw __LINE__;
	}
	
	Info info;
	info.speed = -1.0;	/* 1���߂ɂ����鎞�� */
	info.base = 0;	/* �����������P�ʂœ��͂��Ă��邩 */
	info.time = 0.0;
	
	info.beat_nu = 0;
	info.beat_de = 0;
	
	info.beat_int = 0;	/* �ŏ���0���ڂƂ��� */
	info.beat_frac = 0.0;
	
	vector<pair<double, int> > timeArray;
		/* int�́A0:��؂�, 1:�̎��̋�̍ŏ��ȊO, 2:�̎��̋�̍ŏ� */
		/* -1:���ߐ�, -2:�u�����v */
	vector<string> lyricsArray;	/* �ł̎��i�ǂݕ��j */
	vector<string> lyricsKanjiArray;	/* ����镶���ƕʂɕ\������̎� */
	
	char buf[1024];
	
	if(fgets(buf, sizeof(buf), fp) == NULL){
		throw __LINE__;
	}
	int len = strlen(buf);
	if(buf[len-1] == '\n'){
		buf[len-1] = '\0';
	}
	char outFileName[256];
	sprintf(outFileName, "%s%s", directoryName, buf);	/* 1�s�ڂŏo�̓t�@�C�����w�� */
	//sprintf(outFileName, "%s%s-debug.txt", directoryName, buf);	/* 1�s�ڂŏo�̓t�@�C�����w�� */
	printf("%s�ɏo�͂��܂��B\n", outFileName);
	freopen(outFileName, "w", stdout);
	
	if(fgets(buf, sizeof(buf), fp) == NULL){
		throw __LINE__;
	}
	printf("@%s", buf);	/* 2�s�ڂ͋ȃt�@�C���������̂ł��̂܂܏o�� */
	
	while(fgets(buf, sizeof(buf), fp) != NULL){
		switch(buf[0]){
		case 'e':
			goto L1;	/* �ȍ~�̍s�𖳎� */
		case '#':	/* �R�����g�s */
			break;
		case '@':	/* 1�s���� */
			processCommand(buf + 1, info);
			break;
		case '\'':	/* �ł̎��i���p�X�y�[�X�A�^�u��؂�j */
			for(char *ptr = strtok(buf + 1, " \t\n"); ptr != NULL;
					ptr = strtok(NULL, " \t\n")){
				lyricsArray.push_back(string(ptr));
			}
			break;
		case '\"':	/* �\������̎��i�^�u��؂�j�i�Ă��A���ʋ�؂�Ȃ��j */
			for(char *ptr = strtok(buf + 1, "\t\n"); ptr != NULL;
					ptr = strtok(NULL, "\t\n")){
				lyricsKanjiArray.push_back(string(ptr));
			}
			break;
		default:
			for(char *ptr = buf; *ptr != '\0'; ptr++){
				switch(*ptr){
				case '[':	/* �R�}���h�J�n */
					{
						ptr++;
						char *ptrCmd = ptr;	/* �R�}���h�̊J�n�ʒu */
						while(*ptr != ']'){
							if(*ptr == '\0'){	/* �R�}���h�������Ȃ����� */
								throw __LINE__;
							}
							ptr++;
						}
						*ptr = '\0';	/* ']'��'\0'�ɏ��������� */
						processCommand(ptrCmd, info);
					}
					break;
				case ']':	/* �R�}���h���J���Ă��Ȃ��̂ɕ��邪���� */
					throw __LINE__;
				case '*':	/* �̎���1���ߖ� */
					/* �̎��̋�؂�����āA�̎������āA���Ԃ��o�߂����� */
					timeArray.push_back(make_pair(info.time, 0));
					timeArray.push_back(make_pair(info.time, 2));
					timeAdd(info, timeArray);
					break;
				case '+':	/* �̎���2���ߖڈȍ~ */
					/* �̎������āA���Ԃ��o�߂����� */
					timeArray.push_back(make_pair(info.time, 1));
					timeAdd(info, timeArray);
					break;
				case '-':	/* �����Ȃ��Ƃ��� */
					/* ���Ԃ��o�߂����� */
					timeAdd(info, timeArray);
					break;
				case '/':	/* ���̉̎��̑O�ɑł��؂���ꍇ */
					/* �̎��̋�؂�����āA���Ԃ��o�߂����� */
					timeArray.push_back(make_pair(info.time, 0));
					timeAdd(info, timeArray);
					break;
				}
			}
			break;
		}
	}
L1:
	timeArray.push_back(make_pair(info.time, 0));	/* �I����A��؂�����Ă��� */
	
	/* �����ŁA�C�ӂ̕\������̎��΂��āA��؂肪���ɑ��݂��A���ꂪ�\������̎��̏I���ƂȂ�B */
	
	vector<string>::iterator itrLyrics = lyricsArray.begin();
	vector<string>::iterator itrKanjiLyrics = lyricsKanjiArray.begin();
	bool flag = false;	/* ���O����؂肾�������̃t���O */
	for(vector<pair<double, int> >::iterator itr = timeArray.begin();
			itr != timeArray.end(); itr++){
		if((*itr).second <= -1){	/* ���ߐ��Ȃǂ����鎞�� */
			if((*itr).second == -1){	/* ���ߐ� */
				printf("=");
			}else{	/* ���� */
				printf("-");
			}
			printf("%lf\n", (*itr).first);
		}else if((*itr).second >= 1){	/* �̎��̓��鎞�� */
			if((*itr).second == 2){	/* �\������̎��̓��鎞�� */
				if(itrKanjiLyrics == lyricsKanjiArray.end()){
					throw __LINE__;
				}
				printf("*%lf %s\n", (*itr).first, (*itrKanjiLyrics).c_str());
				itrKanjiLyrics++;
			}
			if(itrLyrics == lyricsArray.end()){	/* �ł̎�������ׂ��Ƃ���ɉ̎����Ȃ� */
				throw __LINE__;
			}
			printf("+%lf %s\n", (*itr).first, (*itrLyrics).c_str());
			itrLyrics++;
			
			flag = false;
		}else{	/* �̎��̋�؂�̓��鎞�� */
			if(!flag){	/* ��؂肪�A�����邱�Ƃ�h�~ */
				printf("/%lf\n", (*itr).first);
			}
			flag = true;
		}
//printf("%lf %d\n",(*itr).first,(int)(*itr).second);
	}
	return 0;
}

int main(int argc, char **argv){
	if(argc < 2){
		return 0;
	}
	try{
		main1(argv[1]);
	}catch(int n){
		fprintf(stderr, "Error %d\n",n);
		getchar();
	}
	return 0;
}
