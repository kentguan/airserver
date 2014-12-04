src = $(wildcard *.cpp)
dir = $(notdir $(src))
obj = $(patsubst %.cpp,%.o,$(dir))
deps = $(patsubst %.cpp,%.d,$(dir))
LIBS = -ldl -lpthread -lboost_system -ggdb -rdynamic
CFLAGS = -ggdb -O2

all: $(obj) $(deps)
	g++ -o air_server $(obj) $(CFLAGS) $(LIBS) 
	cp air_server ./sample/

$(deps): %.d: %.cpp
	rm -f $@
	g++ -MM $< -ggdb >> $@ 2> /dev/null

sinclude $(deps)

$(obj): %.o: %.cpp
	g++ $< -c -o $@ -g

clean:
	-rm -rf *.d
	-rm -rf *.o
	-rm air_server
