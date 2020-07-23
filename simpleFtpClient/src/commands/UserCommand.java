package commands;

import client.FtpClient;
import commands.exceptions.IncorrectNumOfArgsException;

import java.io.IOException;

public class UserCommand extends Command {
    final String USER_CMD = "USER";
    final String PASS_REQUEST_CODE = "331";

    final int ARGS_NUM = 2;
    final int MAX_LEN = 255;

    public UserCommand(String[] userInputs) {
        super(userInputs);
    }

    @Override
    public void run(FtpClient client) throws IncorrectNumOfArgsException, IOException {
        this.validate(ARGS_NUM);
        String rsp = client.sendAndGet(this.getCommandWithArg(USER_CMD));
        if (rsp.startsWith(PASS_REQUEST_CODE)&&rsp.contains("password"))
            this.requestPass(client);
    }

    private void requestPass(FtpClient client) throws IncorrectNumOfArgsException, IOException {
        System.out.print("Password: ");
        byte[] input = new byte[MAX_LEN];
        int len = System.in.read(input);
        String pass = new String(input, 0, len).trim();
        new PassCommand(new String[]{"PASS_CMD", pass}).run(client);
    }

}
