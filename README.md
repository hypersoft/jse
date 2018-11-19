### About JSE

Hypersoft-Systems: U.-S.-A.: JSE (Java-Script-Engine) is a free (as in cost of 
purchase) and open-source computer software project for executing shell-scripts 
written in JavaScript as full-fledged-software-programs within a Linux Operating System environment.

JSE is conceptualized and maintained by [Hypersoft-Systems: U.-S.-A.](https://github.com/hypersoft/) [(Triston J.Taylor)](mailto:pc.wiz.tt@gmail.com).

[Donations](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=DG3H6F8DSG4BC)
in support of this project are welcome and are processed via pay-pal.

In general, JSE IS a full-fledged-application-development-suite. If JSE does not
have the functionality you require built-in, you can simply create a plugin written
in the C programming language (or compatible) to provide the needed 
functionality. In fact, JSE itself is built on the use of plugins and scripts 
to provide all of its core functionalities.

### Design Methodology

  - Let JavaScript do Unix things, in The [Unix Way](http://en.wikipedia.org/wiki/Unix_philosophy)
  - JavaScript interpretation as a System Level coding package
  - HTML Browsing as a System Level application package
  - Zero external runtime environment support requirements (package tools, script-libraries, etc.)

### Technology

JSE itself, depends upon several technologies to achieve its design goals.

  - WebKitGTK+ 4.0 (for JavaScript interpretation and HTML Rendering)
  - DynCall 0.8 \[built-in]
  - GNU ReadLine (for interactive-sessions)

JSE's build environment adds a few requirements:

  - Bash Shell 4.0+
  - curl \[to download the DynCall library package and documentation]
  - Git \[if you use software revision control]
  - Tar and Zip support \[for unpacking the DynCall archive and creating archives]

### Licensing

JSE has a host of licensing options for you to choose from. The project does not
wish for legal issues to be the cause of not using JSE, so [the license for JSE](share/license/license.txt)
is very loose. Please review [the licensing for all other included technologies](share/license),
if you are experiencing a legal permissions issue.

### Features

JSE provides many features to enhance your ability to write JavaScript based shell-scripts and
HTML Applications for the Linux platform. You can use JSE to do any of the following:

  - Write Basic JavaScripts (With JSE's Shell support)
  - Write Advanced JavaScripts (Using shared-system-libraries and JSE Plugins)
  - Create, Design and Debug: HTML Applications (with Advanced or Basic JavaScripting support)
  - Interactive Session (basic-example)

### Over the Internet Live Installation Script

You can very easily add JSE support to your OS with the following script,
provided you meet the system requirements.

```bash
wget -O jse.zip https://github.com/hypersoft/jse/archive/master.zip;
unzip jse.zip;
mv jse-master jse;
cd jse;
make && sudo make install && cd .. && rm -rf jse{,.zip};
```

### HTML Applications

HTML Applications can greatly improve your productivity. HTML Applications backed
by JSE have truly unlimited power and flexibility.

This Screenshot demonstrates the Awesomeness of JSE and its Ghtml counter-part.
The application shown within Ghtml is a manual JavaScript testing application
which is distributed with the source code of JSE, and can be found in the ./test
folder.

![Ghtml Testbed Screenshot](data/Screenshot_2018-11-18_20-42-20.png "Ghtml Testbed")

Powerful applications can easily be written in no-time-at-all using JSE and Ghtml.
To execute the application shown in the Screenshot yourself, simply run:

    $ demo/testbed

from a terminal session within the project's root directory after compiling and
installing the project.

### Security

<p>JSE is a formidable coding platform. So formidable that it could INSTANTLY be
  as dangerous as allowing full command access to a C compiler. To protect host
  systems, JSE employs a simple-file-system-security-contract:</p>

  - Pipes will not be executed with enhanced execution features or plugins
  - A user must have execute permission on the file being executed to enable enhanced execution features and plugins

<p>Note: 
A developer may specify that remote content be loaded, from within an executable file.
This is an intentional security leak on behalf of the execution of the developer's implementation.
Neither Ghtml nor JSE will attempt to prohibit downloading or execution of remote content, and no sand-boxing-implementation of mixed content is provided by this project at this time.
</p>

<hr>

### Recommended Reading for Script Writers
* [Wikipedia: JavaScript](http://en.wikipedia.org/wiki/JavaScript)
* [MDN: JavaScript](https://developer.mozilla.org/en-US/docs/Web/JavaScript)
* [JavaScript: The Definitive Guide](http://www.amazon.com/JavaScript-Definitive-Guide-David-Flanagan/dp/0596000480)
