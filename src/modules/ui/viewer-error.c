#include "viewer-error.h"

GQuark viewer_error_quark(void) {
  return g_quark_from_static_string("viewer-error");
}
