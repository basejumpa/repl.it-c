TEST_SUFFIX := .test
TEST_DEFINE := __TEST__
## TEST-guarded compile-step:

%.o: %.c
	@if head -n 10 $(<) | egrep --quiet "include +<|\" *_test.h *>|\"" ; then \
	   if [ -e $(<)$(TEST_SUFFIX) ]; then mv $(<)$(TEST_SUFFIX) $(<)$(TEST_SUFFIX).old ; fi; \
	   if [ -e $(subst .,_,$(<)$(TEST_SUFFIX)).exe ]; then rm -f $(subst .,_,$(<)$(TEST_SUFFIX)).exe ; fi; \
	   echo gcc -g -O0 --coverage -pg -I. -D$(TEST_DEFINE) -o $(subst .,_,$(<)$(TEST_SUFFIX)).exe $(<) ;\
	   ( gcc -g -O0 --coverage -pg -I. -D$(TEST_DEFINE) -o $(subst .,_,$(<)$(TEST_SUFFIX)).exe $(<) 2>&1 | tee -a $(<)$(TEST_SUFFIX) ; exit $${PIPESTATUS[0]} ) ;\
	   if [ 0 == $$? ] ; then (\
	      gdb --batch --eval-command run ./$(subst .,_,$(<)$(TEST_SUFFIX)).exe 2>&1 | sed -e 's/\(.*\) at \(.*\):\(.*\)/\2:\3:1: error: \1/' | tee $(<)$(TEST_SUFFIX) ; \
	      if tail -n 1 $(<)$(TEST_SUFFIX) | grep --quiet "exited normally"; then (\
	         rm -f ./$(subst .,_,$(<)$(TEST_SUFFIX)).exe-deact ;\
	         echo gcc -c -MMD -g -I. -o $(@) $(<) ; \
	         gcc -c -MMD -g -I. -U$(TEST_DEFINE) -o $(@) $(<) ; \
	      gcov -a -f -c $(@) \
	         | grep -v -E "(^No)|(^$$)" \
	         | tr -d '\r\n' \
	         | sed 's/Function/\r\nFunction/g; s/File/\r\nFile/g;' \
	         | sed 's/\([0-9]\)\([a-zA-Z]\)/\1 \2/g' \
	         | sed '$$d' \
	         | grep -v -E "(__test')|(_test_entry')|('main')" \
	         | grep -v "100.00%" \
	         | tee -a $(<)$(TEST_SUFFIX) ;\
	      gprof $(subst .,_,$(<)$(TEST_SUFFIX)).exe gmon.out > $(subst .,_,$(<)).gprof ; \
	      );\
	      else \
	      cat $(<)$(TEST_SUFFIX) | grep -E "^(.+):[0-9]+:[0-9]+: error:" | head -1 | awk -F":" '{ printf "%s %s",$$1,$$2; }' | xargs java -jar eclipse_remote_control_client_1.3.0.jar open_file ; \
	      fi;\
	      sed --in-place -r -e 's/New Thread [0-9a-fx.]+/New Thread xxxxxxxx/;  s/(process [0-9]+)/(process xxxxxxxx)/; s/error/alex/;' $(<)$(TEST_SUFFIX) 2>/dev/null ;\
	      rm -f ./$(basename $(@)).gcda ./$(basename $(@)).gcno gmon.out ; \
	   );\
	   else \
	      if cat $(<)$(TEST_SUFFIX) | grep -E "^(.+):[0-9]+:[0-9]+: error:" ; then cat $(<)$(TEST_SUFFIX) | grep -E "(.+):[0-9]+:[0-9]+: error:" | head -1 | awk -F":" '{ printf "%s %s",$$1,$$2; }' | xargs java -jar eclipse_remote_control_client_1.3.0.jar open_file ; fi;\
	   fi; \
	else \
	   echo gcc -c -MMD -g -I. -o $(@) $(<) ; \
	   gcc -c -MMD -g -I. -U$(TEST_DEFINE) -o $(@) $(<) ; \
	fi;

.PHONY: clean_local
clean_local:
	rm -f *.o *.gcov *.test *.test.old *.gcno *.gcda *.d gmon.out *.gprof
