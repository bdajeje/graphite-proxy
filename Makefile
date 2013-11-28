all: bin

libs:
	$(MAKE) -C src/ libs

bin: libs
	$(MAKE) -C src/ bin

clean:
	$(MAKE) -C src clean

cleaner:
	$(MAKE) -C src cleaner
	$(MAKE) -C tests cleaner

docs:
	$(MAKE) -C src docs

checks:
	$(MAKE) -C src checks

tests: all
	$(MAKE) -C tests tests

unit_tests:
	$(MAKE) -C tests unit_tests

integration_tests:
	$(MAKE) -C tests integration_tests

.PHONY: docs
