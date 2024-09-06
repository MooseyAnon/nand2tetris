"""File buffer."""

OUT_FILE = None
MAX_BUFFER_LENGTH = 2
BUFFER = []


def init(fp):
    global OUT_FILE

    # set the file to write to
    OUT_FILE = fp


def emit(src):

    if len(BUFFER) >= MAX_BUFFER_LENGTH:
        # write output to a file
        flush()

    BUFFER.append(src)


def flush():
    _write(len(BUFFER))


def _write(length=MAX_BUFFER_LENGTH):

    with open(OUT_FILE, "a") as fp:
        for i in range(length):
            line = BUFFER.pop(0)
            fp.write(f"{line}\n")
