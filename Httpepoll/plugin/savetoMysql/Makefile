savetoMysqld:savetoMysql.cpp my_sql.cpp
	g++ -o $@ $^ -I/usr/include/mysql -L/usr/lib/mysql -lhiredis -lmysqlclient
#	g++ -o $@ $^  -lhiredis 
.PHONY:clean
clean:
	rm -rf savetoMysqld
