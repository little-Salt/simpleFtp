package commands;

import client.FtpClient;
import commands.exceptions.IncorrectNumOfArgsException;

public class FeaturesCommand extends Command {
    final String FEAT_CMD = "FEAT";
    final int ARGS_NUM = 1;

    public FeaturesCommand(String[] userInputs) {
        super(userInputs);
    }

    @Override
    public void run(FtpClient client) throws IncorrectNumOfArgsException {
        this.validate(ARGS_NUM);
        client.sendAndGet(FEAT_CMD);
    }

}
