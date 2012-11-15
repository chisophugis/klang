
compile:
	@for dir in $(SUBDIRS); do \
		make -C $$dir || exit $?; \
		done

clean:
	@for dir in $(SUBDIRS); do \
		make -C $$dir clean; \
		done
	rm -rf *.o *.i *.s $(TARGET)


