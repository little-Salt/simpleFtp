package commands;

import client.FtpClient;
import commands.exceptions.IncorrectNumOfArgsException;

public class QuitCommand extends Command {
    final String QUIT_CMD = "QUIT";
    final int ARGS_NUM = 1;

    public QuitCommand(String[] userInputs) {
        super(userInputs);
    }

    @Override
    public void run(FtpClient client) throws IncorrectNumOfArgsException {
        this.validate(ARGS_NUM);
        client.sendAndGet(QUIT_CMD);
        System.exit(0);
    }
}
