#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{	
	int id;					//уникальный идентификатор
	char manufacturer[128];	// изготовитель
	int year;				// год изготовления
	char model[128];		// модель
	float price;			// цена
	int x_size;				// горизонтальная область сканирования
	int y_size;				// вертикальная область сканирования				
} SCAN_INFO;

typedef struct 
{
	int rec_nmb;			// число записей
	SCAN_INFO **recordS;	// записи
} RECORD_SET;


	int create_db(const char* csv, const char *db);
	void print_db(const char *db);
	int make_index(char *path_dba, char *field_name);
	int reindex(const char *db);
	RECORD_SET * get_recs_by_index(const char *db , char *indx_field );
	void del_scanner(const char *db, int id); 
	void add_scanner(const char *db, const char *scanner_str);
	void print_rec_set(RECORD_SET *rs);
	RECORD_SET* select(const char *db, const char *field, const char *value);
	
	

	
