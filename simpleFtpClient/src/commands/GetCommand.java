package commands;

import client.FtpClient;
import commands.exceptions.IncorrectNumOfArgsException;
import commands.exceptions.UnExpectResponseMsgException;

public class GetCommand extends Command {
    final String RETR_CMD = "RETR";
    final String RETR_SUCCESS_CODE = "150";
    final int ARGS_NUM = 2;

    public GetCommand(String[] userInputs) {
        super(userInputs);
    }

    @Override
    public void run(FtpClient client) throws IncorrectNumOfArgsException, UnExpectResponseMsgException {
        this.validate(ARGS_NUM);
        if(!this.requestServerEnterPasvMode(client))
            return;
        String rsp = client.sendAndGet(this.getCommandWithArg(RETR_CMD));
        if(!rsp.startsWith(RETR_SUCCESS_CODE))
            return;
        client.saveFileFromRemote(this.userInputs[1]);
        client.getResponse();
    }

}
