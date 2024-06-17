#include <gtk/gtk.h>

static void
activate (GtkApplication* app)
{
  GtkWidget *window;  
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "3DViewer_V1.0");
  gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
  gtk_window_present (GTK_WINDOW (window));
}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("School21.gdy.S21_3DViewer-V1_0", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
