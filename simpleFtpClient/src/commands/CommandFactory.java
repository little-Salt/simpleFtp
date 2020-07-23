package commands;

import commands.exceptions.InvalidCommandException;

import java.util.Optional;

public  class CommandFactory {

    public static Optional<Command> build(String userInput) throws InvalidCommandException {
        // parse, skip empty and special case
        if (userInput.isEmpty() || userInput.startsWith("#"))
            return Optional.empty();

        String[] userInputs = userInput.split("\\s+");

        switch(userInputs[0]) {
            case "user":
                return Optional.of(new UserCommand(userInputs));
            case "pw":
                return Optional.of(new PassCommand(userInputs));
            case "quit":
                return Optional.of(new QuitCommand(userInputs));
            case "get":
                return Optional.of(new GetCommand(userInputs));
            case "features":
                return Optional.of(new FeaturesCommand(userInputs));
            case "cd":
                return Optional.of(new CdCommand(userInputs));
            case "dir":
                return Optional.of(new DirCommand(userInputs));
            default:
                throw new InvalidCommandException();
        }
    }
}