import java.lang.System;
import client.FtpClient;

//
// This is an implementation of a simplified version of a command
// line ftp client. The program always takes two arguments
//

public class Main {
    static final int ARG_CNT = 2;

    static final int DEFAULT_PORT = 21;

    public static void main(String [] args) {

        // Get command line arguments and connected to FTP
        // If the arguments are invalid or there aren't enough of them then exit.

//         if (args.length == 0 || args.length > ARG_CNT) {
//             System.out.print("Usage: ServerAddress [ServerPort]\n");
//             System.exit(-1);
//         }

//        String host = args[0];
//        int port = args.length == ARG_CNT? Integer.parseInt(args[1]) : DEFAULT_PORT;
        String host = "localhost";
        int port = 23232;
        try (FtpClient ftpClient = new FtpClient(host, port);) {
            ftpClient.start();
        } catch (Exception e) {
            System.out.format("Unexpected exception: %s\n", e.getMessage());
        }
    }
}