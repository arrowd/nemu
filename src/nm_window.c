#include <nm_core.h>
#include <nm_ncurses.h>

#define NM_VM_MSG   "F1 - help, F10 - main menu "
#define NM_MAIN_MSG "Enter - select a choice, F10 - exit"

static void nm_print_window(const char *msg);

void nm_print_main_window(void)
{
    nm_print_window(_(NM_MAIN_MSG));
}

void nm_print_vm_window(void)
{
    nm_print_window(_(NM_VM_MSG));
}

void nm_print_warn(nm_window_t *w, const char *msg)
{
    box(w, 0, 0);
    mvwprintw(w, 1, 1, "%s", msg);
    wrefresh(w);
    wgetch(w);
}

void nm_print_help(nm_window_t *w)
{
    const char *msg[] = {
          "             nEMU v" NM_VERSION,
          "",
        _(" r - start guest"),
        _(" t - start guest in temporary mode"),
#if (NM_WITH_VNC_CLIENT)
        _(" c - connect to guest via vnc"),
#endif
        _(" f - force stop guest"),
        _(" d - delete guest"),
        _(" e - edit guest settings"),
        _(" i - edit network settings"),
        _(" a - add virtual disk"),
        _(" l - clone guest"),
        _(" s - edit boot settings"),
        _(" m - show command"),
        _(" u - delete unused tap interfaces")
    };

    box(w, 0, 0);

    for (size_t n = 0, y = 1; n < nm_arr_len(msg); n++, y++)
        mvwprintw(w, y, 1, "%s", msg[n]);

    wgetch(w);
}

void nm_print_nemu(void)
{
    const char *nemu[] = {
        "            .x@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@o.           ",
        "          .x@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@x.          ",
        "         o@@@@@@@@@@@@@x@@@@@@@@@@@@@@@@@@@@@@@@@o         ",
        "       .x@@@@@@@@x@@@@xx@@@@@@@@@@@@@@@@@@@@@@@@@@o        ",
        "      .x@@@@@@@x. .x@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@.       ",
        "      x@@@@@ox@x..o@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@.      ",
        "     o@@@@@@xx@@@@@@@@@@@@@@@@@@@@@@@@x@@@@@@@@@@@@@o      ",
        "    .x@@@@@@@@@@x@@@@@@@@@@@@@@@@@@@@@o@@@@@@@@@@@@@@.     ",
        "    .@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@x.@@@@@@@@@@@@@@o     ",
        "    o@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@xx..o@@@@@@@@@@@@@x     ",
        "    x@@@@@@@@@@@@@@@@@@xxoooooooo........oxxx@@@@@@@@x.    ",
        "    x@@@@@@@@@@x@@@@@@x. .oxxx@@xo.       .oxo.o@@@@@x     ",
        "    o@@@@@@@@x..o@@@@@o .xxx@@@o ..      .x@ooox@@@@@o     ",
        "    .x@@@@@@@oo..x@@@@.  ...oo..         .oo. o@@@@@o.     ",
        "     .x@@@@@@xoooo@@@@.                       x@@@x..      ",
        "       o@@@@@@ooxx@@@@.                 .    .@@@x..       ",
        "        .o@@@@@@o.x@@@.                 ..   o@@x.         ",
        "         .x@@@@@@xx@@@.                 ..  .x@o           ",
        "         o@@@@@@@o.x@@.                    .x@x.           ",
        "         o@@@@@@@o o@@@xo.         .....  .x@x.            ",
        "          .@@@@@x. .x@o.x@x..           .o@@@.             ",
        "         .x@@@@@o.. o@o  .o@@xoo..    .o..@@o              ",
        "          o@@@@oo@@xx@x.   .x@@@ooooooo. o@x.              ",
        "         .o@@o..xx@@@@@xo.. o@x.         xx.               ",
        "          .oo. ....ox@@@@@@xx@o          xo.               ",
        "          ....       .o@@@@@@@@o        .@..               ",
        "           ...         ox.ox@@@.        .x..               ",
        "            ...        .x. .@@x.        .o                 ",
        "              .         .o .@@o.         o                 ",
        "                         o. x@@o         .                 ",
        "                         .o o@@..        .                 ",
        "                          ...@o..        .                 ",
        "                           .x@xo.                          ",
        "                            .x..                           ",
        "                           .x....                          ",
        "                           o@..xo                          ",
        "                           o@o oo                          ",
        "                           ..                              "
    };

    nm_clear_screen();

    for (size_t n = 0; n < nm_arr_len(nemu); n++)
        mvprintw(n, 0, "%s", nemu[n]);

    getch();
}

static void nm_print_window(const char *msg)
{
    int col;
    size_t msg_len;

    col = getmaxx(stdscr);
    msg_len = mbstowcs(NULL, msg, strlen(msg));

    nm_clear_screen();
    border(0,0,0,0,0,0,0,0);
    mvprintw(1, (col - msg_len) / 2, msg);
    refresh();
}

/* vim:set ts=4 sw=4 fdm=marker: */