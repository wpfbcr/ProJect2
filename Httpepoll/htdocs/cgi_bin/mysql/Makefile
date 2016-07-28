all:insert_mysql select_mysql delete_mysql modify_mysql
insert_mysql:my_sql.cpp insert.cpp 
	g++ -o $@ $^ -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient
select_mysql:my_sql.cpp select.cpp 
	g++ -o $@ $^ -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient
delete_mysql:my_sql.cpp delete.cpp 
	g++ -o $@ $^ -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient
modify_mysql:my_sql.cpp modify.cpp 
	g++ -o $@ $^ -I/usr/include/mysql -L/usr/lib/mysql -lmysqlclient
.PHONY:clean
clean:
	rm -rf insert_mysql select_mysql delete_mysql modify_mysql
