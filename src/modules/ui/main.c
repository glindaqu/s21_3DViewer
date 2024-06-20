#include <gtk/gtk.h>
#include "viewer-app.h"

int main(int argc, char **argv) {
    return g_application_run(G_APPLICATION(viewer_app_new()), argc, argv);
}
