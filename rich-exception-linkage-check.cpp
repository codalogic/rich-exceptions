
// Just check whether including header in multiple files doesn't cause linkage problems.

#include "rich-exception.h"

#define ANNOTATE_LITE_PROTOTYPES_ONLY
#include "annotate-lite.h"

void show_multiple_linkage_ok()
{
}
