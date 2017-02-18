#include <iostream>
#include <cstdlib>
#include <string>
#include <array>
#include <vector>
#include <memory>
#include <libintl.h>

#include "qemu_manage.h"
#include "qm_windows.h"
#include "guest.h"

using namespace QManager;

int main(void)
{
    init_cfg();
    const struct config *cfg = get_cfg();

    // localization
    char usr_path[1024] = {0};
    snprintf(usr_path, sizeof(usr_path), "%s%s", STRING(USR_PREFIX), "/share/locale");

    setlocale(LC_ALL,"");
    bindtextdomain("qemu-manage", usr_path);
    textdomain("qemu-manage");

    const std::array<std::string, 3> choices = {
        _("Manage guests"),
        _("Install guest"),
        _("Add guest"),
    };

    uint32_t highlight = 1;
    uint32_t ch;

    initscr();
    raw();
    noecho();
    curs_set(0);

    std::unique_ptr<QMWindow> main_window(new MainWindow(10, 30));
    main_window->Init();

    for (;;)
    {
        uint32_t choice = 0;

        main_window->Print();
        std::unique_ptr<MenuList> main_menu(new MenuList(main_window->window, highlight));
        main_menu->Print(choices.begin(), choices.end());

        while ((ch = wgetch(main_window->window)))
        {
            switch (ch) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = choices.size();
                else
                    highlight--;
                break;

            case KEY_DOWN:
                if (highlight == choices.size())
                    highlight = 1;
                else
                    highlight++;
                break;

            case 10:
                choice = highlight;
                break;

            case KEY_F(10):
                clear();
                refresh();
                endwin();
                exit(0);
            }

            std::unique_ptr<MenuList> main_menu(new MenuList(main_window->window, highlight));
            main_menu->Print(choices.begin(), choices.end());

            if (choice != 0)
                break;
        }

        if (choice == MenuVmlist)
        {
            const std::string sql_list_vm = "select name from vms order by name asc";

            std::unique_ptr<QemuDb> db(new QemuDb(cfg->db));
            VectorString guests = db->SelectQuery(sql_list_vm);

            if (guests.empty())
            {
                std::unique_ptr<PopupWarning> Warn(new PopupWarning(_("No guests here."), 3, 20, 7));
                Warn->Init();
                Warn->Print(Warn->window);
            }
            else
            {
                uint32_t listmax = cfg->list_max;
                uint32_t guest_first = 0;
                uint32_t guest_last;
                uint32_t q_highlight = 1;

                if (listmax > guests.size())
                    listmax = guests.size();

                guest_last = listmax;

                clear();
                std::unique_ptr<QMWindow> vm_window(new VmWindow(listmax + 4, 32));
                vm_window->Init();
                vm_window->Print();


                std::unique_ptr<VmList> vm_list(new VmList(vm_window->window, q_highlight, cfg->vmdir));
                vm_list->Print(guests.begin() + guest_first, guests.begin() + guest_last);

                wtimeout(vm_window->window, 1000);

                for (;;)
                {
                    ch = wgetch(vm_window->window);

                    if ((ch == KEY_UP) && (q_highlight == 1) &&
                        (guest_first == 0) && (listmax < guests.size()))
                    {
                        q_highlight = listmax;
                        guest_first = guests.size() - listmax;
                        guest_last = guests.size();
                    }

                    else if (ch == KEY_UP)
                    {
                        if ((q_highlight == 1) && (guests.size() <= listmax))
                            q_highlight = guests.size();
                        else if ((q_highlight == 1) && (guest_first != 0))
                        {
                            guest_first--;
                            guest_last--;
                        }
                        else
                            q_highlight--;
                    }

                    else if ((ch == KEY_DOWN) && (q_highlight == listmax) &&
                             (guest_last == guests.size()))
                    {
                        q_highlight = 1;
                        guest_first = 0;
                        guest_last = listmax;
                    }

                    else if (ch == KEY_DOWN)
                    {
                        if ((q_highlight == guests.size()) && (guests.size() <= listmax))
                            q_highlight = 1;
                        else if ((q_highlight == listmax) && (guest_last < guests.size()))
                        {
                            guest_first++;
                            guest_last++;
                        }
                        else
                            q_highlight++;
                    }

                    else if (ch == MenuKeyEnter)
                    {
                        std::string guest = guests.at((guest_first + q_highlight) - 1);
                        std::unique_ptr<QMWindow> vminfo_window(new VmInfoWindow(guest, cfg->db, 10, 30));
                        vminfo_window->Print();
                    }

                    else if (ch == MenuKeyR)
                    {
                        std::unique_ptr<VmList> vm_list(new VmList(vm_window->window, q_highlight, cfg->vmdir));
                        vm_list->Print(guests.begin() + guest_first, guests.begin() + guest_last);

                        std::string guest = guests.at((guest_first + q_highlight) - 1);

                        if (vm_list->vm_status.at(guest) == "running")
                        {
                            std::unique_ptr<PopupWarning> Warn(new PopupWarning(_("Already running!"), 3, 20, 7));
                            Warn->Init();
                            Warn->Print(Warn->window);
                        }
                        else
                        {
                            if (!check_root())
                            {
                                std::unique_ptr<PopupWarning> Warn(new PopupWarning(_("Must be root."), 3, 20, 7));
                                Warn->Init();
                                Warn->Print(Warn->window);
                            }
                            else
                                start_guest(guest, cfg->db, cfg->vmdir);
                        }
                    }

                    else if (ch == MenuKeyC)
                    {
                        std::unique_ptr<VmList> vm_list(new VmList(vm_window->window, q_highlight, cfg->vmdir));
                        vm_list->Print(guests.begin() + guest_first, guests.begin() + guest_last);

                        std::string guest = guests.at((guest_first + q_highlight) - 1);

                        if (vm_list->vm_status.at(guest) == "running")
                            connect_guest(guest, cfg->db);
                    }

                    else if (ch == MenuKeyD)
                    {
                        std::unique_ptr<VmList> vm_list(new VmList(vm_window->window, q_highlight, cfg->vmdir));
                        vm_list->Print(guests.begin() + guest_first, guests.begin() + guest_last);

                        std::string guest = guests.at((guest_first + q_highlight) - 1);

                        if (vm_list->vm_status.at(guest) == "running")
                        {
                            std::unique_ptr<PopupWarning> Warn(new PopupWarning(_("Stop guest before!"), 3, 20, 7));
                            Warn->Init();
                            Warn->Print(Warn->window);
                        }
                        else
                        {
                            std::unique_ptr<PopupWarning> Warn(new PopupWarning(_("Proceed? (y/n)"), 3, 20, 7));
                            Warn->Init();
                            uint32_t ch = Warn->Print(Warn->window);

                            if (ch == MenuKeyY)
                            {
                                delete_guest(guest, cfg->db, cfg->vmdir);
                                // Exit from loop to reread guests
                                break;
                            }
                        }
                    }

                    else if (ch == MenuKeyF)
                    {
                        std::unique_ptr<VmList> vm_list(new VmList(vm_window->window, q_highlight, cfg->vmdir));
                        vm_list->Print(guests.begin() + guest_first, guests.begin() + guest_last);

                        std::string guest = guests.at((guest_first + q_highlight) - 1);

                        if (vm_list->vm_status.at(guest) == "running")
                            kill_guest(guest);
                    }
     
                    else if (ch == MenuKeyE)
                    {
                        std::string guest = guests.at((guest_first + q_highlight) - 1);

                        std::unique_ptr<QMFormWindow> edit_window(new EditVmWindow(cfg->db, cfg->vmdir, guest, 19, 60));
                        edit_window->Init();
                        edit_window->Print();
                    }

                    else if (ch == MenuKeyA)
                    {
                        std::string guest = guests.at((guest_first + q_highlight) - 1);

                        std::unique_ptr<QMFormWindow> add_disk_window(new AddDiskWindow(cfg->db, cfg->vmdir, guest, 7, 35));
                        add_disk_window->Init();
                        add_disk_window->Print();
                    }

                    else if (ch == MenuKeyI)
                    {
                        std::string guest = guests.at((guest_first + q_highlight) - 1);

                        std::unique_ptr<QMFormWindow> edit_net_window(new EditNetWindow(cfg->db, cfg->vmdir, guest, 9, 39));
                        edit_net_window->Init();
                        edit_net_window->Print();
                    }

                    else if (ch == MenuKeyS)
                    {
                        const std::string guest = guests.at((guest_first + q_highlight) - 1);

                        std::unique_ptr<QMFormWindow> edit_install_window(new EditInstallWindow(cfg->db, cfg->vmdir, guest, 7, 60));
                        edit_install_window->Init();
                        edit_install_window->Print();
                    }

                    else if (ch == MenuKeyL)
                    {
                        std::unique_ptr<VmList> vm_list(new VmList(vm_window->window, q_highlight, cfg->vmdir));
                        vm_list->Print(guests.begin() + guest_first, guests.begin() + guest_last);

                        std::string guest = guests.at((guest_first + q_highlight) - 1);

                        if (vm_list->vm_status.at(guest) == "running")
                        {
                            std::unique_ptr<PopupWarning> Warn(new PopupWarning(_("Stop guest before!"), 3, 20, 7));
                            Warn->Init();
                            Warn->Print(Warn->window);
                        }
                        else
                        {
                            std::unique_ptr<QMFormWindow> clone_window(new CloneVmWindow(cfg->db, cfg->vmdir, guest, 7, 35));
                            clone_window->Init();
                            clone_window->Print();

                            // Exit from loop to reread guests
                            break;
                        }
                    }

                    else if (ch == KEY_F(1))
                    {
                        std::unique_ptr<QMWindow> help_window(new HelpWindow(13, 40));
                        help_window->Init();
                        help_window->Print();
                    }

                    else if (ch == KEY_F(10))
                    {
                        refresh();
                        endwin();
                        break;
                    }

                    vm_window->Print();
                    std::unique_ptr<VmList> vm_list(new VmList(vm_window->window, q_highlight, cfg->vmdir));
                    vm_list->Print(guests.begin() + guest_first, guests.begin() + guest_last);
                }
            }
        }
        else if (choice == MenuAddVm)
        {
            std::unique_ptr<QMFormWindow> add_window(new AddVmWindow(cfg->db, cfg->vmdir, 23, 60));
            add_window->Init();
            add_window->Print();
        }
    }

    return 0;
}
