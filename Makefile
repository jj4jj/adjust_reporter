
libadjust_report.a: ./adjust_report.cpp
	g++ -c adjust_report.cpp --std=c++11 -g
	ar r libadjust_report.a adjust_report.o

test: libadjust_report.a
	g++ -o test test.cpp -L./mmqueue  -ladjust_report -lmmqueue -L. -lcurl -pthread 






