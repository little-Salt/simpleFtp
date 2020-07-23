package client;

import java.io.*;
import java.net.Socket;
import java.net.InetSocketAddress;
import java.util.Optional;

import client.exceptions.ClientInitializationFailureException;
import commands.Command;
import commands.CommandFactory;
import commands.exceptions.IncorrectNumOfArgsException;
import commands.exceptions.InvalidCommandException;
import commands.exceptions.UnExpectResponseMsgException;

/**
 * A ftp client class abstract single connection;
 */
public class FtpClient implements AutoCloseable {
    static final int MAX_LEN = 255;

    static final int CONNECTION_TIMEOUT = 20000;
    static final int TRANSFER_TIMEOUT = 10000;

    static final String CMD_PREFIX = "--> ";
    static final String RSP_PREFIX = "<-- ";

    static final boolean CONTROL_FLOW_FLAG = false;
    static final boolean TRANSFER_FLOW_FLAG = true;

    private final String host;
    private final int port;
    private Socket controlSocket, dataSocket;
    private boolean controlSocket_alive , dataSocket_alive = false;
    private PrintWriter controlWriter;
    private BufferedReader controlReader;

    public FtpClient(String host, int port) {
        this.host = host;
        this.port = port;
    }

    @Override
    public void close() throws Exception {
        if (controlSocket_alive)
            this.controlSocket.close();
        this.controlWriter.close();
        this.controlReader.close();
        if (dataSocket_alive)
            this.dataSocket.close();
    }

    /**
     * the method to start ftp client, and then call other functions,
     * to do sequence jobs, open control socket, prompt massage, parse command and actully run command
     */
    public void start(){
        byte cmdString[] = new byte[MAX_LEN];

        try {
            this.initializeClient();
            this.getResponse();

            for (int len = 1; len > 0; ) {

                System.out.print("simpleFtp> ");
                len = System.in.read(cmdString);
                if (len <= 0)
                    break;

                // Start processing the command here.
                String userInput = new String(cmdString, 0, len).trim();

                // build command
                try {
                    Optional<Command> command = CommandFactory.build(userInput);

                    // if command built, process the command
                    if (command.isPresent()) {
                        this.processCommand(command.get());
                    }

                } catch (InvalidCommandException e) {
                    this.printError(ERROR.E0x001);
                }
            }
        } catch (IOException exception) {
            this.printError(ERROR.E0xFFFE);
        } catch (Exception exception) {
            this.printError(ERROR.E0xFFFF, exception.getMessage());
        }

    }

    /**
     * initialize client, includes open control socket, set up controlWriter and controlReader
     * if fail to initialize, shut down the process
     */
    private void initializeClient() {
        try {
            this.controlSocket = this.openControlSocket();
            if (this.controlSocket == null) throw new ClientInitializationFailureException();
            this.controlSocket_alive = true;
            this.controlWriter = new PrintWriter(controlSocket.getOutputStream(), true);
            this.controlReader = new BufferedReader(new InputStreamReader(controlSocket.getInputStream()));
        } catch (Exception e) {
            this.shutDown();
        }
    }

    /**
     * a wrapper for call acutl method to run command
     * @param command next user command
     * @throws IOException  when System.in encounter some IO Exception
     * @throws UnExpectResponseMsgException throw by some commands
     */
    private void processCommand(Command command) throws IOException, UnExpectResponseMsgException {
        try {
            command.run(this);
        } catch (IncorrectNumOfArgsException e) {
            this.printError(ERROR.E0x002);
        }
    }

    /**
     * Send command through control socket
     */
    public void send(String cmd) {
        this.controlWriter.println(cmd + "\r");
        this.printLog(cmd);
    }

    /**
     * retrieve server response
     * @return server reseponse
     */
    public String getResponse() {
        return this.retrieveResponse(this.controlReader, CONTROL_FLOW_FLAG);
    }

    /**
     * Send command through control socket, return server response
     * @param cmd the command send to ftp server
     * @return the server response
     */
    public String sendAndGet(String cmd) {
        this.send(cmd);
        return this.getResponse();
    }

    /**
     * retrieve response from data socket
     *
     * @return the response
     */
    public String getDataSocketResponse() {
        String rsp = "";
        try (BufferedReader reader = new BufferedReader(new InputStreamReader(this.dataSocket.getInputStream()));) {
            rsp = this.retrieveResponse(reader, TRANSFER_FLOW_FLAG);
        } catch (IOException e) {
            this.printError(ERROR.E0x3A7);
        }
        this.closeCurrentDataSocket();
        return rsp;

    }

    /**
     * save remote file data from current data socket
     *
     * @param fileName
     */
    public void saveFileFromRemote(String fileName) {

        try (DataInputStream reader = new DataInputStream(this.dataSocket.getInputStream());) {

            try (FileOutputStream fileOutput = new FileOutputStream(fileName);) {
                byte[] data = new byte[1024];
                int bytesNum;
                while ((bytesNum = reader.read(data)) != -1) {
                    fileOutput.write(data, 0, bytesNum);
                }
            } catch(FileNotFoundException e) {
                this.printError(ERROR.E0x38E, fileName);
            }

        } catch (IOException e) {
            this.printError(ERROR.E0x3A7);
        } finally {
            this.closeCurrentDataSocket();
        }
    }

    /**
     * open and set proper data scoket to this client
     * @param host host name
     * @param port port number
     * @return return status of set data socket
     */
    public boolean setDataSocket(String host, int port) {
        this.dataSocket = this.openDataSocket(host, port);
        this.dataSocket_alive = (this.dataSocket != null);
        return this.dataSocket_alive;
    }

    /**
     * close current data socket, should call after finish transfer.
     *
     * @throws IOException
     */
    private void closeCurrentDataSocket() {
        try {
            this.dataSocket.close();
        } catch (Exception e) {
        } finally {
            this.dataSocket_alive = false;
        }
    }

    /**
     * open a control socket
     * @return the controlSocket
     */
    private Socket openControlSocket() {
        Socket socket = openSocket(this.host, this.port, CONNECTION_TIMEOUT, CONTROL_FLOW_FLAG);
        return socket;
    }

    /**
     * open data socket
     * @return the data socket
     */
    private Socket openDataSocket(String host, int port) {
        if (this.dataSocket_alive)
            this.closeCurrentDataSocket();
        Socket socket = openSocket(host, port, TRANSFER_TIMEOUT, TRANSFER_FLOW_FLAG);
        return socket;
    }

    /**
     *  open an control or data socket for ftp client and print any error message, handle exception diffterently.
     *
     * @param host the host address
     * @param port the port number
     * @param timeout socket timeout in millisecond
     * @param pasv true for opening data socket
     * @return if open successfully return sockect, otherwise for data socket return null
     *          for control socket exit programm with code -1.
     */
    private Socket openSocket(String host, int port, int timeout, boolean pasv) {
        Socket socket = new Socket();

        try{
            socket.connect(new InetSocketAddress(host, port), timeout);
        } catch(Exception e) {
            if (pasv) {
                this.printError(ERROR.E0x3A2, host, port);
            } else {
                this.printError(ERROR.E0xFFFC, host, port);
            }
            socket = null;
        }
        return socket;
    }

    /**
     * retrieve response from given buffer reader and append response to one string,
     * handler exception differently based on type of socket
     * @param reader the buffer reader contain the response
     * @param pasv flag for socket type, true for data socket, false for control socket
     * @return the response string
     */
    private String retrieveResponse(BufferedReader reader, boolean pasv) {
        String rst = "";
        try {
            String rsp = reader.readLine();
            while (rsp != null) {
                this.printResponse(rsp);
                rst = rst.concat(rsp);
                if(rsp.matches("(\\d{3})\\s.+"))
                    break;
                rsp = reader.readLine();
            }
        } catch(IOException e) {
            if (pasv) {
                this.printError(ERROR.E0x3A7);
                this.closeCurrentDataSocket();
            } else {
                this.printError(ERROR.E0xFFFD);
                this.shutDown();
            }
        }
        return rst;
    }

    private void printLog(String cmd) {
        System.out.println(CMD_PREFIX + cmd);
    }

    private void printResponse(String rsp) {
        System.out.println(RSP_PREFIX + rsp);
    }

    private void printError(ERROR err) {
        System.out.println(err.getMsg());
    }

    private void printError(ERROR err, Object... args) {
        System.out.format(err.getMsg(), args);
    }

    /**
     * after close all resourses, terminamte program, only be used when exception happen
     */
    private void shutDown() {
        try{
            this.close();
        } catch(Exception e) {
        } finally {
            System.exit(-1);
        }
    }

}
