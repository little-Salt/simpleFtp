package commands;

import java.io.IOException;
import java.util.Arrays;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import client.FtpClient;
import commands.exceptions.IncorrectNumOfArgsException;
import commands.exceptions.UnExpectResponseMsgException;

public abstract class Command {
    final String PASV_CMD = "PASV";
    final String PASV_SUCCESS_CODE = "227";

    public String[] userInputs;

    public Command(String[] userInputs) {
        this.userInputs = userInputs;
    }

    protected void validate(int argsNum) throws IncorrectNumOfArgsException {
        if (this.userInputs.length != argsNum)
            throw new IncorrectNumOfArgsException();
    };

    public String getCommandWithArg(String cmd) {
        return cmd + " " + this.userInputs[1];
    }

    /**
     * request client send pasv mode command to server and set client properly after get response
     * @param client the client
     * @return return result of request, true means successfully enter pasv mode
     * @throws UnExpectResponseMsgException throw exception when can not found ip and port in success responses
     */
    protected boolean requestServerEnterPasvMode(FtpClient client) throws UnExpectResponseMsgException {
        String rsp = client.sendAndGet(PASV_CMD);
        if (rsp.startsWith(PASV_SUCCESS_CODE))
            return this.setClientDataSocket(client, rsp);
        return false;
    }

    /**
     * based on server success reponse message to open data socket and set client properly
     * @param client the ftp client
     * @param rsp the success server respomse message
     * @return return result of request, true means successfully open data socket for the client
     * @throws UnExpectResponseMsgException
     */
    private boolean setClientDataSocket(FtpClient client, String rsp) throws UnExpectResponseMsgException {
        Matcher matcher = Pattern.compile("\\((\\d+,){5}\\d+\\)").matcher(rsp);
        if (!matcher.find())
            throw new UnExpectResponseMsgException("Not found ip and port in success PASV command server response");
        String[] address = rsp.substring(matcher.start() + 1, matcher.end() - 1).split(",");
        String host = String.join(".", Arrays.copyOfRange(address, 0, 3 + 1));
        int port = Integer.parseInt(address[4]) * 256 + Integer.parseInt(address[5]);
        return client.setDataSocket(host, port);
    }

    /**
     * an abstract method, each concrete class should implement it with correct logic to run its own command
     * @param client the client want to process this command
     * @throws IncorrectNumOfArgsException the number of arguments for command is incorrect
     * @throws IOException throw by some command
     * @throws UnExpectResponseMsgException throw by some command who requir server enter pasv mode
     */
    public abstract void run(FtpClient client) throws IncorrectNumOfArgsException, IOException, UnExpectResponseMsgException;
}
