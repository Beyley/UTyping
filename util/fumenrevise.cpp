/*
                  argv[1]      argv[2] argv[3]              argv[4]
 >fumenrevise.exe hanbetsu.txt lor.txt revised_hanbetsu.txt 0.05

  argv[1] : ���Ƃ̕��ʂ̃t�@�C����
  argv[2] : list of replays ���L�����t�@�C����
  argv[3] : �␳���ꂽ���ʂ����̖��O�ŕۑ�
  argv[4] : ``epsilon'' : �덷���E
            ���ԍ� epsilon �ȏ�̉�������������Ȃ��ꍇ�̓~�X�Ƃ݂Ȃ�

  argv[2] �̊e�s�ɂ̓��v���C�̃t�@�C�����������D1 �s�� 1 �D�ő� 10 �D



  ���v���C�̓f�o�b�O���[�h�ō�������̂��g�p���邱�Ƃ������������܂��D
  �i���ʂ̊e�����ɑ΂����v���C���� 1 �̉�����Ή����������̂Łj
  �i���Ȃ݂ɂ��̃v���O�����̓��v���C�̕������͓ǂ�ł܂���j


  �u���ߐ��v�u�����v�u�ł��؂�_�v��
  ���Ɂu�����v�����ꂽ�Ƃ��ɓ��������ł܂Ƃ߂ďo�͂��܂��D
  ���������āC�u�����v�̂Ȃ������́u���ߐ��v�u�����v�u�ł��؂�_�v�͂��܂��o�͂���܂���D
      �Ō�̉����ȍ~�́u���ߐ��v�u�����v�u�ł��؂�_�v�͏o�͂���܂���D
  �����͎��Ƃŏ������ޕK�v������܂��D

  argv[1] �ɓ������̂́C���ۂ̕��ʂł͂Ȃ�
  ���̋Ȃ̏��ߐ��E�����������ɂ������̂ɂ��Ă�����
  ���Ƃ��Ɗy��������܂���D


-- 
mos

*/

#include<stdio.h>
#include<math.h>
#include<string.h>



int main(int args, char* argv[]){
    if(args < 4+1) return 1;

    FILE * pfread, * pflor, * pfreplay[30], * pfwrite;
    double d;
    char c;
//    char *str;
    int nreplay = 0;	// number of available replays
    bool waiting_slash, waiting_star, waiting_minus, waiting_equal;
    char buf[256];	// temporary
    char kanjibuf[256];	// to store KanjiLyrics 
    double data[30][1000];
    int datalength[30];
    int iter[30];

    double epsilon;

    if((pfread = fopen (argv[1], "r")) == NULL)
        return 1;
    if((pflor   = fopen (argv[2], "r")) == NULL)
        return 1;
    if((pfwrite = fopen (argv[3], "w")) == NULL)
        return 1;
    sscanf(argv[4], "%lf", &epsilon);

    // argv[2] �ɍڂ��Ă��郊�v���C�t�@�C�����J��
    for(int i=0; i<30; i++){
        if(fgets(buf, sizeof(buf), pflor) == NULL){
           fprintf(stdout, "%d replays found, no more replays.\r\n", i);
           break;
        }
        for(int k=0; k<256; k++){
            if(buf[k]=='\n' || buf[k]=='\r' || buf[k]=='\0'){
                buf[k] = '\0';
                break;
            }
        }

        if((pfreplay[i] = fopen(buf, "rb")) == NULL){
            printf("replay #%d unavailable\r\n", i);
            break;
        }
        nreplay++;
    }
    fclose(pflor);


    // ���v���C���J���Ď����f�[�^���i�[�i�����f�[�^�͖����j
    for(int i=0; i<nreplay; i++){
        fprintf(stdout, "[replay file %2d of %2d] ", i+1, nreplay);

        int j;
        data[i][0] = -90;
        for(j=1; j<1000; j++){
            if((c = getc(pfreplay[i])) == EOF){
                fprintf(stdout, "eof\n");
                break;
            }
            if((fread(&d, sizeof(d), 1, pfreplay[i])) < 1){
                fprintf(stderr, "replay format error at #%d: <%c %f>\r\n", j, c, d);
                break;
            }      
      
            data[i][j] = d;
        }
        datalength[i] = j;
        iter[i] = 0;
    }

    // ���ʃt�@�C���̊e�s�ɂ��āc�c
    waiting_equal = waiting_minus = waiting_slash = waiting_star = false;
  while(fgets(buf, sizeof(buf), pfread) != NULL){
    switch(buf[0]){
        case '@': // �����t�@�C���w��Ȃ̂ł��̂܂ܒʂ�
            fprintf(pfwrite, "%s", buf);
            break;

        case '=': // �����͎��� '+' �ł܂Ƃ߂ď���
            waiting_equal = true;
            break;
        case '-': 
            waiting_minus = true;
            break;
        case '/': 
            waiting_slash = true;
            break;
        case '*': // KanjiLyrics �͋L�����Ă���
            waiting_star  = true;
            sscanf(buf+1, "%lf %s", &d, kanjibuf);
            break;

        case '+': // �����̏ꍇ
            if(sscanf(buf+1, "%lf %s", &d, buf) == EOF){
                fprintf(stdout, "error at %lf %s\n", d, buf);
                break;
            }

            int howmanyhas = 0;
            double moveby = 0;
            double d1, d2;
            double d_new;

            for(int i=0; i<nreplay; i++){
                while(data[i][iter[i]] < d && iter[i] < datalength[i])
                    iter[i]++;
                	// so that [iter[i]] <= d < [iter[i+1]] 

                d1 = data[i][iter[i]] - d;
                d2 = data[i][iter[i+1]] - d;

                if(fabs(d1) < epsilon || fabs(d2) < epsilon){
                    howmanyhas++;
                    if(fabs(d1) < fabs(d2))
                        moveby += d1;
                    else
                        moveby += d2;
                }
            }
            if(howmanyhas > 0)
                d_new = d + moveby/howmanyhas;
            else 
                d_new = d;
                                    
            // '=' '-' '*' '/' ���܂Ƃ߂ď���
            if(waiting_equal)
                fprintf(pfwrite, "=%lf\r\n", d_new);
            if(waiting_minus)
                fprintf(pfwrite, "-%lf\r\n", d_new);
            if(waiting_slash)
                fprintf(pfwrite, "/%lf\r\n", d_new);
            if(waiting_star )
                fprintf(pfwrite, "*%lf %s\r\n", d_new, kanjibuf);

            waiting_equal = waiting_minus = waiting_slash = waiting_star = false;

            fprintf(pfwrite, "+%lf %s\r\n", d_new, buf);
    }


  }
    

    for(int i=0; i<nreplay; i++)
        fclose(pfreplay[i]);
    fclose(pfread);
    fclose(pfwrite);

    return 0;
}

