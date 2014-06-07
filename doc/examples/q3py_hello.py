import q3py
import ctypes


# Game module initialization function.
# defined in ioq3/code/game/g_public.h
GAME_INIT = 0

# Engine system trap to print errors from game module.
# Defined in ioq3/code/game/g_public.h
G_ERROR = 1


# Compare with trap_Error() from ioq3/code/game/g_syscalls.c
def qerror(msg):
    c_msg = ctypes.create_string_buffer(msg)

    return q3py.syscall(G_ERROR, ctypes.addressof(c_msg))


# Compare with G_InitGame() from ioq3/code/game/g_main.c
def init_game(level_time, random_seed, restart):
    print("Python init_game(level_time={level_time}, "
          "random_seed={random_seed}, "
          "restart={restart})".format(level_time=level_time,
                                    random_seed=random_seed,
                                    restart=restart))
    qerror(b"Hello, Quake 3!")


# Compare with vmMain() from ioq3/code/game/g_main.c
def vm_main(cmd, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8,
            arg9, arg10, arg11):

    if (cmd == GAME_INIT):
        init_game(arg0, arg1, bool(arg2))

    return -1


# Related to dllEntry() in ioq3/code/game/g_syscalls.c
def dll_entry():
    print("Python dll_entry() called")

    return vm_main
