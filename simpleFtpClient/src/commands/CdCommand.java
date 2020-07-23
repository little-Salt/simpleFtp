package commands;

import client.FtpClient;
import commands.exceptions.IncorrectNumOfArgsException;

public class CdCommand extends Command {
    final String CWD_CMD = "CWD";
    final int ARGS_NUM = 2;

    public CdCommand(String[] userInputs) {
        super(userInputs);
    }

    @Override
    public void run(FtpClient client) throws IncorrectNumOfArgsException {
        this.validate(ARGS_NUM);
        client.sendAndGet(this.getCommandWithArg(CWD_CMD));
    }

}
