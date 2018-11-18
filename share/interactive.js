
var interactiveLineNumber = 0;
var interactiveReporting = true;

function prompt() {
	return '[' + Environment.USER + ': ' + localPath().split("/").pop() + "] ";
}

readEvalLoop: while (++interactiveLineNumber) {
	if ((script = readLine(prompt())) !== -1) {
		while (1) {
			script += '\n';
			try {
				checkSyntax(script, parameter[0], interactiveLineNumber);
			} catch (exception) {
				if (exception.message === "Unexpected end of script") {
					script += readline("> ");
					continue;
				} else {
					printErrorLine(exception);
					continue readEvalLoop;
				}
			}
			break;
		}
		try {
			var r = eval(script);
			if (interactiveReporting) printErrorLine(String(r).chomp());
		} catch (exception) {
			printErrorLine(exception + ": " + exception.stack.toString());
			continue readEvalLoop;
		}
	} else exit(0);
}
