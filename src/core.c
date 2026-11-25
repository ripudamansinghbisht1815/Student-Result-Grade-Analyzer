#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif
#include "app.h"
static void flushline(void){int c;while((c=getchar())!='\n'&&c!=EOF){}}
static int line(const char *p,char *b,size_t n){printf("%s",p);if(!fgets(b,n,stdin))return 0;char *q=strchr(b,'\n');if(q)*q=0;else flushline();return b[0]!=0;}
int ensureDirectories(void)
{
#ifdef _WIN32
    _mkdir("data");
    _mkdir("reports");
    _mkdir("backup");
#else
    mkdir("data", 0777);
    mkdir("reports", 0777);
    mkdir("backup", 0777);
#endif

    return 1;
}
void initializeDefaults(void){ensureDirectories();FILE*f=fopen(CONFIG_FILE,"rb");if(!f){SystemConfig c={DEFAULT_WEAK_THRESHOLD,MAX_SEMESTERS,MAX_SUBJECTS};f=fopen(CONFIG_FILE,"wb");if(f){fwrite(&c,sizeof c,1,f);fclose(f);}}else fclose(f);f=fopen(GRADE_FILE,"rb");if(!f){GradeConfig g[]={{"A+",90,100},{"A",80,90},{"B+",70,80},{"B",60,70},{"C",50,60},{"D",40,50},{"F",0,40}};f=fopen(GRADE_FILE,"wb");if(f){fwrite(g,sizeof g[0],7,f);fclose(f);}}else fclose(f);f=fopen(ADMIN_FILE,"rb");if(!f){Admin a={"admin","admin123"};f=fopen(ADMIN_FILE,"wb");if(f){fwrite(&a,sizeof a,1,f);fclose(f);}}else fclose(f);}
int adminLogin(void){Admin a;FILE*f=fopen(ADMIN_FILE,"rb");if(!f||fread(&a,sizeof a,1,f)!=1){if(f)fclose(f);return 0;}fclose(f);for(int k=3;k;k--){char u[MAX_USERNAME_LENGTH],p[MAX_PASSWORD_LENGTH];if(!line("Username: ",u,sizeof u)||!line("Password: ",p,sizeof p))return 0;if(!strcmp(u,a.username)&&!strcmp(p,a.password))return 1;printf("Invalid credentials. Attempts left: %d\n",k-1);}return 0;}
int saveStudent(const Student*s){FILE*f=fopen(STUDENT_FILE,"ab");if(!f)return 0;int ok=fwrite(s,sizeof *s,1,f)==1;fclose(f);return ok;}
int findStudentById(const char*id,Student*out){FILE*f=fopen(STUDENT_FILE,"rb");if(!f)return 0;Student s;while(fread(&s,sizeof s,1,f)==1)if(!strcmp(s.studentId,id)){if(out)*out=s;fclose(f);return 1;}fclose(f);return 0;}
int studentIdExists(const char*id){return findStudentById(id,NULL);}
int rollExists(const char*r){FILE*f=fopen(STUDENT_FILE,"rb");if(!f)return 0;Student s;while(fread(&s,sizeof s,1,f)==1)if(!strcmp(s.rollNumber,r)){fclose(f);return 1;}fclose(f);return 0;}
void generateStudentId(char out[]){int hi=1000;FILE*f=fopen(STUDENT_FILE,"rb");Student s;if(f){while(fread(&s,sizeof s,1,f)==1){int n;if(sscanf(s.studentId,"STU%d",&n)==1&&n>hi)hi=n;}fclose(f);}snprintf(out,MAX_ID_LENGTH,"STU%d",hi+1);}
int saveSubject(const Subject*s){FILE*f=fopen(SUBJECT_FILE,"ab");if(!f)return 0;int ok=fwrite(s,sizeof *s,1,f)==1;fclose(f);return ok;}
int findSubject(const char*c,Subject*out){FILE*f=fopen(SUBJECT_FILE,"rb");if(!f)return 0;Subject s;while(fread(&s,sizeof s,1,f)==1)if(!strcmp(s.subjectCode,c)){if(out)*out=s;fclose(f);return 1;}fclose(f);return 0;}
int subjectHasMarks(const char*code){FILE*f=fopen(MARKS_FILE,"rb");if(!f)return 0;MarkRecord m;while(fread(&m,sizeof m,1,f)==1)if(!strcmp(m.subjectCode,code)){fclose(f);return 1;}fclose(f);return 0;}
int saveMark(const MarkRecord*m){FILE*f=fopen(MARKS_FILE,"ab");if(!f)return 0;int ok=fwrite(m,sizeof *m,1,f)==1;fclose(f);return ok;}
int markExists(const char*id,const char*code,int sem){FILE*f=fopen(MARKS_FILE,"rb");if(!f)return 0;MarkRecord m;while(fread(&m,sizeof m,1,f)==1)if(!strcmp(m.studentId,id)&&!strcmp(m.subjectCode,code)&&m.semester==sem){fclose(f);return 1;}fclose(f);return 0;}
int invalidateResult(const char*id,int sem){FILE*in=fopen(RESULT_FILE,"rb");if(!in)return 1;FILE*out=fopen("data/results.tmp","wb");if(!out){fclose(in);return 0;}SemesterResult r;int ok=1;while(fread(&r,sizeof r,1,in)==1)if(strcmp(r.studentId,id)||r.semester!=sem)if(fwrite(&r,sizeof r,1,out)!=1){ok=0;break;}fclose(in);fclose(out);if(!ok){remove("data/results.tmp");return 0;}remove(RESULT_FILE);return rename("data/results.tmp",RESULT_FILE)==0;}
int upsertResult(const SemesterResult*r){if(!invalidateResult(r->studentId,r->semester))return 0;FILE*f=fopen(RESULT_FILE,"ab");if(!f)return 0;int ok=fwrite(r,sizeof *r,1,f)==1;fclose(f);return ok;}
int findResult(const char*id,int sem,SemesterResult*out){FILE*f=fopen(RESULT_FILE,"rb");if(!f)return 0;SemesterResult r;while(fread(&r,sizeof r,1,f)==1)if(!strcmp(r.studentId,id)&&r.semester==sem){if(out)*out=r;fclose(f);return 1;}fclose(f);return 0;}
float percentage(float a,float b){return b>0?a/b*100.0f:0;}
void getGrade(float p,char out[]){FILE*f=fopen(GRADE_FILE,"rb");GradeConfig g;if(f){while(fread(&g,sizeof g,1,f)==1)if(p>=g.minimumPercentage&&(p<g.maximumPercentage||(fabsf(p-100.0f)<0.0001f&&fabsf(g.maximumPercentage-100.0f)<0.0001f))){strcpy(out,g.grade);fclose(f);return;}fclose(f);}strcpy(out,"N/A");}
float weakThreshold(void){SystemConfig c;FILE*f=fopen(CONFIG_FILE,"rb");if(f&&fread(&c,sizeof c,1,f)==1){fclose(f);return c.weakSubjectThreshold;}if(f)fclose(f);return DEFAULT_WEAK_THRESHOLD;}
int setWeakThreshold(float value){if(value<0||value>100)return 0;SystemConfig c={value,MAX_SEMESTERS,MAX_SUBJECTS};FILE*f=fopen(CONFIG_FILE,"wb");if(!f)return 0;int ok=fwrite(&c,sizeof c,1,f)==1;fclose(f);return ok;}
static int cmpGrade(const void*a,const void*b){const GradeConfig*x=a,*y=b;return x->minimumPercentage<y->minimumPercentage?-1:x->minimumPercentage>y->minimumPercentage?1:0;}
int validateGrades(const GradeConfig grades[],int count){if(count<1||count>MAX_GRADE_CONFIGS)return 0;GradeConfig a[MAX_GRADE_CONFIGS];memcpy(a,grades,(size_t)count*sizeof a[0]);qsort(a,(size_t)count,sizeof a[0],cmpGrade);if(fabsf(a[0].minimumPercentage)>0.001f||fabsf(a[count-1].maximumPercentage-100)>0.001f)return 0;for(int i=0;i<count;i++){if(!a[i].grade[0]||a[i].minimumPercentage<0||a[i].maximumPercentage>100||a[i].minimumPercentage>=a[i].maximumPercentage)return 0;if(i&&fabsf(a[i-1].maximumPercentage-a[i].minimumPercentage)>0.001f)return 0;}return 1;}
int saveGrades(const GradeConfig grades[],int count){if(!validateGrades(grades,count))return 0;FILE*f=fopen(GRADE_FILE,"wb");if(!f)return 0;int ok=fwrite(grades,sizeof grades[0],(size_t)count,f)==(size_t)count;fclose(f);if(ok)remove(RESULT_FILE);return ok;}
static int cmpRank(const void*a,const void*b){float x=((const RankingRecord*)b)->percentage-((const RankingRecord*)a)->percentage;return x>0?1:x<0?-1:0;}
int buildRanking(int sem,RankingRecord out[],int max){FILE*f=fopen(RESULT_FILE,"rb");if(!f)return 0;SemesterResult r;int n=0;while(n<max&&fread(&r,sizeof r,1,f)==1)if(r.semester==sem){Student s;if(findStudentById(r.studentId,&s)){strcpy(out[n].studentId,s.studentId);strcpy(out[n].rollNumber,s.rollNumber);strcpy(out[n].studentName,s.name);out[n].percentage=r.percentage;out[n].passed=r.passed;n++;}}fclose(f);qsort(out,(size_t)n,sizeof out[0],cmpRank);for(int i=0;i<n;i++)out[i].rank=(i&&fabsf(out[i].percentage-out[i-1].percentage)<0.001f)?out[i-1].rank:i+1;return n;}
int getRank(const char*id,int sem,int*total){RankingRecord a[MAX_STUDENTS];int n=buildRanking(sem,a,MAX_STUDENTS);if(total)*total=n;for(int i=0;i<n;i++)if(!strcmp(a[i].studentId,id))return a[i].rank;return 0;}
static int filterStudentFile(const char*path,size_t sz,const char*id,int kind){FILE*in=fopen(path,"rb");if(!in)return 1;FILE*out=fopen("data/filter.tmp","wb");if(!out){fclose(in);return 0;}union { Student student; MarkRecord mark; SemesterResult result; } record; int ok=1;while(fread(&record,sz,1,in)==1){int keep=1;if(kind==0)keep=strcmp(record.student.studentId,id)!=0;else if(kind==1)keep=strcmp(record.mark.studentId,id)!=0;else keep=strcmp(record.result.studentId,id)!=0;if(keep&&fwrite(&record,sz,1,out)!=1){ok=0;break;}}fclose(in);fclose(out);if(!ok){remove("data/filter.tmp");return 0;}remove(path);return rename("data/filter.tmp",path)==0;}
int deleteStudentCascade(const char*id){return filterStudentFile(STUDENT_FILE,sizeof(Student),id,0)&&filterStudentFile(MARKS_FILE,sizeof(MarkRecord),id,1)&&filterStudentFile(RESULT_FILE,sizeof(SemesterResult),id,2);}
static int copyFile(const char*s,const char*d){FILE*in=fopen(s,"rb");if(!in)return 1;FILE*out=fopen(d,"wb");if(!out){fclose(in);return 0;}unsigned char b[4096];size_t n;int ok=1;while((n=fread(b,1,sizeof b,in))>0)if(fwrite(b,1,n,out)!=n){ok=0;break;}if(ferror(in))ok=0;fclose(in);fclose(out);return ok;}
int backupData(void){ensureDirectories();const char*src[]={STUDENT_FILE,SUBJECT_FILE,MARKS_FILE,RESULT_FILE,GRADE_FILE,CONFIG_FILE};const char*dst[]={"backup/students.dat","backup/subjects.dat","backup/marks.dat","backup/results.dat","backup/grades.dat","backup/config.dat"};for(int i=0;i<6;i++)if(!copyFile(src[i],dst[i]))return 0;return 1;}
int restoreData(void){const char*src[]={"backup/students.dat","backup/subjects.dat","backup/marks.dat","backup/results.dat","backup/grades.dat","backup/config.dat"};const char*dst[]={STUDENT_FILE,SUBJECT_FILE,MARKS_FILE,RESULT_FILE,GRADE_FILE,CONFIG_FILE};for(int i=0;i<6;i++)if(!copyFile(src[i],dst[i]))return 0;return 1;}
