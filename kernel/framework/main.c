#include "../test/test.h"
#include <kernel.h>
#include <klib.h>

int main()
{
  ioe_init();
  // cte_init(os->trap);
  os->init();
  // mpe_init(os->run);
  mpe_init(pmm_test);
  return 1;
}
