tests: unit_tests integration_tests

build_unit_tests:
	$(MAKE) -C units/

unit_tests: build_unit_tests
	$(MAKE) -C units/ run

integration_tests:
	$(MAKE) -C integration/ run

clean:
	$(MAKE) -C units/ clean

cleaner:
	$(MAKE) -C units/ cleaner