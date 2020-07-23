package client;

public enum ERROR {
    E0x001("0x001 Invalid command"),
    E0x002("0x002 Incorrect number of arguments"),
    E0x38E("0x38E Access to local file %s denied.\n"),
    E0xFFFC("0xFFFC Control connection to %s on port %d failed to open.\n"),
    E0xFFFD("0xFFFD Control connection I/O error, closing control connection."),
    E0x3A2("0x3A2 Data transfer connection to %s on port %d failed to open.\n"),
    E0x3A7("0x3A7 Data transfer connection I/O error, closing data connection."),
    E0xFFFE("0xFFFE Input error while reading commands"),
    E0xFFFF("0xFFFF Processing error. %d\n");

    private String msg;

    public String getMsg() {
        return this.msg;
    }

    private ERROR(String msg) {
        this.msg = msg;
    }
}
