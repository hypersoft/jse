/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

var line = 0;

rel: while (++line) {
	if (script = readline('{' + Environment.USER + '@' + Environment.PWD.split("/").pop() + "} ")) {
		while (1) {
			script += '\n';
			try {
				checkSyntax(script, argv[0], line);
			} catch (exception) {
				if (exception.message === "Unexpected end of script") {
					script += readline("> ");
					continue;					
				} else {
					error(exception);
					continue rel;
				}
			}
			break;
		}
		try {
			eval(script);
		} catch (exception) {
			error(exception);
			continue rel;
		}
	} else break;
}
