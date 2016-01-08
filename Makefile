release/libadjust_report.a: ./adjust_report.cpp
	mkdir -p release
	cd release && g++ -c ../adjust_report.cpp --std=c++11 -O2 -I../mmqueue
	cd release && g++ -c ../mmqueue/mmqueue.cpp ../mmqueue/base/*.cpp -O2 -I../mmqueue --std=c++11
	cd release && ar r libadjust_report.a adjust_report.o

debug/libadjust_report_d.a: ./adjust_report.cpp
	mkdir -p debug
	cd debug && g++ -c ../adjust_report.cpp --std=c++11 -g -Wall -I../mmqueue
	cd debug && g++ -c ../mmqueue/mmqueue.cpp ../mmqueue/base/*.cpp -g -I../mmqueue --std=c++11
	cd debug && ar r libadjust_report_d.a *.o

test: debug/libadjust_report_d.a
	g++ -o test test.cpp -ladjust_report_d  -Ldebug -lcurl -pthread -g

install: release/libadjust_report.a debug/libadjust_report_d.a
	mkdir -p adjust_report/include
	mkdir -p adjust_report/lib
	cp ./adjust_report.h adjust_report/include
	cp ./release/libadjust_report.a adjust_report/lib
	cp ./debug/libadjust_report_d.a adjust_report/lib

clean:
	rm -f *.o
	rm -f *.a
	rm -f test
	rm -f debug/*.o release/*.o






