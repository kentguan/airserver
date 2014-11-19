src = $(wildcard *.cpp)
dir = $(notdir $(src))
obj = $(patsubst %.cpp,%.o,$(dir))
deps = $(patsubst %.cpp,%.d,$(dir))

all: $(obj) $(deps)
	g++ -o air_server $(obj) -lpthread -lboost_system

$(deps): %.d: %.cpp
	@rm -f $@
	@g++ -MM $< >> $@ 2> /dev/null

sinclude $(deps)

clean:
	-rm -rf *.d
	-rm -rf *.o
	-rm air_server
