with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "GTK-C-SHELL";
  buildInputs = with pkgs; [
    gcc
    gnumake
    check
    pkg-config
    clang-tools
    gtk4
    gtk3
    glib
    glibc
    libepoxy
    valgrind
    python3
  ];
}
