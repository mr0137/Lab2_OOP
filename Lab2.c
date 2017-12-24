#include "Lab2.h"

int create_db(const char* csv, const char *db)
{
	FILE *f_csv, *f_db;
	SCAN_INFO *record, *buffer;

	int num=0, check_copy=0;
	char n='\n';

	f_csv=fopen(csv , "r");
	f_db=fopen(db , "wb+");
	record=(SCAN_INFO*)malloc(sizeof(SCAN_INFO));
	
	
	if (f_csv==NULL || f_db==NULL || record==NULL )
	{
		printf("Open err");
		fclose(f_csv);
		fclose(f_db);
		return 0;
	}

	/*заносим элементы из csv в структуру*/
	while (!feof(f_csv))
	{
		fscanf(f_csv, "%[A-z0-9];%[A-z0-9];%d;%f;%d;%d\n", &record->manufacturer, &record->model, &(record->year), &(record->price), &(record->x_size), &(record->y_size));
		record->id=num;
		fseek(f_db, 0, SEEK_SET);
		buffer = (SCAN_INFO*)malloc(sizeof(SCAN_INFO));
		if(buffer == NULL)
			exit(0);
		/*запуск проверки на повторение с последующей записью структуры в файл .db*/
		if (!feof(f_db))
		{

			while (!feof(f_db))
			{
				fread(buffer, sizeof(SCAN_INFO), 1, f_db);
				
				if ((*record->manufacturer == *buffer->manufacturer) && (*record->model == *buffer->model) && (record->price == buffer->price) && (record->year == buffer->year))
				{
					check_copy=1;
				}
			}

			free(buffer);
		//	fseek(f_db, 0, SEEK_SET);
		}

		fseek(f_db, 0, SEEK_END);
		/*если повторение нету - записывем в файл .db структуру*/
		if( check_copy==0 )
		{
			num++;
			fwrite(record, sizeof(SCAN_INFO), 1, f_db);
		}
		check_copy=0;
	}

	free(record);
	fclose(f_db);
	fclose(f_csv);
	
	return 1;

}

void print_db(const char *db)
{
	FILE *f_db, *f_txt;
	SCAN_INFO *record;
	record=(SCAN_INFO*)malloc(sizeof(SCAN_INFO));

	f_db=fopen(db, "rb");
	f_txt=fopen("print_db.txt", "wt");

	if (f_db==NULL || record==NULL)
	{
		printf("Open err");
	}
	else
	{
		while (fread(record, sizeof(SCAN_INFO), 1, f_db)!=0)
		{
			printf("|  %d\t", record->id);
			printf("|  %s\t", record->manufacturer);
			printf("|  %d\t", record->year);
			printf("|  %s\t", record->model);
			printf("|  %.2f\t", record->price);
			printf("|  %d\t", record->x_size);
			printf("|  %d \t|\n", record->y_size);
			/*запись структуры в файл print_db.txt*/
			fprintf(f_txt,"%d;%s;%s;%d;%.2f;%d;%d\n",record->id, &record->manufacturer, &record->model, record->year, record->price, record->x_size, record->y_size);
		}
	}
	fclose(f_db);
	fclose(f_txt);
}

int reindex(const char *db)
{
	make_index("Data.db", "manufacturer");
	make_index("Data.db", "year");
	make_index("Data.db", "model");
	make_index("Data.db", "price");
	make_index("Data.db", "x_size");
	make_index("Data.db", "y_size");
}

int make_index(const char *db, const char *field_name)
{
	FILE *f_db, *f_idx;
	const char IDX[4]={'.','i','d','x'};
	char symb;
	char name[128];
	SCAN_INFO *buffer, **copy_place;
	int file_name, i=0, number=0;
	int exit_sort=0;
	file_name=strlen(field_name);
	f_db=fopen(db,"rb");
	buffer=(SCAN_INFO*)malloc(sizeof(SCAN_INFO));

	if (f_db == NULL || buffer == NULL)
	{
		printf("error\n");
		return 0;
	}
	/*записываем в строку '.idx'*/	
	strcpy(name,field_name);
	for(i; i<4; i++)
	{
		name[file_name+i]=IDX[i];
	}
	name[file_name+i]='\0';


	if ((f_idx = fopen(name, "w")) == NULL)
	{
		fclose(f_db);
		return 0;
	}
	/*считываем количество записей в .db*/
	while(fread(buffer,sizeof(SCAN_INFO),1,f_db)!=0)
	{
		number++;
	}
	free(buffer);
	/*************************************/
	copy_place=(SCAN_INFO**)malloc(number*sizeof(SCAN_INFO*));
	fseek(f_db,0,SEEK_SET);
	for (i = 0; i < number; i++)
	{
		if ((copy_place[i] = (SCAN_INFO*)malloc(sizeof(SCAN_INFO))) == NULL)
		{
			/*если не удалось выделить память - очищаем то, на что мы уже могли выделить*/
			for (i; i >= 0 ; i--)
			{
				free(copy_place[i]);
			}
			free(copy_place);
			fclose(f_db);
			fclose(f_idx);
		}
		fread(copy_place[i], sizeof(SCAN_INFO), 1, f_db);
	}
	buffer=(SCAN_INFO*)malloc(sizeof(SCAN_INFO));
	fseek(f_db,0,SEEK_SET);
	if (buffer == NULL)
	{
		printf("error\n");
		return 0;
	}
	
	if (field_name == "manufacturer")
	{
		printf("\n--- %s.idx content : \t", field_name);
		while(exit_sort!=1)
		{
			for (i=0; i<number-1; i++)
			{
				exit_sort=1;
				if (strcmp(copy_place[i]->manufacturer,copy_place[i+1]->manufacturer)==-1)
				{
					buffer=copy_place[i];
					copy_place[i]=copy_place[i+1];
					copy_place[i+1]=buffer;
					exit_sort=0;
				}
			}
		}
		for (i=0; i<number; i++)
		{
			exit_sort=copy_place[i]->id;
			fprintf(f_idx,"%d ",copy_place[i]->id);
			printf("%d ",copy_place[i]->id);
		}
		printf("\n");
	}
	else if (field_name == "year")
	{
		printf("\n--- %s.idx content :  \t", field_name);
		while(exit_sort!=1)
		{
			for (i=0; i<number-1; i++)
			{
				exit_sort=1;
				if (copy_place[i]->year>copy_place[i+1]->year)
				{
					buffer=copy_place[i];
					copy_place[i]=copy_place[i+1];
					copy_place[i+1]=buffer;
					exit_sort=0;
				}
			}
		}
		for (i=0; i<number; i++)
		{
			exit_sort=copy_place[i]->id;
			fprintf(f_idx,"%d ",copy_place[i]->id);
			printf("%d ",copy_place[i]->id);
		}
		printf("\n");
	}
	else if (field_name == "model")
	{
		printf("\n--- %s.idx content : \t", field_name);
		while(exit_sort!=1)
		{
			for (i=0; i<number-1; i++)
			{
				exit_sort=1;
				if (strcmp(copy_place[i]->manufacturer,copy_place[i+1]->manufacturer)==-1)
				{
					buffer=copy_place[i];
					copy_place[i]=copy_place[i+1];
					copy_place[i+1]=buffer;
					exit_sort=0;
				}
			}
		}
			for (i=0; i<number; i++)
			{
				exit_sort=copy_place[i]->id;
				fprintf(f_idx,"%d ",copy_place[i]->id);
				printf("%d ",copy_place[i]->id);
			}
			printf("\n");
	}
	else if (field_name == "price")
	{
		printf("\n--- %s.idx content : \t", field_name);
		while(exit_sort!=1)
		{
			for (i=0; i<number-1; i++)
			{
				exit_sort=1;
				if (copy_place[i]->price>copy_place[i+1]->price)
				{
					buffer=copy_place[i];
					copy_place[i]=copy_place[i+1];
					copy_place[i+1]=buffer;
					exit_sort=0;
				}
			}
		}
		for (i=0; i<number; i++)
		{
			exit_sort=copy_place[i]->id;
			fprintf(f_idx,"%d ",copy_place[i]->id);
			printf("%d ",copy_place[i]->id);
		}
		printf("\n");
	}
	else if (field_name == "x_size")
	{
		printf("\n--- %s.idx content : \t", field_name);
		while(exit_sort!=1)
		{
			for (i=0; i<number-1; i++)
			{
				exit_sort=1;
				if (copy_place[i]->x_size>copy_place[i+1]->x_size)
				{
					buffer=copy_place[i];
					copy_place[i]=copy_place[i+1];
					copy_place[i+1]=buffer;
					exit_sort=0;
				}
			}
		}
		for (i=0; i<number; i++)
		{
			exit_sort=copy_place[i]->id;
			fprintf(f_idx,"%d ",copy_place[i]->id);
			printf("%d ",copy_place[i]->id);
		}
		printf("\n");
	}
	else if (field_name == "y_size")
	{
		printf("\n--- %s.idx content : \t", field_name);
		while(exit_sort!=1)
		{
			for (i=0; i<number-1; i++)
			{
				exit_sort=1;
				if (copy_place[i]->y_size>copy_place[i+1]->y_size)
				{
					buffer=copy_place[i];
					copy_place[i]=copy_place[i+1];
					copy_place[i+1]=buffer;
					exit_sort=0;
				}
			}
		}
		for (i=0; i<number; i++)
		{
			exit_sort=copy_place[i]->id;
			fprintf(f_idx,"%d ",copy_place[i]->id);
			printf("%d ",copy_place[i]->id);
		}
		printf("\n");
	}
	free(copy_place);
	free(buffer);
	fclose(f_db);
	fclose(f_idx);

	return 1;
}

RECORD_SET * get_recs_by_index(const char *db , char *indx_field )
{
	RECORD_SET *record_set;
	FILE *f_db, *f_idx;
	SCAN_INFO *record;
	int i=0;
	char symb[2];

	printf("\n\n--- %s\n\n",indx_field);
	f_db = fopen(db, "r");
	f_idx = fopen(indx_field, "r");
	record_set = (RECORD_SET*)malloc(sizeof(RECORD_SET));
	record = (SCAN_INFO*)malloc(sizeof(SCAN_INFO));

	if (f_db == NULL || f_idx == NULL || record_set == NULL)
	{
		printf("open file error\n");
		fclose(f_db);
		fclose(f_idx);
		return NULL;
	}
	while(fread(&symb[0],sizeof(char),1,f_idx)!=0)
	{	
		fread(&symb[1],sizeof(char),1,f_idx);
		i++;
  	}
	record_set->recordS = (SCAN_INFO**)malloc(i*sizeof(SCAN_INFO*));
	if (record_set->recordS == NULL)
	{
		free(record_set);
		fclose(f_db);
		fclose(f_idx);
		return NULL;
	}
	fseek(f_idx,0,SEEK_SET);
	i=0;
	/*переписываем элементы из .db в нужном индексном порядке в структуру RECORD_SET*/
	while (fread(&symb,sizeof(symb),1,f_idx)!=0)
	{
		fseek(f_db,0,SEEK_SET);
		while(fread(record,sizeof(SCAN_INFO),1,f_db)!=0)
		{
			if(atoi(symb)==record->id)
			{
				record_set->recordS[i] = (SCAN_INFO*)malloc(sizeof(SCAN_INFO));
				fseek(f_db,record->id*sizeof(SCAN_INFO),SEEK_SET);
				fread(record_set->recordS[i],sizeof(SCAN_INFO),1,f_db);
				i++;
			}
		}
	}
	record_set->rec_nmb=i;
	fclose(f_db);
	fclose(f_idx);

	return record_set;
}

void print_rec_set(RECORD_SET *rs)
{
	int i=0;
	printf("--- rec_nmb  %d \n\n--- rs content : \n\n", rs->rec_nmb);
	for (i; i<rs->rec_nmb; i++)
	{
		printf("|  %d\t", rs->recordS[i]->id);
		printf("|  %s\t", rs->recordS[i]->manufacturer);
		printf("|  %d\t", rs->recordS[i]->year);
		printf("|  %s\t", rs->recordS[i]->model);
		printf("|  %.2f\t", rs->recordS[i]->price);
		printf("|  %d\t", rs->recordS[i]->x_size);
		printf("|  %d \t|\n", rs->recordS[i]->y_size);
		free(rs->recordS[i]);
	}
	free(rs->recordS);
}

void del_scanner(const char *db, int id)
{
	FILE *f_db;
	SCAN_INFO *buffer, **copy_place;
	int i=0, number=0, j=0;

	f_db=fopen(db,"r");
	buffer=(SCAN_INFO*)malloc(sizeof(SCAN_INFO));

	if (f_db == NULL || buffer == NULL)
	{
		printf("error\n");
		exit(1);
	}

	while(fread(buffer,sizeof(SCAN_INFO),1,f_db)!=0)
	{
		number++;
	}
	free(buffer);
	copy_place=(SCAN_INFO**)malloc(number*sizeof(SCAN_INFO*));
	fseek(f_db,0,SEEK_SET);
	/*переписываем элементы из файла .db в структуру */
	for (i = 0; i < number; i++)
	{

			if ((copy_place[i] = (SCAN_INFO*)malloc(sizeof(SCAN_INFO))) == NULL)
			{
				for (i; i >= 0 ; i--)
				{
					free(copy_place[i]);
				}
				free(copy_place);
				fclose(f_db);
			}
			fread(copy_place[i], sizeof(SCAN_INFO), 1, f_db);
		
	}
	fclose(f_db);
	f_db=fopen(db,"w+");
	/*переписываем элементы из структуры в файл .db за исключением того элемента, что нам нужно удалить из файла*/
	for (i=0; i<number; i++)
		{
			if(i!=id)
			{
				copy_place[i]->id=j;
				fwrite(copy_place[i],sizeof(SCAN_INFO),1,f_db);
			
				j++;
			}

		}
	fclose(f_db);
}

void add_scanner(const char *db, const char *scanner_str)
{
	FILE *f_db;
	SCAN_INFO *buffer, **copy_place, *record;
	int i=0, number=0, j=0, rec_c, count59=0;
	char mass[128], temp[128], symb;

	f_db=fopen(db,"r");
	buffer=(SCAN_INFO*)malloc(sizeof(SCAN_INFO));
	record=(SCAN_INFO*)malloc(sizeof(SCAN_INFO));
	strcpy(temp,scanner_str);
	temp[strlen(scanner_str)+1]='\0';
	if (f_db == NULL || buffer == NULL || record == NULL)
	{
		printf("error\n");
		exit(1);
	}

	while(fread(buffer,sizeof(SCAN_INFO),1,f_db)!=0)
	{
		number++;
	}

	symb=temp[0];
	/*переписываем данные из строки в структуру поэлементно*/
	while (symb!='\0')
	{
			symb=temp[j];
			if ((symb>=33 && symb<=90) || (symb>=97 && symb<=122))
			{
				if (symb==59)
				{
					count59++;
					mass[i]='\0';	//используется для ограничения размеров массива mass
				switch (count59)	//для удобства и избавления от кучи if'ов
				{
					case 1:
						for (rec_c=0; rec_c<i; rec_c++)//цикл для переписывания поэлементно данных типа char
						{
							buffer->manufacturer[rec_c]=mass[rec_c];
							buffer->manufacturer[rec_c+strlen(mass)]='\0';
						}
						i=0;
						break;
					case 2:
						for (rec_c=0; rec_c<i; rec_c++)
						{
							buffer->model[rec_c]=mass[rec_c];
							buffer->model[rec_c+strlen(mass)]='\0';
						}
						i=0;
						break;
					case 3:
						i=atoi(mass);
						buffer->year=i;
						i=0;
						break;
					case 4:
						buffer->price=atof(mass);//приведение строки к типу double ( но и к float'у подошло тоже )
						i=0;
						break;
					case 5:
						i=atoi(mass);
						buffer->x_size=i;//аналогично тому, что описано выше, только приводим к типу int
						i=0;
						break;
					case 6:
						i=atoi(mass);
						buffer->y_size=i;
						symb='\0';
						break;
				}
				}
				else
				{
					mass[i]=symb;
					i++;
				}
			j++;
			}
	}
	copy_place=(SCAN_INFO**)malloc(number*sizeof(SCAN_INFO*));
	fseek(f_db,0,SEEK_SET);
	/*проверка на наличие совпадения элемента из файла с элементом, который мы хотим добавить*/
	while (fread(record,sizeof(SCAN_INFO),1,f_db)!=0)
	{
		if(record->manufacturer==buffer->manufacturer && record->model==buffer->model && record->price==buffer->price && record->year==buffer->year)
		{
			printf("Element %s exist",scanner_str);
			fclose(f_db);
			free(record);
			free(buffer);
			free(copy_place);
			return 0;
		}
	}
	fseek(f_db,0,SEEK_SET);
	/*Запись элементов из файла в структуру*/
	for (i = 0; i < number; i++)
	{
		copy_place[i] = (SCAN_INFO*)malloc(sizeof(SCAN_INFO));
			if (copy_place[i] == NULL)
			{
				for (i; i >= 0 ; i--)
				{
					free(copy_place[i]);
				}
				free(copy_place);
				fclose(f_db);
			}
			fread(copy_place[i], sizeof(SCAN_INFO), 1, f_db);
		
	}
	fclose(f_db);
	f_db=fopen(db,"wb");
	/*запись структуры в файл .db*/
	for (i=0; i<number; i++)
	{
		fwrite(copy_place[i], sizeof(SCAN_INFO), 1, f_db);
	}
	/*присваиваем уникальный индекс*/
	buffer->id=number;
	/*дописываем строку в конец файла*/
	fwrite(buffer,sizeof(SCAN_INFO),1,f_db);
	fclose(f_db);
	free(buffer);			
	free(copy_place);
}

RECORD_SET* select(const char *db, const char *field, const char *value)
{
	FILE *f_db;
	RECORD_SET *record_set;
	SCAN_INFO *record;
	char elem[128], sec_elem[128];
	int i=0, j=0, number=0;
	float num1, num;

	f_db=fopen(db,"rb");
	record = (SCAN_INFO*)malloc(sizeof(SCAN_INFO));
	record_set = (RECORD_SET*)malloc(sizeof(SCAN_INFO*));
	strcpy(elem,value);
	elem[strlen(value)+1]='\0';
	if (f_db == NULL || record == NULL || record_set == NULL || record_set->recordS == NULL)
	{
		printf("\nOpen file error");
		fclose(f_db);
		return NULL;
	}
	while(fread(record,sizeof(SCAN_INFO),1,f_db)!=0)
	{
		number++;
	}
	fseek(f_db,0,SEEK_SET);
	record_set->rec_nmb=0;
	record_set->recordS = (SCAN_INFO**)malloc(number*sizeof(SCAN_INFO*));
	if (field == "manufacturer")
	{
		while(fread(record,sizeof(SCAN_INFO),1,f_db)!=0)
		{
			if (strcmp(record->manufacturer,value)==0)
			{
				record_set->recordS[i] = (SCAN_INFO*)malloc(sizeof(SCAN_INFO));
				record_set->rec_nmb+=1;
				fseek(f_db, j * sizeof(SCAN_INFO), SEEK_SET);
				fread(record_set->recordS[i], sizeof(SCAN_INFO), 1, f_db);
				i++;
			}
			j++;
		}
	}
	else if (field == "year")
	{
		while(fread(record,sizeof(SCAN_INFO),1,f_db)!=0)
		{
			if (record->year==atoi(elem))
			{
				record_set->recordS[i] = (SCAN_INFO*)malloc(sizeof(SCAN_INFO));
				record_set->rec_nmb+=1;
				fseek(f_db, j * sizeof(SCAN_INFO), SEEK_SET);
				fread(record_set->recordS[i], sizeof(SCAN_INFO), 1, f_db);
				i++;
			}
			j++;
		}
	}
	else if (field == "model")
	{
		while(fread(record,sizeof(SCAN_INFO),1,f_db)!=0)
		{
			
			if (strcmp(record->model,value)==0)
			{
				record_set->recordS[i] = (SCAN_INFO*)malloc(sizeof(SCAN_INFO));
				record_set->rec_nmb+=1;
				fseek(f_db, j * sizeof(SCAN_INFO), SEEK_SET);
				fread(record_set->recordS[i], sizeof(SCAN_INFO), 1, f_db);
				i++;
			}
			j++;
		}
	}
	else if (field == "price")
	{	
		while(fread(record,sizeof(SCAN_INFO),1,f_db)!=0)
		{
			num=record->price;
			num1=atof(elem)-record->price;
			if (num1<0.001 && num1>-0.001)
			{
				record_set->recordS[i] = (SCAN_INFO*)malloc(sizeof(SCAN_INFO));
				record_set->rec_nmb+=1;
				fseek(f_db, j * sizeof(SCAN_INFO), SEEK_SET);
				fread(record_set->recordS[i], sizeof(SCAN_INFO), 1, f_db);
				i++;
			}
			j++;
		}
	}
	else if (field == "x_size")
	{
		while(fread(record,sizeof(SCAN_INFO),1,f_db)!=0)
		{
			if (record->x_size==atoi(elem))
			{
				record_set->recordS[i] = (SCAN_INFO*)malloc(sizeof(SCAN_INFO));
				record_set->rec_nmb+=1;
				fseek(f_db, j * sizeof(SCAN_INFO), SEEK_SET);
				fread(record_set->recordS[i], sizeof(SCAN_INFO), 1, f_db);
				i++;
			}
			j++;
		}
	}
	else if (field == "y_size")
	{
		while(fread(record,sizeof(SCAN_INFO),1,f_db)!=0)
		{
			if (record->y_size==atoi(elem))
			{
				record_set->recordS[i] = (SCAN_INFO*)malloc(sizeof(SCAN_INFO));
				record_set->rec_nmb+=1;
				fseek(f_db, j * sizeof(SCAN_INFO), SEEK_SET);
				fread(record_set->recordS[i], sizeof(SCAN_INFO), 1, f_db);
				i++;
			}
			j++;
		}
	}
	for (i; i<record_set->rec_nmb; i++)
	{
		free(record_set->recordS[i]);
	}
	free(record);
	return record_set;
}