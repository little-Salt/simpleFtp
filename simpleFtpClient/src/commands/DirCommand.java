package commands;

import client.FtpClient;
import commands.exceptions.IncorrectNumOfArgsException;
import commands.exceptions.UnExpectResponseMsgException;

public class DirCommand extends Command {
    final String LIST_CMD = "NLST";
    final int ARGS_NUM = 1;

    public DirCommand(String[] userInputs) {
        super(userInputs);
    }

    @Override
    public void run(FtpClient client) throws UnExpectResponseMsgException, IncorrectNumOfArgsException {
        this.validate(ARGS_NUM);
        if(!this.requestServerEnterPasvMode(client))
            return;
        client.sendAndGet(LIST_CMD);
        client.getDataSocketResponse();
        client.getResponse();
    }

}
