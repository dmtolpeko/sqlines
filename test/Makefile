TESTS = \
	informix/returning.sql


check: $(TESTS)
	@PATH=../sqlines:${PATH} \
	./runtests.sh $(TESTS)
