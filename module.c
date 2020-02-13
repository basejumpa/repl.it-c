#ifdef __TEST__
#include "test/_test.h"
#endif /* __TEST__ */

int y(int x);

#ifdef __TEST__
test_def(y){
   ok(y(0) == 0, "");
   ok(y(1) == 1, "");
   ok(y(2) == 4, "");
}
#endif /* __TEST__ */

int y(int x){
  return x*x;
}

#ifdef __TEST__
test_def(staple)
{
   test_call(y);
} /* staple */

test_entry(staple);
test_main(); /**< Comment this out if your c-file already defines a main function */
#endif /* __TEST__ */
