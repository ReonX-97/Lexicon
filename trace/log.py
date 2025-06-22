import gdb
import subprocess
import os

LOG_FILE = "trace.log"

class Logger(gdb.Breakpoint):
    def __init__(self, funcname):
        super().__init__(funcname, gdb.BP_BREAKPOINT, internal=False)
        self.funcname = funcname

    def stop(self):
        with open(LOG_FILE, "a") as f:
            f.write(f"[CALL] {self.funcname}\n")
        return False  # Don't stop execution

def extract_function_names_from_nm(binary_path):
    try:
        output = subprocess.check_output(['nm', binary_path], text=True)
        functions = []

        for line in output.splitlines():
            parts = line.strip().split()
            if len(parts) == 3 and parts[1] == 'T':  # 'T' = global text (i.e., function)
                name = parts[2]
                if not (name.startswith("_") or name.startswith("std")):
                    functions.append(name)

        return functions

    except subprocess.CalledProcessError as e:
        print(f"[!] Failed to run nm: {e}")
        return []

def setup_tracer():
    print(f"[*] Logging all function calls to {LOG_FILE}...")

    binary_path = gdb.current_progspace().filename
    if not binary_path or not os.path.exists(binary_path):
        print("[!] No valid binary loaded in GDB.")
        return

    # Clear previous log
    open(LOG_FILE, "w").close()

    function_names = extract_function_names_from_nm(binary_path)
    count = 0

    for func in function_names:
        try:
            Logger(func)
            count += 1
        except Exception:
            continue

    print(f"[*] Attached to {count} functions.")

setup_tracer()
