#include "../termkey.h"
#include "taplib.h"

int main(int argc, char *argv[])
{
  TermKey   *tk;
  TermKeyKey key;
  long       args[16];
  size_t     nargs = 16;
  unsigned long command;

  plan_tests(7);

  tk = termkey_new_abstract("vt100", 0);

  termkey_push_bytes(tk, "\e[5;25v", 7);

  is_int(termkey_getkey(tk, &key), TERMKEY_RES_KEY, "getkey yields RES_KEY for position report");

  is_int(key.type, TERMKEY_TYPE_UNKNOWN_CSI, "key.type for unknown CSI");

  is_int(termkey_interpret_csi(tk, &key, args, &nargs, &command), TERMKEY_RES_KEY, "interpret_csi yields RES_KEY");

  is_int(nargs,     2, "nargs for unknown CSI");
  is_int(args[0],   5, "args[0] for unknown CSI");
  is_int(args[1],  25, "args[1] for unknown CSI");
  is_int(command, 'v', "command for unknown CSI");

  termkey_destroy(tk);

  return exit_status();
}
