src = $(wildcard *.cc)
obj = $(src:.c=.o)

LDFLAGS = -lpthread

test: $(obj)
	$(CXX) -std=c++2a -fconcepts -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) test
