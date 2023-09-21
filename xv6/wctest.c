//Tests write count functionality
#include "types.h"
#include "user.h"
#include "stat.h"

int
main(int argc, char *argv[])
{
   
    printf(1, "Initial write count: %d\n", writecount());

    printf(1, "---Setting write count to 150---\n");
    setwritecount(150);
    printf(1, "After setting to 150: %d\n", writecount());
    printf(1, "printf() should increase writecount: %d\n", writecount());

    printf(1, "---Resetting to 0---\n");
    setwritecount(0);
    printf(1, "After resetting: %d\n", writecount());


    exit();
}