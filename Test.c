#include "Lab2.h"

int main() {
	
	RECORD_SET *rs;

	if(create_db("Text.csv", "Data.db")==1)
	print_db("Data.db");
	reindex("Data.db");

	del_scanner("Data.db", 3);
	print_db("Data.db");
	reindex("Data.db");

	add_scanner("Data.db","ASUS;AsScan8;2009;99.99;210;297");
	print_db("Data.db");
	reindex("Data.db");

	rs = get_recs_by_index("Data.db", "manufacturer.idx");
	print_rec_set(rs);

	rs = get_recs_by_index("Data.db", "model.idx");
	print_rec_set(rs);

	rs = get_recs_by_index("Data.db", "price.idx");
	print_rec_set(rs);

	rs = select("Data.db", "manufacturer", "HP");
	print_rec_set(rs);

	rs = select("Data.db", "model", "DellScan1");
	print_rec_set(rs);

	rs = select("Data.db", "year", "2007");
	print_rec_set(rs);

	rs = select("Data.db", "price", "19.99"); 
	print_rec_set(rs);

	rs = select("Data.db", "x_size", "210");
	print_rec_set(rs);

	rs = select("Data.db", "y_size", "297");
	print_rec_set(rs);

	/*free(rs);*/
	system("pause");
	return 0;
}
