#include <kernel.h>
#include <klib.h>

void pmm_test();

int main()
{
  ioe_init();
  // cte_init(os->trap);
  os->init();
  // mpe_init(os->run);
  mpe_init(pmm_test);
  return 1;
}
