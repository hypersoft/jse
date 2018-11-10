
var line = 0;

function prompt() {
	return '{' + Environment.USER + ': ' + localPath().split("/").pop() + "} ";
}

rel: while (++line) {
	if (script = readline(prompt())) {
		while (1) {
			script += '\n';
			try {
				checkSyntax(script, parameter[0], line);
			} catch (exception) {
				if (exception.message === "Unexpected end of script") {
					script += readline("> ");
					continue;
				} else {
					printErrorLine(exception);
					continue rel;
				}
			}
			break;
		}
		try {
			eval(script);
		} catch (exception) {
			printErrorLine(exception);
			continue rel;
		}
	} else break;
}
