.PHONY: debug release clean debug_headless

debug : BUILD_TYPE := debug
debug: lib_gui
	$(MAKE) -C test
	test/main > /dev/null

release : BUILD_TYPE := release
release: lib_gui

clean : BUILD_TYPE := clean
clean: lib_gui
	$(MAKE) -C test clean

lib_gui:
	$(MAKE) -C lib $(BUILD_TYPE)
	$(MAKE) -C gui $(BUILD_TYPE)

debug_headless:
	$(MAKE) -C lib debug
	$(MAKE) -C test
	test/main > /dev/null
