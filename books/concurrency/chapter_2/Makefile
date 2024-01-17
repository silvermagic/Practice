src = $(wildcard *.cc)
obj = $(src:.c=.o)

LDFLAGS = -lpthread

test: $(obj)
	$(CXX) -std=c++14 -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) test