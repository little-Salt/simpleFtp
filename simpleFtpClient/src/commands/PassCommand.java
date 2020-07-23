package commands;

import client.FtpClient;
import commands.exceptions.IncorrectNumOfArgsException;

public class PassCommand extends Command {
    final String PASS_CMD = "PASS";
    final int ARGS_NUM = 2;

    public PassCommand(String[] userInputs) {
        super(userInputs);
    }

    @Override
    public void run(FtpClient client) throws IncorrectNumOfArgsException {
        this.validate(ARGS_NUM);
        client.sendAndGet(this.getCommandWithArg(PASS_CMD));
    }

}
