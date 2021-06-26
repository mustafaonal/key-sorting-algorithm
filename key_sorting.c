#include <json/json.h>
#include<json-c/json.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define RECORD_COUNT 100

int studentNumber = 0;

struct _JsonFile
{
    char dataFilename[20];
    char indexFilename[20];
    int recordLength;
    char keyEncoding[3];
    int keyStart;
    int keyEnd;
    char order[4];
};
typedef struct _JsonFile JsonFile;

struct _Student {
	char rowID[4];
	char studentID[10];
    char firstName[20];
    char lastName[20];
    char phone[10];
};

typedef struct _Student Student;

struct _Index {
	char name[22];
	int index;
	
};
typedef struct _Index Index;

int compareStudents(const void* a, const void* b){
	return (strcmp(((Index*)a)->name, ((Index*)b)->name));
}

JsonFile bufferJson;

void readJsonFile(char *name) {
    FILE *fpointer;
	char *buffer;
    unsigned long fileLen;

	fpointer = fopen("generated.json","r");
    if (!fpointer)
	{
		fprintf(stderr, "Unable to open file %s", name);
		return;
	}
    fseek(fpointer, 0, SEEK_END);
	fileLen=ftell(fpointer);
	fseek(fpointer, 0, SEEK_SET);
    buffer=(char *)malloc(fileLen+1);
    if (!buffer)
	{
		fprintf(stderr, "Memory error!");
                                fclose(fpointer);
		return;
	}

	fread(buffer, fileLen, 1, fpointer);
	fclose(fpointer);

    /*struct json_object *parsed_json;
    struct json_object *dataFilename;
	struct json_object *indexFile;
	struct json_object *recordLength;
	struct json_object *keyEncoding;
    struct json_object *keyStart;
    struct json_object *keyEnd;
    struct json_object *order;
	parsed_json = json_tokener_parse(buffer);
	json_object_object_get_ex(parsed_json, "dataFilename", &dataFilename);
	json_object_object_get_ex(parsed_json, "indexFile", &indexFile);
	json_object_object_get_ex(parsed_json, "recordLength", &recordLength);
    json_object_object_get_ex(parsed_json, "keyEncoding", &keyEncoding);
    json_object_object_get_ex(parsed_json, "keyStart", &keyStart);
    json_object_object_get_ex(parsed_json, "keyEnd", &keyEnd);
    json_object_object_get_ex(parsed_json, "order", &order);
	printf("dataFilename: %s\n", json_object_get_string(dataFilename));
    printf("indexFile: %s\n", json_object_get_string(indexFile));
    printf("recordLength: %d\n", json_object_get_int(recordLength));
    printf("keyEncoding: %s\n", json_object_get_string(keyEncoding));
    printf("keyStart: %d\n", json_object_get_int(keyStart));
    printf("keyEnd: %d\n", json_object_get_int(keyEnd));
    printf("order: %s\n", json_object_get_string(order));*/
    //gcc assigment2.c -o assigment2.out yapınca ve ./assigment2.out yapınca çalışıyor.

    //yukarı kısımda parse ediyor ama çalıştırırken undefined reference to diye hata veriyor

    int i;
    char *token = "\"";
    char *token2 = ",";
    int c = 0;
    
    for(i = 0; i< fileLen;i++){
        if(buffer[i] == token[0] ) {
            c++;
            int j = i+1;
            int f = i+3;
             int k = 0;
             char StringToInt[10];
            while(c == 3 && buffer[j] != token[0] || c == 7 && buffer[j] != token[0] || c == 13 && buffer[j] != token[0] || c == 21 && buffer[j] != token[0] || c == 10 && buffer[f] != token2[0] || c == 16 && buffer[f] != token2[0] || c == 18 && buffer[f] != token2[0]) {
                if(c == 3) {
                    bufferJson.dataFilename[k] = buffer[j];
                    k++;
                }
                if(c == 7) {
                    bufferJson.indexFilename[k] = buffer[j];
                    k++;
                }
                if(c == 10) {
                    StringToInt[k] = buffer[f];
                    k++;
                    if(buffer[f+1] == token2[0] ) {
                        bufferJson.recordLength = atoi(StringToInt);
                    }
                }
                if(c == 13) {
                    bufferJson.keyEncoding[k] = buffer[j];
                    k++;
                }
                if(c == 16) {
                     StringToInt[k] = buffer[f];
                    k++;
                    if(buffer[f+1] == token2[0] ) {
                        bufferJson.keyStart = atoi(StringToInt);
                    }
                }
                if(c == 18) {
                     StringToInt[k] = buffer[f];
                    k++;
                    if(buffer[f+1] == token2[0] ) {
                        bufferJson.keyEnd = atoi(StringToInt);
                    }
                }
                if(c == 21){
                    bufferJson.order[k] = buffer[j];
                    k++;
                }
                
                j++;
                f++;
            }
            memset(&StringToInt[0], 0, sizeof(StringToInt));
            
            
            
        }
    
    }

    printf("dataFilename: %s\n", bufferJson.dataFilename);
    printf("indexFile: %s\n", bufferJson.indexFilename);
    printf("recordLength: %d\n", bufferJson.recordLength);
    printf("keyEncoding: %s\n", bufferJson.keyEncoding);
    printf("keyStart: %d\n", bufferJson.keyStart);
    printf("keyEnd: %d\n", bufferJson.keyEnd);
    printf("order: %s\n", bufferJson.order);



    free(buffer);


}



void CreateIndexFile() {
 
    FILE* fp;
	fp = fopen("sample.dat", "rb");
	if(!fp)
		return;
	int i=0;
    Index indexes[100];
    Student data;
	fseek(fp, 0, SEEK_SET);
    fread(&data, sizeof(Student), 1, fp);
    studentNumber++;
    char buf[20];
    
    while(!feof(fp)) {
        fseek(fp, bufferJson.keyStart * (i+1), SEEK_SET);
        fread(buf, sizeof(char), bufferJson.keyEnd-bufferJson.keyStart, fp);
        strcpy(indexes[i].name, buf);
        indexes[i].index = i;
        i++;
        fseek(fp, bufferJson.recordLength * i, SEEK_SET);
        fread(&data, sizeof(Student), 1, fp);
        studentNumber++;


    }
    qsort(indexes, i, sizeof(Index), compareStudents);
	FILE* fp1;
	fp1 = fopen(bufferJson.indexFilename, "wb");
	if(!fp1)
		return;
	fseek(fp1, 0, SEEK_SET);
	fwrite(indexes, sizeof(Index), RECORD_COUNT, fp1);
	fclose(fp1);
	fclose(fp);
    

}




void ReadDataFile(char *name)
{
	FILE *file;
	unsigned char *buffer;
	unsigned long fileLen;

	//Open file
	file = fopen(name, "rb");
	if (!file)
	{
		fprintf(stderr, "Unable to open file %s", name);
		return;
	}
	
	//Get file length
	fseek(file, 0, SEEK_END);
	fileLen=ftell(file);
	fseek(file, 0, SEEK_SET);

	//Allocate memory
	buffer=(char *)malloc(fileLen+1);
	if (!buffer)
	{
		fprintf(stderr, "Memory error!");
                                fclose(file);
		return;
	}

	//Read file contents into buffer
	fread(buffer, fileLen, 1, file);
	fclose(file);
    
 
    
    for (int c=0;c<fileLen;c++) {

 
     printf("%.2x ", (int)buffer[c]);
     

     // put an extra space between every 4 bytes
      if (c % 4 == 3)
     {
         printf(" ");
     }
    

     // Display 16 bytes per line
     
     if (c % 16 == 15)
     {
         printf("\n");
     }
    }
    // Add an extra line feed for good measure
    printf("\n");





	free(buffer);
}

int search(char *key) {
    FILE *inputFile;
    inputFile = fopen(bufferJson.indexFilename, "rb");
    int returnKey;

    fseek(inputFile, studentNumber*sizeof(Index), SEEK_SET);
    int lastId = (ftell(inputFile)/sizeof(Index))-1;
    int i;
    for(i = 0;i<lastId;i++) {
        Index index;
        fseek(inputFile, i*sizeof(Index), SEEK_SET);
        fread(&index, sizeof(Index), 1, inputFile);
        if(strcmp(index.name, key) == 0){
            returnKey = index.index;
        }
    }

    fclose(inputFile);






    int result;


    fclose(inputFile);
    return result;



}


int main() {

    char menu_option;

    printf("        Key-Sorting Indexing Tool\n");
    printf("------------------------------------------\n\n");


    do{
    printf("    a. Open\n");
    printf("    b. create_index\n");
    printf("    c. search\n");
    printf("    d. closed\n");
    printf("    e. Save and quit.\n");
    printf(" Please enter an option from the menu: \n");
    scanf("%c",&menu_option);
    char JsonFileName[20];
    char key[20];



    switch(menu_option){

    case 'a':
    printf("please enter json file name: ");
    scanf("%s", JsonFileName);
    readJsonFile(JsonFileName);
    printf("Json file readed, hexadecimal reading of data file: \n");
    ReadDataFile(bufferJson.dataFilename);
    break;
    case 'b':
    CreateIndexFile();
    printf("index file created as %s\n", bufferJson.indexFilename);
    break;
    case'c':
    printf("please enter key: ");
    scanf("%s", key);
    search(key);
    break;
    case'd':
    printf("closed the used files and free the memory\n");
    break;
    }

    }while(menu_option !='e');



}
