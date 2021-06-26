#include <json/json.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int jsonParametersCheck[7];// controls existince of the every parameters from the json file
char *dataFileName;
char *indexFileName;
int recordLength;
char *keyEncoding;
int keyStart;
int keyEnd;
char *order;
json_object *jobj = NULL;
int size; // size of the indexes


// struct for the indexes, it holds key name and relative record number
struct _Index {
    char *name;
    int index;
};

typedef struct _Index Index;

int compareIndexesAscending(const void* a, const void* b){
    return(strcmp(((Index*)a)->name,((Index*)b)->name));
}
int compareIndexesDescending(const void* a, const void* b){
    return-(strcmp(((Index*)a)->name,((Index*)b)->name));
}
void openJson(char *jsonFileName)
{

    jobj = json_object_from_file(jsonFileName);
    if (jobj == NULL)
    {
        printf("The json file is not found!");
        exit(0);
    }
    else
    {
        //iterating for every object in the json file
        json_object_object_foreach(jobj, key, val)
        {
            if (strcmp(key, "dataFileName") == 0)
            {
                dataFileName = (char)malloc(strlen(json_object_get_string(val)) + 1);
                dataFileName = json_object_get_string(val);
                // if a parameter name matches then the control mechanism changes 0 to 1
                jsonParametersCheck[0] = 1;
            }
            else if (strcmp(key, "indexFileName") == 0)
            {
                indexFileName = (char)malloc(strlen(json_object_get_string(val)) + 1);
                indexFileName = json_object_get_string(val);
                jsonParametersCheck[1] = 1;
            }
            else if (strcmp(key, "recordLength") == 0)
            {
                recordLength = json_object_get_int(val);
                jsonParametersCheck[2] = 1;
            }
            else if (strcmp(key, "keyEncoding") == 0)
            {
                keyEncoding = (char)malloc(strlen(json_object_get_string(val)) + 1);
                keyEncoding = json_object_get_string(val);
                jsonParametersCheck[3] = 1;
            }
            else if (strcmp(key, "keyStart") == 0)
            {
                keyStart = json_object_get_int(val);
                jsonParametersCheck[4] = 1;
            }
            else if (strcmp(key, "keyEnd") == 0)
            {
                keyEnd = json_object_get_int(val);
                jsonParametersCheck[5] = 1;
            }
            else if (strcmp(key, "order") == 0)
            {
                order = (char)malloc(strlen(json_object_get_string(val)) + 1);
                order = json_object_get_string(val);
                jsonParametersCheck[6] = 1;
            }
        }
    }
}
void createIndexFile(){
    FILE* fp;
    //checking encoding to decide the type of reading operation
    if(strcmp(keyEncoding,"BIN")==0){
        fp= fopen(dataFileName,"rb");
    }
    else if(strcmp(keyEncoding,"CHR")==0 ||strcmp(keyEncoding,"UTF")==0){
        fp= fopen(dataFileName,"r");
    }
    
    if(!fp){
        printf("Data file is not found!\n");
        return;
    }
    
    fseek(fp,0L,SEEK_END);
    //calculating size of the indexes
    size = (ftell(fp)/recordLength);
    // size of the key names
    int indexNameLength = keyEnd-keyStart;
    Index indexes[size];

    // allocating memory for the each member's name variable
    for(int i=0;i<size;i++){
        indexes[i].name = malloc(sizeof(char)*indexNameLength);
    }
    
    fseek(fp,0L,SEEK_SET);
    // locating cursor to beginning of the key name
    fseek(fp,keyStart,SEEK_SET);
    char* name= (char *)malloc(indexNameLength*sizeof(char));
    fread(name,indexNameLength,1,fp);
    
    int i=0;
    
    while(i<size){
        strcpy(indexes[i].name,name);
        indexes[i].index=i;
        i++;
        // locating cursor to every record's starting key location
        fseek(fp,i*(recordLength)+keyStart,SEEK_SET);
        fread(name,indexNameLength,1,fp);
    }
    
    if(strcmp(order,"ASC")==0){
        qsort(indexes , size, sizeof(indexes[0]), compareIndexesAscending);
    }
    else if(strcmp(order,"DESC")==0){
        
        qsort(indexes , size, sizeof(indexes[0]), compareIndexesDescending);
    }
    for(int i=0;i<size;i++){
        
        printf("%s - %d\n",indexes[i].name,indexes[i].index);
    }
    FILE* fp1;
    if(strcmp(keyEncoding,"BIN")==0){
        fp1= fopen(indexFileName,"wb");
    }
    else if(strcmp(keyEncoding,"CHR")==0||strcmp(keyEncoding,"UTF")==0){
        fp1= fopen(indexFileName,"w");
    }
    
    if(!fp1)
        return;
    fseek(fp1,0L,SEEK_SET);
    if(strcmp(keyEncoding,"BIN")==0){
       fwrite(indexes,sizeof(Index),size,fp1);
        
    }
    else if(strcmp(keyEncoding,"CHR")==0||strcmp(keyEncoding,"UTF")==0){
        for(int i=0;i<size;i++){
            fprintf(fp1,"%s%d",indexes[i].name,indexes[i].index);
        }   
    }
    fclose(fp1);
    fclose(fp);
}

int binaryFileSearch(FILE* fp, char* name,int firstIdx,int lastIdx){
    
	Index first,last,middle;
    
    first.name= malloc(sizeof(char)*(keyEnd-keyStart));
    last.name= malloc(sizeof(char)*(keyEnd-keyStart));
    middle.name= malloc(sizeof(char)*(keyEnd-keyStart));
	int returnData;
	
	// Calculate the middle Index
	int middleIdx = (firstIdx+lastIdx)/2;
    
	// Read first record and return if it is the searched one.
	fseek(fp, firstIdx*(sizeof(Index)), SEEK_SET);
	fread(&first, sizeof(Index), 1, fp);
    
    
	if(strcmp(first.name,name) == 0)
	{
		returnData=first.index;
		return returnData;
	}
	// Read last record and return if it is the searched one.
	fseek(fp, lastIdx*sizeof(Index), SEEK_SET);
	fread(&last, sizeof(Index), 1, fp);
	if(strcmp(last.name,name) == 0)
	{
		returnData=last.index;
		
		return returnData;
	}
	// Recursion exit condition, If middle index is equal to first or last index
	// required comparisons are already made, so record is not found.
	// Create and return an empty person.
	if(middleIdx==firstIdx || middleIdx == lastIdx) {
		int d=-1;
		return d;
	}

	// Read the middle record and return if it is the searched one.
	fseek(fp, middleIdx*sizeof(Index), SEEK_SET);
	fread(&middle, sizeof(Index), 1, fp);
	if(strcmp(middle.name,name) == 0)
	{
		returnData=middle.index;
		return returnData;
	}
	// Determine the record position and recursively call with appropriate attributes.
	if(strcmp(middle.name,name)>0) {
		return binaryFileSearch(fp, name, firstIdx+1, middleIdx-1);
	} 
	else {
		return binaryFileSearch(fp, name, middleIdx+1, lastIdx-1);
	}
}
int findRecordByName (char *searchName){
    int lastIdx;
    int firstIdx=0;
    
    FILE* indexFile;
    indexFile= fopen(indexFileName,"rb");
    fseek(indexFile,size*sizeof(Index),SEEK_SET);
    lastIdx = (ftell(indexFile)/sizeof(Index))-1;
    int result=binaryFileSearch(indexFile,searchName,firstIdx,lastIdx);
    
    fclose(indexFile);
    return result;

}
void printMenu(int* choice){
    printf("You can perform the following tasks\n");
    printf("(1) Open a Json file(with .json extension) to read parameters\n");
    printf("(2) Create the index file for given parameters\n");
    printf("(3)Search a record for the given key\n");
    printf("(4) Quit\n");
    printf("Enter your choice: \n");
    scanf("%d",choice);
}
int main()
{

    int choice;
    int who;
    char *jsonFileName;
    jsonFileName = malloc(sizeof(char)*100);

    label:

    printMenu(&choice);
    // for the invalid choice situation
    while(choice>4 || choice <1){
        printf("\n Enter a valid number to perform a operation!\n");
        printMenu(&choice);
    }

    switch(choice){
        //fetching parameters from json file
        case 1: 
        printf("Enter your json file's name: ");
        scanf("%s",jsonFileName);
        openJson(jsonFileName);
        int i=0;
        for (; i < 7; i++)
        {
            if (jsonParametersCheck[i] == 0)
            {
                break;
            }
        }
        
        if(i!=7){// means one or more parameter(s) are not found in the json file
            printf("Some parameter(s) are not found!");
            exit(0);
        }
        goto label;
        break;
        // creating index file for given parameters
        case 2: createIndexFile();
        goto label;
        break;
        case 3:
        printf("Enter the name to search: \n");
        char *searchName = malloc(sizeof(char)*100);
        scanf("%s",searchName);
        who =findRecordByName(searchName);//getting index of the desired record
        if(who!=-1){
            FILE* fp2;
            fp2=fopen(dataFileName,"rb");
            fseek(fp2,who*recordLength,SEEK_SET);
            char *data = malloc(sizeof(char)*recordLength+1);
            fread(data,recordLength,1,fp2);
            printf("%s\n",data);
        }
        else{
            printf("record is not found!");
        }
        goto label;
        break;
        //exit the program
        case 4:
        exit(0);
        break;
    }
    return 0;
}
