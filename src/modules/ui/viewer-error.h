#ifndef _VIEWER_ERROR_H_
#define _VIEWER_ERROR_H_

#include <glib.h>

G_BEGIN_DECLS

#define VIEWER_ERROR (viewer_error_quark())

typedef enum {
  VIEWER_ERROR_SHADER_COMPILATION,
  VIEWER_ERROR_SHADER_LINK
} ViewerError;

GQuark viewer_error_quark(void);

G_END_DECLS

#endif /* _VIEWER_ERROR_H_ */
