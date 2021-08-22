.PHONY: debug release clean debug_headless test lib/board.a gui/main

debug : BUILD_TYPE := debug
debug: gui/main tui/main test

release : BUILD_TYPE := release
release: gui/main tui/main

clean : BUILD_TYPE := clean
clean: lib/board.a gui/main tui/main
	$(MAKE) -C test clean


lib/board.a:
	$(MAKE) -C lib $(BUILD_TYPE)

gui/main: lib/board.a
	$(MAKE) -C gui $(BUILD_TYPE)

tui/main: lib/board.a
	$(MAKE) -C tui $(BUILD_TYPE)


debug_headless:
	$(MAKE) -C lib debug
	$(MAKE) -C test
	test/main > /dev/null

test:
	$(MAKE) -C test
	test/main > /dev/null
